#include "mpi.h"
#include "ruby/ruby.h"
#include <stdarg.h>
#include "global.h"

struct mpi_request {
    MPI_Request *req;
    MPI_Comm *comm;
    req_type type;
    VALUE obj;
    int peer, tag;
    /* Transmission is always a two stage process: 
       - Getting or sending the buffer size
       - Getting or sending the actual data
     */
    enum {TX_SIZE, TX_DATA} stage;
    int buflen;
    char *buf;
};

VALUE cRequest;

#define INITIAL_RLIST_CAP   16
static int capacity = INITIAL_RLIST_CAP;
static struct mpi_request **rlist = NULL;
static void request_watch(struct mpi_request *mr)
{
    int i;

    for (i = 0; i < capacity; i++) {
        if (rlist[i] == NULL) {
            rlist[i] = mr;
            break;
        }
    }

    if (i == capacity) {
        capacity *= 2;
        REALLOC_N(rlist, struct mpi_request *, capacity);
        rlist[i] = mr;
    }
}

static void request_unwatch(struct mpi_request *mr)
{
    int i;

    for (i = 0; i < capacity; i++) {
        if (rlist[i] == mr) {
            rlist[i] = NULL;
            break;
        }
    }
}

static VALUE request_thread(void *data)
{
    int rv, i, flag;
    MPI_Status stat;

    while (1) {
        for (i = 0; i < capacity; i++) {
            if (rlist[i] != NULL) {
                rv = MPI_Test(rlist[i]->req, &flag, &stat);
                mpi_exception(rv);

                if (flag) {
                    /* XXX: Is it ok to ignore the status? */
                    /* cilibrar */ rlist[i]->peer = stat.MPI_SOURCE;
                    free(rlist[i]->buf);
                    rlist[i]->buf = ALLOC_N(char, rlist[i]->buflen + 1);
                    MPI_Irecv(rlist[i]->buf, rlist[i]->buflen + 1, MPI_BYTE,
                              rlist[i]->peer, rlist[i]->tag, *rlist[i]->comm,
                              rlist[i]->req);
                    rlist[i]->stage = TX_DATA;
                    rlist[i] = NULL;
                }
            }
        }
        rb_thread_schedule();
    }

    return Qtrue;
}

static void request_mark(struct mpi_request *mr)
{
    rb_gc_mark(mr->obj);
}

static void request_free(struct mpi_request *mr)
{
    int rv;

    rv = MPI_Request_free(mr->req);
    mpi_exception(rv);
    
    free(mr->req);
    free(mr->buf);
    free(mr);
}

/*
   request_new(NORMAL, MPI_Request *req);
   request_new(P[BSR]SEND, VALUE obj, int dest, int tag, MPI_Comm *comm);
   request_new(RECV, int src, int tag, MPI_Comm *comm);
*/

VALUE request_new(req_type type, ...)
{
    va_list args;
    struct mpi_request *mr;
    VALUE tdata;

    mr = ALLOC(struct mpi_request);
    mr->type = type;
    mr->obj = Qnil;

    va_start(args, type);
    switch (type) {
        case PSEND:
        case PBSEND:
        case PSSEND:
        case PRSEND:
            mr->obj = va_arg(args, VALUE);
            /* fall through */
        case RECV:
            mr->req = ALLOC(MPI_Request);
            mr->peer = va_arg(args, int);
            mr->tag = va_arg(args, int);
            mr->comm = va_arg(args, MPI_Comm *);
            mr->stage = TX_SIZE;
            mr->buflen = 0;
            mr->buf = NULL;
            break;
        case NORMAL:
            mr->req = va_arg(args, MPI_Request *);
            break;
    }
    va_end(args);

    tdata = Data_Wrap_Struct(cRequest, request_mark, request_free, mr);
    rb_obj_call_init(tdata, 0, NULL);

    return tdata;
}

