#include "mpi.h"
#include "ruby/ruby.h"
#include "global.h"

struct mpi_op {
    MPI_Op *op;
    int idx;
};

VALUE cOp;
static ID id_call;

/* Welcome to amature hour... */
#include "mpi_op_fns.c"

static int op_register(VALUE proc)
{
    int i;

    for (i = 0; i < MAX_OPS; i++) {
        if (!op_table[i].assigned) {
            op_table[i].proc = proc;
            op_table[i].assigned = 1;
            op_table[i].resize = 0;
            break;
        }
    }

    if (i == MAX_OPS)
        return -1;

    return i;
}

static void op_free(struct mpi_op *op)
{
    MPI_Op_free(op->op);
    if (op->idx >= 0)
        op_table[op->idx].assigned = 0;
}

static VALUE op_new(struct mpi_op *op)
{
    VALUE tdata;

    tdata = Data_Wrap_Struct(cOp, NULL, op_free, op);
    rb_obj_call_init(tdata, 0, NULL);

    return tdata;
}

MPI_Op *op_get_mpi_op(VALUE rop)
{
    struct mpi_op *op;

    Data_Get_Struct(rop, struct mpi_op, op);

    return op->op;
}

int op_get_resize(VALUE rop)
{
    struct mpi_op *op;

    Data_Get_Struct(rop, struct mpi_op, op);

    return op_table[op->idx].resize;
}

void op_clear_resize(VALUE rop)
{
    struct mpi_op *op;

    Data_Get_Struct(rop, struct mpi_op, op);

    op_table[op->idx].resize = 0;
}

static VALUE op_create(VALUE self, VALUE proc, VALUE rcommute)
{
    int rv, commute;
    struct mpi_op *op;

    op = ALLOC(struct mpi_op);
    op->op = ALLOC(MPI_Op);

    op->idx = op_register(proc);
    if (op->idx < 0) {
        free(op);
        mpi_exception(MPI_ERR_INTERN);
        return Qnil;
    }

    commute = ((rcommute != Qfalse) && (rcommute != Qnil));

    rv = MPI_Op_create(fn_table[op->idx], commute, op->op);
    mpi_exception(rv);

    return op_new(op);
}


/* Predefines */
static ID id_cmp, id_sum, id_prod;
static ID id_land, id_band, id_lor, id_bor, id_bxor;

static void op_max(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        int cmp;
        VALUE str, receiver, arg;

        str = rb_str_new(((char *)invec) + (i * size), size);
        receiver = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg = rb_funcall(mMarshal, id_load, 1, str);

        cmp = rb_funcall(receiver, id_cmp, 1, arg);

        /* We only need to copy invec to outvec if invec > outvec */
        if (cmp > 0) {
            MEMCPY(((char *)inoutvec) + (i * size), 
                   ((char *)invec) + (i * size), 
                   char, length + 1);
        } 
    }
}

static void op_min(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        int cmp;
        VALUE str, receiver, arg;

        str = rb_str_new(((char *)invec) + (i * size), size);
        receiver = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg = rb_funcall(mMarshal, id_load, 1, str);

        cmp = FIX2INT(rb_funcall(receiver, id_cmp, 1, arg));

        /* We only need to copy invec to outvec if invec < outvec */
        if (cmp < 0) {
            MEMCPY(((char *)inoutvec) + (i * size), 
                   ((char *)invec) + (i * size), 
                   char, length + 1);
        } 
    }
}