static VALUE request_wait(VALUE self)
{
    int rv;
    struct mpi_request *mr;
    MPI_Status *stat;

    Data_Get_Struct(self, struct mpi_request, mr);

    stat = ALLOC(MPI_Status);

    if ((mr->type == RECV) && (mr->stage == TX_SIZE)) {
        rv = MPI_Wait(mr->req, stat);
        mpi_exception(rv);

        free(mr->buf);
        mr->buf = ALLOC_N(char, mr->buflen + 1);

        rv = MPI_Irecv(mr->buf, mr->buflen + 1, MPI_BYTE, mr->peer, mr->tag, 
                       *mr->comm, mr->req);
        mpi_exception(rv);

        request_unwatch(mr);

        mr->stage = TX_DATA;
    }

    rv = MPI_Wait(mr->req, stat);
    /* cilibrar */ mr->peer = stat->MPI_SOURCE;
    mpi_exception(rv);

    if (mr->type == RECV) {
        VALUE str;

        str = rb_str_new2(mr->buf);
        mr->obj = rb_funcall(mMarshal, id_load, 1, str);
        free(mr->buf);
        mr->buf = NULL;
    }

    return status_new(stat);
}

static VALUE request_waitany(VALUE self, VALUE rreqs)
{
    int rv;
    int i, length, no_sizereqs = 0;
    MPI_Status *stat;
    MPI_Request *reqs, *sizereqs;
    struct mpi_request *mr;

    length = RARRAY(rreqs)->len;
    reqs = ALLOCA_N(MPI_Request, length);
    sizereqs = ALLOCA_N(MPI_Request, length);
    stat = ALLOC(MPI_Status);

    for (i = 0; i < length; i++) {
        Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);

        /* Only requests that are not receives in the receive size stage */
        /* can possibly be ready to go. */
        if ((mr->type == RECV) && (mr->stage == TX_SIZE)) {
            reqs[i] = MPI_REQUEST_NULL;
            sizereqs[i] = *mr->req;
            no_sizereqs++;
        } else {
            reqs[i] = *mr->req;
            sizereqs[i] = MPI_REQUEST_NULL;
        }
    }

    if (no_sizereqs == length) {
        VALUE str;

        /* First to finish getting their length wins */
        rv = MPI_Waitany(length, sizereqs, &i, stat);
        mpi_exception(rv);

        if (i == MPI_UNDEFINED)
            return UNDEFINED;

        free(mr->buf);
        mr->buf = ALLOC_N(char, mr->buflen + 1);

        rv = MPI_Irecv(mr->buf, mr->buflen + 1, MPI_BYTE, mr->peer, mr->tag, 
                       *mr->comm, mr->req);
        mpi_exception(rv);

        MPI_Wait(&sizereqs[i], stat);
        mpi_exception(rv);

        Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
        if (sizereqs[i] == MPI_REQUEST_NULL) 
            *mr->req = MPI_REQUEST_NULL;

        str = rb_str_new2(mr->buf);
        mr->obj = rb_funcall(mMarshal, id_load, 1, str);
        free(mr->buf);
        mr->buf = NULL;

        request_unwatch(mr);
    } else {
        rv = MPI_Waitany(length, reqs, &i, stat);
        mpi_exception(rv);

        if (i == MPI_UNDEFINED)
            return UNDEFINED;

        if (reqs[i] == MPI_REQUEST_NULL) {
            /* assign the request in rreqs to be request null */
            Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
            *mr->req = MPI_REQUEST_NULL;
        }
    }

    return rb_ary_new3(2, rb_fix_new(i), status_new(stat));
}

static VALUE request_waitall(VALUE self, VALUE rreqs)
{
    int rv;
    int i, length, have_sizereqs = 0;
    MPI_Status *stats;
    MPI_Request *reqs, *sizereqs;
    VALUE rstats;
    struct mpi_request *mr;

    length = RARRAY(rreqs)->len;
    reqs = ALLOCA_N(MPI_Request, length);
    sizereqs = ALLOCA_N(MPI_Request, length);
    stats = ALLOCA_N(MPI_Status, length);

    for (i = 0; i < length; i++) {
        Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
        if ((mr->type == RECV) && (mr->stage == TX_SIZE)) {
            sizereqs[i] = *mr->req;
            have_sizereqs = 1;
        } else {
            sizereqs[i] = MPI_REQUEST_NULL;
        }

        reqs[i] = *mr->req;
    }

    /* If any of the requests are for receivers still waiting on buffers, we 
       have to wait on those first. */
    if (have_sizereqs) {
        rv = MPI_Waitall(length, sizereqs, stats);
        mpi_exception(rv);
        for (i = 0; i < length; i++) {
            Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
            if ((mr->type == RECV) && (mr->stage == TX_SIZE)) {
                free(mr->buf);

                mr->buf = ALLOC_N(char, mr->buflen + 1);
                rv = MPI_Irecv(mr->buf, mr->buflen + 1, MPI_BYTE, mr->peer, 
                        mr->tag, *mr->comm, mr->req);
                mpi_exception(rv);

                mr->stage = TX_DATA;
                request_unwatch(mr);
            }
            reqs[i] = *mr->req;
        }
    }

    rv = MPI_Waitall(length, reqs, stats);
    mpi_exception(rv);

    rstats = rb_ary_new2(length);
    for (i = 0; i < length; i++) {
        MPI_Status *stat;

        if (reqs[i] == MPI_REQUEST_NULL) {
            /* assign the request in rreqs to be request null */
            Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
            *mr->req = MPI_REQUEST_NULL;
        }

        stat = ALLOC(MPI_Status);
        *stat = stats[i];
        rb_ary_store(rstats, i, status_new(stat));
    }

    return rstats;
}

static VALUE request_waitsome(VALUE self, VALUE rreqs)
{
    int rv;
    int i, length;
    int outcount;
    int *indices;
    MPI_Status *stats;
    MPI_Request *reqs;
    VALUE routreqs, rstats;

    length = RARRAY(rreqs)->len;
    reqs = ALLOCA_N(MPI_Request, length);
    stats = ALLOCA_N(MPI_Status, length);
    indices = ALLOCA_N(int, length);

    for (i = 0; i < length; i++) {
        struct mpi_request *req;

        Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, req);
        reqs[i] = *req->req;
    }

    rv = MPI_Waitsome(length, reqs, &outcount, indices, stats);
    mpi_exception(rv);

    if (outcount == MPI_UNDEFINED)
        return UNDEFINED;

    routreqs = rb_ary_new2(outcount);
    rstats = rb_ary_new2(outcount);
    for (i = 0; i < outcount; i++) {
        MPI_Status *stat;

        if (reqs[indices[i]] == MPI_REQUEST_NULL) {
            struct mpi_request *req;

            /* assign the request in rreqs to be request null */
            Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, req);
            *req->req = MPI_REQUEST_NULL;
        }

        rb_ary_store(routreqs, i, rb_ary_entry(rreqs, indices[i]));

        stat = ALLOC(MPI_Status);
        *stat = stats[indices[i]];
        rb_ary_store(rstats, i, status_new(stat));
    }

    return rb_ary_new3(2, routreqs, rstats);
}

static VALUE request_test(VALUE self)
{
    int rv, flag;
    MPI_Status *stat;
    struct mpi_request *mr;

    Data_Get_Struct(self, struct mpi_request, mr);

    if ((mr->type == RECV) && (mr->stage == TX_SIZE))
        return rb_ary_new3(2, Qfalse, Qnil);

    stat = ALLOC(MPI_Status);

    rv = MPI_Test(mr->req, &flag, stat);
    mpi_exception(rv);

    return rb_ary_new3(2, flag ? Qtrue: Qfalse, status_new(stat));
}

static VALUE request_testany(VALUE self, VALUE rreqs)
{
    int rv, flag;
    int i, length;
    MPI_Status *stat;
    MPI_Request *reqs;
    struct mpi_request *mr;

    length = RARRAY(rreqs)->len;
    reqs = ALLOCA_N(MPI_Request, length);
    stat = ALLOC(MPI_Status);

    for (i = 0; i < length; i++) {

        Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
        if ((mr->type == RECV) && (mr->stage == TX_SIZE))
            reqs[i] = MPI_REQUEST_NULL;
        else
            reqs[i] = *mr->req;
    }

    rv = MPI_Testany(length, reqs, &i, &flag, stat);
    mpi_exception(rv);

    if (i == MPI_UNDEFINED)
        return UNDEFINED;

    if (reqs[i] == MPI_REQUEST_NULL) {
        /* assign the request in rreqs to be request null */
        Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
        *mr->req = MPI_REQUEST_NULL;
    }

    return rb_ary_new3(3, rb_fix_new(i), flag ? Qtrue:Qfalse, status_new(stat));
}