static void op_sum(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, receiver, arg;

        str = rb_str_new(((char *)invec) + (i * size), size);
        receiver = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(receiver, id_sum, 1, arg);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = StringValueCStr(str);

        if (length + 1 > size) {
            op_table[0].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_prod(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, receiver, arg;

        str = rb_str_new(((char *)invec) + (i * size), size);
        receiver = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(receiver, id_prod, 1, arg);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = StringValueCStr(str);

        if (length + 1 > size) {
            op_table[0].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_lor(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, receiver, arg;

        str = rb_str_new(((char *)invec) + (i * size), size);
        receiver = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(receiver, id_lor, 1, arg);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = StringValueCStr(str);

        if (length + 1 > size) {
            op_table[0].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_bor(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, receiver, arg;

        str = rb_str_new(((char *)invec) + (i * size), size);
        receiver = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(receiver, id_bor, 1, arg);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = StringValueCStr(str);

        if (length + 1 > size) {
            op_table[0].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_land(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, receiver, arg;

        str = rb_str_new(((char *)invec) + (i * size), size);
        receiver = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(receiver, id_land, 1, arg);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = StringValueCStr(str);

        if (length + 1 > size) {
            op_table[0].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_band(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, receiver, arg;

        str = rb_str_new(((char *)invec) + (i * size), size);
        receiver = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(receiver, id_band, 1, arg);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = StringValueCStr(str);

        if (length + 1 > size) {
            op_table[0].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_lxor(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        int val, val1, val2;
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        val1 = ((arg1 != Qfalse) && (arg1 != Qnil));
        val2 = ((arg2 != Qfalse) && (arg2 != Qnil));

        val = (val1 || val2) && (!val1 || !val2);

        str = rb_funcall(mMarshal, id_dump, 1, rb_fix_new(val));
        data = StringValueCStr(str);

        if (length + 1 > size) {
            op_table[0].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_bxor(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, receiver, arg;

        str = rb_str_new(((char *)invec) + (i * size), size);
        receiver = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(receiver, id_bxor, 1, arg);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = StringValueCStr(str);

        if (length + 1 > size) {
            op_table[0].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_maxloc(void *invec, void *inoutvec, int *len, 
                      MPI_Datatype *dtype)
{
    int rv, i, size, length;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        int cmp;
        VALUE str, ary1, ary2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        ary1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        ary2 = rb_funcall(mMarshal, id_load, 1, str);

        cmp = rb_funcall(rb_ary_entry(ary1, 0),id_cmp,1,rb_ary_entry(ary2, 0));

        /* We only need to copy invec to outvec if invec > outvec */
        if (cmp > 0) {
            MEMCPY(((char *)inoutvec) + (i * size), 
                   ((char *)invec) + (i * size), 
                   char, length + 1);
        } 
    }
}

static void op_minloc(void *invec, void *inoutvec, int *len, 
                      MPI_Datatype *dtype)
{
    int rv, i, size, length;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        int cmp;
        VALUE str, ary1, ary2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        ary1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        ary2 = rb_funcall(mMarshal, id_load, 1, str);

        cmp = rb_funcall(rb_ary_entry(ary1, 0),id_cmp,1,rb_ary_entry(ary2, 0));

        /* We only need to copy invec to outvec if invec < outvec */
        if (cmp < 0) {
            MEMCPY(((char *)inoutvec) + (i * size), 
                   ((char *)invec) + (i * size), 
                   char, length + 1);
        } 
    }
}

static void init_predef_ops()
{
    int rv;
    struct mpi_op *max, *min, *sum, *prod; 
    struct mpi_op *land, *band, *lor, *bor, *lxor, *bxor;
    struct mpi_op *maxloc, *minloc;

    id_cmp = rb_intern("<=>");
    id_sum = rb_intern("+");
    id_prod = rb_intern("*");

    max = ALLOC(struct mpi_op);
    max->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_max, 1, max->op);
    mpi_exception(rv);
    rb_define_const(cOp, "MAX", op_new(max));

    min = ALLOC(struct mpi_op);
    min->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_min, 1, min->op);
    mpi_exception(rv);
    rb_define_const(cOp, "MIN", op_new(min));

    sum = ALLOC(struct mpi_op);
    sum->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_sum, 1, sum->op);
    mpi_exception(rv);
    rb_define_const(cOp, "SUM", op_new(sum));

    prod = ALLOC(struct mpi_op);
    prod->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_prod, 1, prod->op);
    mpi_exception(rv);
    rb_define_const(cOp, "PROD", op_new(prod));

    id_land = rb_intern("and");
    id_band = rb_intern("&");
    id_lor = rb_intern("or");
    id_bor = rb_intern("|");
    /* lxor is hard coded */
    id_bxor = rb_intern("^");

    land = ALLOC(struct mpi_op);
    land->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_land, 1, land->op);
    mpi_exception(rv);
    rb_define_const(cOp, "LAND", op_new(land));

    band = ALLOC(struct mpi_op);
    band->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_band, 1, band->op);
    mpi_exception(rv);
    rb_define_const(cOp, "BAND", op_new(band));

    lor = ALLOC(struct mpi_op);
    lor->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_lor, 1, lor->op);
    mpi_exception(rv);
    rb_define_const(cOp, "LOR", op_new(lor));

    bor = ALLOC(struct mpi_op);
    bor->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_bor, 1, bor->op);
    mpi_exception(rv);
    rb_define_const(cOp, "BOR", op_new(bor));

    lxor = ALLOC(struct mpi_op);
    lxor->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_lxor, 1, lxor->op);
    mpi_exception(rv);
    rb_define_const(cOp, "LXOR", op_new(lxor));

    bxor = ALLOC(struct mpi_op);
    bxor->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_bxor, 1, bxor->op);
    mpi_exception(rv);
    rb_define_const(cOp, "BXOR", op_new(bxor));


    maxloc = ALLOC(struct mpi_op);
    maxloc->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_maxloc, 1, maxloc->op);
    mpi_exception(rv);
    rb_define_const(cOp, "MAXLOC", op_new(maxloc));

    minloc = ALLOC(struct mpi_op);
    minloc->op = ALLOC(MPI_Op);
    rv = MPI_Op_create(op_minloc, 1, minloc->op);
    mpi_exception(rv);
    rb_define_const(cOp, "MINLOC", op_new(minloc));
}

void Init_Op()
{
    int i;

    for (i = 0; i < MAX_OPS; i++) {
        op_table[i].assigned = 0;
        op_table[i].resize = 0;
    }

    id_call = rb_intern("call");

    cOp = rb_define_class_under(mMPI, "Op", rb_cObject);

    init_predef_ops();

    rb_define_singleton_method(cOp, "create", op_create, 2);
}