static VALUE request_testall(VALUE self, VALUE rreqs)
{
    int rv, flag;
    int i, length;
    MPI_Status *stats;
    MPI_Request *reqs;
    VALUE rstats;
    struct mpi_request *mr;

    length = RARRAY(rreqs)->len;
    reqs = ALLOCA_N(MPI_Request, length);
    stats = ALLOCA_N(MPI_Status, length);

    for (i = 0; i < length; i++) {
        Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
        if ((mr->type == RECV) && (mr->stage == TX_SIZE))
            return rb_ary_new3(2, Qfalse, Qnil);
            
        reqs[i] = *mr->req;
    }

    rv = MPI_Testall(length, reqs, &flag, stats);
    mpi_exception(rv);

    rstats = rb_ary_new2(length);
    for (i = 0; i < length; i++) {
        MPI_Status *stat;

        if (reqs[i] == MPI_REQUEST_NULL) {
            /* assign the request in rreqs to be request null */
            Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
            *mr->req = MPI_REQUEST_NULL;
        }

        stat = ALLOC(MPI_Status);
        *stat = stats[i];
        rb_ary_store(rstats, i, status_new(stat));
    }

    return rb_ary_new3(2, flag ? Qtrue : Qfalse, rstats);
}

static VALUE request_testsome(VALUE self, VALUE rreqs)
{
    int rv;
    int i, length;
    int outcount;
    int *indices;
    MPI_Status *stats;
    MPI_Request *reqs;
    VALUE routreqs, rstats;
    struct mpi_request *mr;

    length = RARRAY(rreqs)->len;
    reqs = ALLOCA_N(MPI_Request, length);
    stats = ALLOCA_N(MPI_Status, length);
    indices = ALLOCA_N(int, length);

    for (i = 0; i < length; i++) {

        Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
        reqs[i] = *mr->req;
    }

    rv = MPI_Testsome(length, reqs, &outcount, indices, stats);
    mpi_exception(rv);

    if (i == MPI_UNDEFINED)
        return UNDEFINED;

    routreqs = rb_ary_new2(outcount);
    rstats = rb_ary_new2(outcount);
    for (i = 0; i < outcount; i++) {
        MPI_Status *stat;

        if (reqs[indices[i]] == MPI_REQUEST_NULL) {
            /* assign the request in rreqs to be request null */
            Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
            *mr->req = MPI_REQUEST_NULL;
        }

        rb_ary_store(routreqs, i, rb_ary_entry(rreqs, indices[i]));

        stat = ALLOC(MPI_Status);
        *stat = stats[indices[i]];
        rb_ary_store(rstats, i, status_new(stat));
    }

    return rb_ary_new3(2, routreqs, rstats);
}

static VALUE request_cancel(VALUE self)
{
    int rv;
    struct mpi_request *mr;

    Data_Get_Struct(self, struct mpi_request, mr);

    switch (mr->type) {
        case RECV:
            mr->stage = TX_SIZE;
            free(mr->buf);
            mr->buf = NULL;
            request_unwatch(mr);
        case PSEND:
        case PBSEND:
        case PSSEND:
        case PRSEND:
        case NORMAL:
            rv = MPI_Cancel(mr->req);
            mpi_exception(rv);
            break;
    }

    return Qtrue;
}

VALUE request_start(VALUE self)
{
    int rv;
    VALUE dump;
    MPI_Request tmp;
    struct mpi_request *mr;

    Data_Get_Struct(self, struct mpi_request, mr);

    switch (mr->type) {
        case PSEND:
            dump = rb_funcall(mMarshal, id_dump, 1, mr->obj);
            free(mr->buf);
            mr->buf = rb_str2cstr(dump, &mr->buflen);

            rv = MPI_Isend(&mr->buflen, 1, MPI_INT, mr->peer, mr->tag, 
                           *mr->comm, &tmp);
            mpi_exception(rv);

            rv = MPI_Isend(&mr->buf, mr->buflen, MPI_BYTE, mr->peer, mr->tag, 
                           *mr->comm, mr->req);
            mpi_exception(rv);
            break;
        case PBSEND:
            dump = rb_funcall(mMarshal, id_dump, 1, mr->obj);
            free(mr->buf);
            mr->buf = rb_str2cstr(dump, &mr->buflen);

            rv = MPI_Ibsend(&mr->buflen, 1, MPI_INT, mr->peer, mr->tag, 
                           *mr->comm, &tmp);
            mpi_exception(rv);

            rv = MPI_Ibsend(&mr->buf, mr->buflen, MPI_BYTE, mr->peer, mr->tag, 
                           *mr->comm, mr->req);
            mpi_exception(rv);
            break;
        case PSSEND:
            dump = rb_funcall(mMarshal, id_dump, 1, mr->obj);
            free(mr->buf);
            mr->buf = rb_str2cstr(dump, &mr->buflen);

            rv = MPI_Issend(&mr->buflen, 1, MPI_INT, mr->peer, mr->tag, 
                           *mr->comm, &tmp);
            mpi_exception(rv);

            rv = MPI_Issend(&mr->buf, mr->buflen, MPI_BYTE, mr->peer, mr->tag, 
                           *mr->comm, mr->req);
            mpi_exception(rv);
            break;
        case PRSEND:
            dump = rb_funcall(mMarshal, id_dump, 1, mr->obj);
            free(mr->buf);
            mr->buf = rb_str2cstr(dump, &mr->buflen);

            rv = MPI_Irsend(&mr->buflen, 1, MPI_INT, mr->peer, mr->tag, 
                           *mr->comm, &tmp);
            mpi_exception(rv);

            rv = MPI_Irsend(&mr->buf, mr->buflen, MPI_BYTE, mr->peer, mr->tag, 
                           *mr->comm, mr->req);
            mpi_exception(rv);
            break;
        case RECV:
            rv = MPI_Irecv(&mr->buflen, 1, MPI_INT, mr->peer, mr->tag,
                           *mr->comm, mr->req);
            mpi_exception(rv);
            request_watch(mr);
            break;
        case NORMAL:
            rv = MPI_Start(mr->req);
            mpi_exception(rv);
            break;
    }

    return Qtrue;
}

static VALUE request_startall(VALUE self, VALUE rreqs)
{
    int i, rv, length, noreqs = 0;
    MPI_Request *reqs;

    length = RARRAY(rreqs)->len;
    reqs = ALLOCA_N(MPI_Request, length);

    for (i = 0; i < length; i++) {
        struct mpi_request *mr;

        Data_Get_Struct(rb_ary_entry(rreqs, i), struct mpi_request, mr);
        if (mr->type == NORMAL)
            reqs[noreqs++] = *mr->req;
        else
            request_start(self);
    }

    rv = MPI_Startall(noreqs, reqs);
    mpi_exception(rv);

    return Qtrue;
}

static VALUE request_null_p(VALUE self)
{
    struct mpi_request *mr;

    Data_Get_Struct(self, struct mpi_request, mr);

    return *mr->req == MPI_REQUEST_NULL;
}

static VALUE request_object(VALUE self)
{
    struct mpi_request *mr;

    Data_Get_Struct(self, struct mpi_request, mr);

    return mr->obj;
}

void Init_Request()
{
    rlist = ALLOC_N(struct mpi_request *, INITIAL_RLIST_CAP);
    MEMZERO(rlist, struct mpi_request *, INITIAL_RLIST_CAP);
    rb_thread_create(request_thread, NULL);
    
    cRequest = rb_define_class_under(mMPI, "Request", rb_cObject);
    rb_define_method(cRequest, "wait", request_wait, 0);
    rb_define_singleton_method(cRequest, "waitany", request_waitany, 1);
    rb_define_singleton_method(cRequest, "waitall", request_waitall, 1);
    rb_define_singleton_method(cRequest, "waitsome", request_waitsome, 1);
    rb_define_method(cRequest, "test", request_test, 0);
    rb_define_singleton_method(cRequest, "testany", request_testany, 1);
    rb_define_singleton_method(cRequest, "testall", request_testall, 1);
    rb_define_singleton_method(cRequest, "testsome", request_testsome, 1);
    rb_define_method(cRequest, "cancel", request_cancel, 0);
    rb_define_method(cRequest, "start", request_start, 0);
    rb_define_singleton_method(cRequest, "startall", request_startall, 1);

    rb_define_method(cRequest, "null?", request_null_p, 0);
    rb_define_method(cRequest, "object", request_object, 0);
}
