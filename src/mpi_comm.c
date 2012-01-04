#include "mpi.h"
#include "ruby/ruby.h"
#include "global.h"

VALUE cComm, cIntraComm;
static VALUE SELF, WORLD, ANY_TAG, ANY_SOURCE, GRAPH, CART, COMM_NULL;

struct mpi_comm {
  MPI_Comm *comm;
  char *buffer;
  long bufsize;
};

static void comm_free(struct mpi_comm *comm)
{
  free(comm->buffer);
  if ((*comm->comm != MPI_COMM_WORLD) && (*comm->comm != MPI_COMM_SELF))
    MPI_Comm_free(comm->comm);
  free(comm->comm);
  free(comm);
}

static VALUE comm_new(struct mpi_comm *comm)
{
  VALUE tdata;

  tdata = Data_Wrap_Struct(cComm, NULL, comm_free, comm);
  rb_obj_call_init(tdata, 0, NULL);

  return tdata;
}

static VALUE intra_comm_new(struct mpi_comm *comm)
{
  VALUE tdata;

  tdata = Data_Wrap_Struct(cIntraComm, NULL, comm_free, comm);
  rb_obj_call_init(tdata, 0, NULL);

  return tdata;
}

/* Constructors */
static VALUE comm_dup(VALUE self)
{
  int rv;
  struct mpi_comm *mc_comm, *newcomm;

  Data_Get_Struct(self, struct mpi_comm, mc_comm);

  newcomm = ALLOC(struct mpi_comm);
  newcomm->comm = ALLOC(MPI_Comm);
  newcomm->buffer = NULL;
  newcomm->bufsize = 0;

  rv = MPI_Comm_dup(*mc_comm->comm, newcomm->comm);
  mpi_exception(rv);

  return comm_new(newcomm);
}

static VALUE comm_create(VALUE self, VALUE rgrp)
{
  int rv;
  MPI_Group *grp;
  struct mpi_comm *mc_comm, *newcomm;

  Data_Get_Struct(self, struct mpi_comm, mc_comm);

  newcomm = ALLOC(struct mpi_comm);
  newcomm->comm = ALLOC(MPI_Comm);
  newcomm->buffer = NULL;
  newcomm->bufsize = 0;

  grp = group_get_mpi_group(rgrp);
    
  rv = MPI_Comm_create(*mc_comm->comm, *grp, newcomm->comm);
  mpi_exception(rv);

  return comm_new(newcomm);
}

static VALUE comm_intercomm_create(VALUE self, VALUE rlocal_leader, 
                                   VALUE rpeer, VALUE rremote_leader, 
                                   VALUE rtag)
{
  int rv, local_leader, remote_leader, tag;
  struct mpi_comm *mc_comm, *peer, *newcomm;

  Data_Get_Struct(self, struct mpi_comm, mc_comm);
  Data_Get_Struct(rpeer, struct mpi_comm, peer);

  Check_Type(rlocal_leader, T_FIXNUM);
  Check_Type(rremote_leader, T_FIXNUM);

  local_leader = FIX2INT(rlocal_leader);
  remote_leader = FIX2INT(rremote_leader);

  if (rtag == ANY_TAG) {
    tag = MPI_ANY_TAG;
  } else {
    Check_Type(rtag, T_FIXNUM);
    tag = FIX2INT(rtag);
  }

  newcomm = ALLOC(struct mpi_comm);
  newcomm->comm = ALLOC(MPI_Comm);
  newcomm->buffer = NULL;
  newcomm->bufsize = 0;

  rv = MPI_Intercomm_create(*mc_comm->comm, local_leader, 
			    *peer->comm, remote_leader, tag, 
			    newcomm->comm);
  mpi_exception(rv);

  return comm_new(newcomm);
}

static VALUE comm_intercomm_merge(VALUE self, VALUE rhigh)
{
  int rv, high;
  struct mpi_comm *mc_comm, *newcomm;

  Data_Get_Struct(self, struct mpi_comm, mc_comm);

  Check_Type(rhigh, T_FIXNUM);
  high = FIX2INT(rhigh);

  newcomm = ALLOC(struct mpi_comm);
  newcomm->comm = ALLOC(MPI_Comm);
  newcomm->buffer = NULL;
  newcomm->bufsize = 0;

  rv = MPI_Intercomm_merge(*mc_comm->comm, high, newcomm->comm);
  mpi_exception(rv);

  return comm_new(newcomm);
}

static VALUE comm_split(VALUE self, VALUE rcolor, VALUE rkey)
{
  int rv, color, key;
  struct mpi_comm *mc_comm, *newcomm;

  Data_Get_Struct(self, struct mpi_comm, mc_comm);

  newcomm = ALLOC(struct mpi_comm);
  newcomm->comm = ALLOC(MPI_Comm);
  newcomm->buffer = NULL;
  newcomm->bufsize = 0;

  Check_Type(rcolor, T_FIXNUM);
  Check_Type(rkey, T_FIXNUM);

  color = FIX2INT(rcolor);
  key = FIX2INT(rkey);

  rv = MPI_Comm_split(*mc_comm->comm, color, key, newcomm->comm);
  mpi_exception(rv);

  return comm_new(newcomm);
}

/* Topologies */
static VALUE comm_cart_create(VALUE self, VALUE rdims, VALUE rperiods, 
                              VALUE rreorder)
{
  int rv, i, ndims, *dims, *periods, reorder;
  struct mpi_comm *mc_comm, *newcomm;

  Data_Get_Struct(self, struct mpi_comm, mc_comm);

  newcomm = ALLOC(struct mpi_comm);
  newcomm->comm = ALLOC(MPI_Comm);
  newcomm->buffer = NULL;
  newcomm->bufsize = 0;

  ndims = RARRAY(rdims)->len;
  if (RARRAY(rperiods)->len != ndims) {
    mpi_exception(MPI_ERR_ARG);
    return Qnil;
  }

  dims = ALLOCA_N(int, ndims);
  periods = ALLOCA_N(int, ndims);

  Check_Type(rreorder, T_FIXNUM);

  reorder = FIX2INT(rreorder);

  for (i = 0; i < ndims; i++) {
    dims[i] = FIX2INT(rb_ary_entry(rdims, i));
    /* Qfalse and Qnil are the only values in Ruby that evaluate to 
       non-true. */
    periods[i] = (rb_ary_entry(rperiods, i) != Qfalse) && 
      (rb_ary_entry(rperiods, i) != Qnil);
  }

  rv = MPI_Cart_create(*mc_comm->comm, ndims, dims, periods, reorder, 
		       newcomm->comm);
  mpi_exception(rv);

  return comm_new(newcomm);
}

static VALUE comm_graph_create(VALUE self, VALUE rindex, VALUE redges, 
                               VALUE rreorder)
{
  int rv, i, nnodes, nedges, *index, *edges, reorder;
  struct mpi_comm *mc_comm, *newcomm;

  Data_Get_Struct(self, struct mpi_comm, mc_comm);

  newcomm = ALLOC(struct mpi_comm);
  newcomm->comm = ALLOC(MPI_Comm);
  newcomm->buffer = NULL;
  newcomm->bufsize = 0;

  nnodes = RARRAY(rindex)->len;
  nedges = RARRAY(rindex)->len;

  index = ALLOCA_N(int, nnodes);
  edges = ALLOCA_N(int, nedges);

  Check_Type(rreorder, T_FIXNUM);

  reorder = FIX2INT(rreorder);

  for (i = 0; i < nnodes; i++)
    index[i] = FIX2INT(rb_ary_entry(rindex, i));

  for (i = 0; i < nedges; i++)
    edges[i] = FIX2INT(rb_ary_entry(redges, i));

  rv = MPI_Graph_create(*mc_comm->comm, nnodes, index, edges, reorder, 
			newcomm->comm);
  mpi_exception(rv);

  return comm_new(newcomm);
}

static VALUE comm_topo_test(VALUE self)
{
  int rv, status;
  struct mpi_comm *mc_comm;

  Data_Get_Struct(self, struct mpi_comm, mc_comm);

  rv = MPI_Topo_test(*mc_comm->comm, &status);
  mpi_exception(rv);

  switch (status) {
  case MPI_GRAPH:
            return GRAPH;
        case MPI_CART:
            return CART;
        case MPI_UNDEFINED:
        default:
            return UNDEFINED;
    }
}

static VALUE comm_graphdims_get(VALUE self)
{
    int rv, nnodes, nedges;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Graphdims_get(*mc_comm->comm, &nnodes, &nedges);
    mpi_exception(rv);
    
    return rb_ary_new3(2, rb_fix_new(nnodes), rb_fix_new(nedges));
}

static VALUE comm_graph_get(VALUE self)
{
    int rv, i, nnodes, nedges, *index, *edges;
    struct mpi_comm *mc_comm;
    VALUE rindex, redges;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Graphdims_get(*mc_comm->comm, &nnodes, &nedges);
    mpi_exception(rv);

    index = ALLOCA_N(int, nnodes);
    edges = ALLOCA_N(int, nedges);

    rv = MPI_Graph_get(*mc_comm->comm, nnodes, nedges, index, edges);
    mpi_exception(rv);

    rindex = rb_ary_new2(nnodes);
    redges = rb_ary_new2(nedges);

    for (i = 0; i < nnodes; i++)
        rb_ary_store(rindex, i, rb_fix_new(index[i]));

    for (i = 0; i < nedges; i++)
        rb_ary_store(redges, i, rb_fix_new(edges[i]));
    
    return rb_ary_new3(2, rindex, redges);
}


static VALUE comm_cartdim_get(VALUE self)
{
    int rv, ndims;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Cartdim_get(*mc_comm->comm, &ndims);
    mpi_exception(rv);
    
    return rb_fix_new(ndims);
}

static VALUE comm_cart_get(VALUE self)
{
    int rv, i, ndims, *dims, *periods, *coords;
    struct mpi_comm *mc_comm;
    VALUE rdims, rperiods, rcoords;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Cartdim_get(*mc_comm->comm, &ndims);
    mpi_exception(rv);

    dims = ALLOCA_N(int, ndims);
    periods = ALLOCA_N(int, ndims);
    coords = ALLOCA_N(int, ndims);

    rv = MPI_Cart_get(*mc_comm->comm, ndims, dims, periods, coords);
    mpi_exception(rv);

    rdims = rb_ary_new2(ndims);
    rperiods = rb_ary_new2(ndims);
    rcoords = rb_ary_new2(ndims);

    for (i = 0; i < ndims; i++) {
        rb_ary_store(rdims, i, rb_fix_new(dims[i]));
        rb_ary_store(rperiods, i, periods[i] ? Qtrue : Qfalse);
        rb_ary_store(rcoords, i, rb_fix_new(coords[i]));
    }
    
    return rb_ary_new3(3, rdims, rperiods, rcoords);
}

static VALUE comm_cart_rank(VALUE self, VALUE rcoords)
{
    int rv, i, ndims, rank, *coords;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Cartdim_get(*mc_comm->comm, &ndims);
    mpi_exception(rv);

    if (RARRAY(rcoords)->len != ndims) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }

    coords = ALLOCA_N(int, ndims);
    for (i = 0; i < ndims; i++)
        coords[i] = FIX2INT(rb_ary_entry(rcoords, i));

    rv = MPI_Cart_rank(*mc_comm->comm, coords, &rank);
    mpi_exception(rv);

    return rb_fix_new(rank);
}

static VALUE comm_cart_coords(VALUE self, VALUE rrank)
{
    int rv, i, ndims, rank, *coords;
    struct mpi_comm *mc_comm;
    VALUE rcoords;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rrank, T_FIXNUM);
    rank = FIX2INT(rrank);

    rv = MPI_Cartdim_get(*mc_comm->comm, &ndims);
    mpi_exception(rv);

    coords = ALLOCA_N(int, ndims);

    rv = MPI_Cart_coords(*mc_comm->comm, rank, ndims, coords);
    mpi_exception(rv);

    rcoords = rb_ary_new2(ndims);

    for (i = 0; i < ndims; i++)
        rb_ary_store(rcoords, i, rb_fix_new(coords[i]));
    
    return rcoords;
}

static VALUE comm_graph_neighbors_count(VALUE self, VALUE rrank)
{
    int rv, rank, nneighbors;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rrank, T_FIXNUM);
    rank = FIX2INT(rrank);

    rv = MPI_Graph_neighbors_count(*mc_comm->comm, rank, &nneighbors);
    mpi_exception(rv);

    return rb_fix_new(nneighbors);
}

static VALUE comm_graph_neighbors(VALUE self, VALUE rrank)
{
    int rv, i, rank, nneighbors, *neighbors;
    struct mpi_comm *mc_comm;
    VALUE rneighbors;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rrank, T_FIXNUM);
    rank = FIX2INT(rrank);

    rv = MPI_Graph_neighbors_count(*mc_comm->comm, rank, &nneighbors);
    mpi_exception(rv);

    neighbors = ALLOCA_N(int, nneighbors);

    rv = MPI_Graph_neighbors(*mc_comm->comm, rank, nneighbors, neighbors);
    mpi_exception(rv);

    rneighbors = rb_ary_new2(nneighbors);
    for (i = 0; i < nneighbors; i++)
        rb_ary_store(rneighbors, i, neighbors[i]);

    return rneighbors;
}

static VALUE comm_cart_shift(VALUE self, VALUE rdir, VALUE rdisp)
{
    int rv, dir, disp, src, dest;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rdir, T_FIXNUM);
    Check_Type(rdisp, T_FIXNUM);

    dir = FIX2INT(rdir);
    disp = FIX2INT(rdisp);

    rv = MPI_Cart_shift(*mc_comm->comm, dir, disp, &src, &dest);
    mpi_exception(rv);

    return rb_ary_new3(2, rb_fix_new(src), rb_fix_new(dest));
}

static VALUE comm_cart_sub(VALUE self, VALUE rremain_dims)
{
    int rv, i, ndims, *remain_dims;
    struct mpi_comm *mc_comm, *newcomm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Cartdim_get(*mc_comm->comm, &ndims);
    mpi_exception(rv);

    if (RARRAY(rremain_dims)->len != ndims) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }

    newcomm = ALLOC(struct mpi_comm);
    newcomm->comm = ALLOC(MPI_Comm);
    newcomm->buffer = NULL;
    newcomm->bufsize = 0;

    remain_dims = ALLOCA_N(int, ndims);
    for (i = 0; i < ndims; i++)
        remain_dims[i] = rb_ary_entry(rremain_dims, i);

    rv = MPI_Cart_sub(*mc_comm->comm, remain_dims, newcomm->comm);
    mpi_exception(rv);

    return comm_new(newcomm);
}

static VALUE comm_cart_map(VALUE self, VALUE rdims, VALUE rperiods)
{
    int rv, i, ndims, rank, *dims, *periods;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    ndims = RARRAY(rdims)->len;
    if (RARRAY(rperiods)->len != ndims) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }

    dims = ALLOCA_N(int, ndims);
    periods = ALLOCA_N(int, ndims);

    for (i = 0; i < ndims; i++) {
        dims[i] = FIX2INT(rb_ary_entry(rdims, i));
        /* Qfalse and Qnil are the only values in Ruby that evaluate to 
           non-true. */
        periods[i] = (rb_ary_entry(rperiods, i) != Qfalse) && 
                     (rb_ary_entry(rperiods, i) != Qnil);
    }

    rv = MPI_Cart_map(*mc_comm->comm, ndims, dims, periods, &rank);
    mpi_exception(rv);

    return rb_fix_new(rank);
}

static VALUE comm_graph_map(VALUE self, VALUE rindex, VALUE redges)
{
    int rv, i, rank, nnodes, *index, *edges;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    nnodes = RARRAY(rindex)->len;
    if (RARRAY(redges)->len != nnodes) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }

    index = ALLOCA_N(int, nnodes);
    edges = ALLOCA_N(int, nnodes);

    for (i = 0; i < nnodes; i++) {
        index[i] = FIX2INT(rb_ary_entry(rindex, i));
        edges[i] = FIX2INT(rb_ary_entry(redges, i));
    }

    rv = MPI_Graph_map(*mc_comm->comm, nnodes, index, edges, &rank);
    mpi_exception(rv);

    return rb_fix_new(rank);
}

/* Environmental */

static VALUE comm_rank(VALUE self)
{
    int rv, rank;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Comm_rank(*mc_comm->comm, &rank);
    mpi_exception(rv);

    return rb_fix_new(rank);
}

static VALUE comm_size(VALUE self)
{
    int rv, size;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);
   
    rv = MPI_Comm_size(*mc_comm->comm, &size);
    mpi_exception(rv);

    return rb_fix_new(size);
}

static VALUE comm_group(VALUE self)
{
    int rv;
    MPI_Group *grp;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    grp = ALLOC(MPI_Group);
    
    rv = MPI_Comm_group(*mc_comm->comm, grp);
    mpi_exception(rv);

    return group_new(grp);
}

static VALUE comm_abort(VALUE self, VALUE rerrorcode)
{
    int rv, errorcode;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);
   
    errorcode = FIX2INT(rerrorcode);

    rv = MPI_Abort(*mc_comm->comm, errorcode);
    mpi_exception(rv);

    return Qtrue;
}

/* Point to point */
static VALUE comm_send(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int rv;
    int length;
    int dest, tag;
    char *data;
    VALUE dump;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rdest, T_FIXNUM);
    dest = FIX2INT(rdest);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    dump = rb_funcall(mMarshal, id_dump, 1, obj);
    data = StringValueCStr(dump);

    rv = MPI_Send(&length, 1, MPI_INT, dest, tag, *mc_comm->comm);
    mpi_exception(rv);
    rv = MPI_Send(data, length + 1, MPI_BYTE, dest, tag, *mc_comm->comm);
    mpi_exception(rv);

    return Qtrue;
}

static VALUE comm_recv(VALUE self, VALUE rsrc, VALUE rtag)
{
    int rv, length;
    int src, tag;
    char *data;
    VALUE obj;
    MPI_Status *stat;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    if (rsrc == ANY_SOURCE) {
        src = MPI_ANY_SOURCE;
    } else {
        Check_Type(rsrc, T_FIXNUM);
        src = FIX2INT(rsrc);
    }

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    stat = ALLOC(MPI_Status);
    rv = MPI_Recv(&length, 1, MPI_INT, src, tag, *mc_comm->comm, stat);
    mpi_exception(rv);
    if (src == MPI_ANY_SOURCE) src = stat->MPI_SOURCE;

    data = ALLOC_N(char, length + 1);
    rv = MPI_Recv(data, length+1, MPI_BYTE, src, tag, *mc_comm->comm, stat);
    mpi_exception(rv);
    obj = rb_str_new(data, length + 1);

    return rb_ary_new3(2, rb_funcall(mMarshal, id_load, 1, obj),
                          status_new(stat));
}

static VALUE comm_bsend(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int rv;
    int length;
    int dest, tag;
    char *data;
    VALUE dump;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rdest, T_FIXNUM);
    dest = FIX2INT(rdest);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    dump = rb_funcall(mMarshal, id_dump, 1, obj);
    data = StringValueCStr(dump);

    rv = MPI_Bsend(&length, 1, MPI_INT, dest, tag, *mc_comm->comm);
    mpi_exception(rv);
    rv = MPI_Bsend(data, length+1, MPI_BYTE, dest, tag, *mc_comm->comm);
    mpi_exception(rv);

    return Qtrue;
}

static VALUE comm_ssend(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int rv;
    int length;
    int dest, tag;
    char *data;
    VALUE dump;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rdest, T_FIXNUM);
    dest = FIX2INT(rdest);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    dump = rb_funcall(mMarshal, id_dump, 1, obj);
    data = StringValueCStr(dump);

    rv = MPI_Ssend(&length, 1, MPI_INT, dest, tag, *mc_comm->comm);
    mpi_exception(rv);
    rv = MPI_Ssend(data, length+1, MPI_BYTE, dest, tag, *mc_comm->comm);
    mpi_exception(rv);

    return Qtrue;
}

static VALUE comm_rsend(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int rv;
    int length;
    int dest, tag;
    char *data;
    VALUE dump;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rdest, T_FIXNUM);
    dest = FIX2INT(rdest);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    dump = rb_funcall(mMarshal, id_dump, 1, obj);
    data = StringValueCStr(dump);

    /* Note: we only care about the first one being a ready send. */
    /* If it works, that means the user did the correct thing. */
    rv = MPI_Rsend(&length, 1, MPI_INT, dest, tag, *mc_comm->comm);
    mpi_exception(rv);
    rv = MPI_Send(data, length + 1, MPI_BYTE, dest, tag, *mc_comm->comm);
    mpi_exception(rv);

    return Qtrue;
}

static VALUE comm_sendrecv(VALUE self, VALUE obj, VALUE rdest, VALUE rdtag, 
                           VALUE rsrc, VALUE rstag)
{
    int rv;
    int dest, src;
    int dtag, stag;
    int dlen, slen;
    char *ddata, *sdata;
    VALUE dump, str;
    MPI_Status *stat;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rdest, T_FIXNUM);
    dest = FIX2INT(rdest);
    
    if (rdtag == ANY_TAG) {
        dtag = MPI_ANY_TAG;
    } else {
        Check_Type(rdtag, T_FIXNUM);
        dtag = FIX2INT(rdtag);
    }

    if (rsrc == ANY_SOURCE) {
        src = MPI_ANY_SOURCE;
    } else {
        Check_Type(rsrc, T_FIXNUM);
        src = FIX2INT(rsrc);
    }
    
    if (rstag == ANY_TAG) {
        stag = MPI_ANY_TAG;
    } else {
        Check_Type(rstag, T_FIXNUM);
        stag = FIX2INT(rstag);
    }

    dump = rb_funcall(mMarshal, id_dump, 1, obj);
    ddata = StringValueCStr(dump);

    stat = ALLOC(MPI_Status);
    
    rv = MPI_Sendrecv(&dlen, 1, MPI_INT, dest, dtag, 
                      &slen, 1, MPI_INT, src, stag, 
                      *mc_comm->comm, stat);
    mpi_exception(rv);

    sdata = ALLOC_N(char, slen + 1);

    rv = MPI_Sendrecv(ddata, dlen + 1, MPI_BYTE, dest, dtag, 
                      sdata, slen + 1, MPI_BYTE, src, stag,
                      *mc_comm->comm, stat);
    mpi_exception(rv);

    str = rb_str_new(sdata, slen + 1);

    return rb_ary_new3(2, rb_funcall(mMarshal, id_load, 1, str), 
                          status_new(stat));
}

/* Non-blocking */

static VALUE comm_isend(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int rv;
    int length;
    int dest, tag;
    char *data;
    VALUE dump;
    MPI_Request tmp, *req;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rdest, T_FIXNUM);
    dest = FIX2INT(rdest);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    dump = rb_funcall(mMarshal, id_dump, 1, obj);
    data = StringValueCStr(dump);

    req = ALLOC(MPI_Request);

    rv = MPI_Isend(&length, 1, MPI_INT, dest, tag, *mc_comm->comm, &tmp);
    mpi_exception(rv);
    rv = MPI_Isend(data, length+1, MPI_BYTE, dest, tag, *mc_comm->comm, req);
    mpi_exception(rv);

    return request_new(NORMAL, req);
}

static VALUE comm_issend(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int rv;
    int length;
    int dest, tag;
    char *data;
    VALUE dump;
    MPI_Request tmp, *req;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    dest = FIX2INT(rdest);
    Check_Type(rdest, T_FIXNUM);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    dump = rb_funcall(mMarshal, id_dump, 1, obj);
    data = StringValueCStr(dump);

    req = ALLOC(MPI_Request);

    rv = MPI_Issend(&length, 1, MPI_INT, dest, tag, *mc_comm->comm, &tmp);
    mpi_exception(rv);
    rv = MPI_Issend(data, length+1, MPI_BYTE, dest, tag, *mc_comm->comm, req);
    mpi_exception(rv);

    return request_new(NORMAL, req);
}

static VALUE comm_ibsend(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int rv;
    int length;
    int dest, tag;
    char *data;
    VALUE dump;
    MPI_Request tmp, *req;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    dest = FIX2INT(rdest);
    Check_Type(rdest, T_FIXNUM);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }


    dump = rb_funcall(mMarshal, id_dump, 1, obj);
    data = StringValueCStr(dump);

    req = ALLOC(MPI_Request);

    rv = MPI_Ibsend(&length, 1, MPI_INT, dest, tag, *mc_comm->comm, &tmp);
    mpi_exception(rv);
    rv = MPI_Ibsend(data, length+1, MPI_BYTE, dest, tag, *mc_comm->comm, req);
    mpi_exception(rv);

    return request_new(NORMAL, req);
}

static VALUE comm_irsend(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int rv;
    int length;
    int dest, tag;
    char *data;
    VALUE dump;
    MPI_Request tmp, *req;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rdest, T_FIXNUM);
    dest = FIX2INT(rdest);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    dump = rb_funcall(mMarshal, id_dump, 1, obj);
    data = StringValueCStr(dump);

    req = ALLOC(MPI_Request);

    rv = MPI_Irsend(&length, 1, MPI_INT, dest, tag, *mc_comm->comm, &tmp);
    mpi_exception(rv);
    rv = MPI_Irsend(data, length+1, MPI_BYTE, dest, tag, *mc_comm->comm, req);
    mpi_exception(rv);

    return request_new(NORMAL, req);
}

static VALUE comm_irecv(VALUE self, VALUE rsrc, VALUE rtag)
{    
    int src, tag;
    struct mpi_comm *mc_comm;
    VALUE req;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    if (rsrc == ANY_SOURCE) {
        src = MPI_ANY_SOURCE;
    } else {
        Check_Type(rsrc, T_FIXNUM);
        src = FIX2INT(rsrc);
    }

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    req = request_new(RECV, src, tag, mc_comm->comm);
    request_start(req);

    return req;
}

/* Persistent communication */
static VALUE comm_send_init(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int dest, tag;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    dest = FIX2INT(rdest);
    Check_Type(rdest, T_FIXNUM);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    return request_new(PSEND, obj, dest, tag, mc_comm->comm);
}

static VALUE comm_bsend_init(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int dest, tag;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    dest = FIX2INT(rdest);
    Check_Type(rdest, T_FIXNUM);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    return request_new(PBSEND, obj, dest, tag, mc_comm->comm);
}

static VALUE comm_ssend_init(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int dest, tag;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rdest, T_FIXNUM);
    dest = FIX2INT(rdest);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    return request_new(PSSEND, obj, dest, tag, mc_comm->comm);
}

static VALUE comm_rsend_init(VALUE self, VALUE obj, VALUE rdest, VALUE rtag)
{
    int dest, tag;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    dest = FIX2INT(rdest);
    Check_Type(rdest, T_FIXNUM);

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    return request_new(PRSEND, obj, dest, tag, mc_comm->comm);
}

static VALUE comm_recv_init(VALUE self, VALUE rsrc, VALUE rtag)
{    
    int src, tag;
    struct mpi_comm *mc_comm;
    VALUE req;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    if (rsrc == ANY_SOURCE) {
        src = MPI_ANY_SOURCE;
    } else {
        Check_Type(rsrc, T_FIXNUM);
        src = FIX2INT(rsrc);
    }

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    req = request_new(RECV, src, tag, mc_comm->comm);

    return req;
}

/* Buffering */
static VALUE comm_buffer_for(VALUE self, VALUE ary)
{
    int rv;
    int length;
    VALUE dump;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    dump = rb_funcall(mMarshal, id_dump, 1, ary);
    length = RSTRING(dump)->len;
    /* Add this so we can send lengths before each object */
    length += (sizeof(int) * RARRAY(ary)->len);

    if (mc_comm->bufsize < length) {
        int size;
        char *buffer;

        rv = MPI_Buffer_detach(&buffer, &size);
        mpi_exception(rv);

        REALLOC_N(mc_comm->buffer, char, length);
        mc_comm->bufsize = length;

        rv = MPI_Buffer_attach(mc_comm->buffer, length);
        mpi_exception(rv);
    }

    return Qtrue;
}

static VALUE comm_unbuffer(VALUE self)
{
    int rv;
    int size;
    char *buffer;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Buffer_detach(&buffer, &size);
    mpi_exception(rv);

    mc_comm->buffer = NULL;
    mc_comm->bufsize = 0;
    free(buffer);

    return Qtrue;
}


/* Collective ops */

static VALUE comm_barrier(VALUE self)
{
    int rv;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Barrier(*mc_comm->comm);
    mpi_exception(rv);
    
    return Qtrue;
}

static VALUE comm_bcast(VALUE self, VALUE obj, VALUE rroot)
{
    int rv, rank, root, length;
    char *data;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    root = FIX2INT(rroot);

    rv = MPI_Comm_rank(*mc_comm->comm, &rank);
    mpi_exception(rv);
    
    if (rank == root) {
        VALUE dump;

        dump = rb_funcall(mMarshal, id_dump, 1, obj);
        data = StringValueCStr(dump);
    }

    rv = MPI_Bcast(&length, 1, MPI_INT, root, *mc_comm->comm);
    mpi_exception(rv);

    if (rank != root)
        data = ALLOCA_N(char, length + 1);

    rv = MPI_Bcast(data, length + 1, MPI_BYTE, root, *mc_comm->comm);
    mpi_exception(rv);

    if (rank == root) {
        return obj;
    } else {
        VALUE load;

        obj = rb_str_new(data, length);
        load = rb_funcall(mMarshal, id_load, 1, obj);

        return load;
    }
}

#define GATHER_TAG     42
static VALUE comm_gather(VALUE self, VALUE obj, VALUE rroot)
{
    /* This is being reimplemented based on the MPICH algorithm to allow 
       gathering of objects of arbitrary size */
    int rv, rank, root, csize, length;
    char *data;
    struct mpi_comm *mc_comm;
    VALUE ary;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    root = FIX2INT(rroot);

    rv = MPI_Comm_rank(*mc_comm->comm, &rank);
    mpi_exception(rv);
    rv = MPI_Comm_size(*mc_comm->comm, &csize);
    mpi_exception(rv);
    
    if (rank == root) {
        int i;

        ary = rb_ary_new2(csize);
        rb_ary_store(ary, root, obj);

        for (i = 0; i < csize; i++) {
            if (i != rank) {
                VALUE load;
                MPI_Status stat;

                rv = MPI_Recv(&length, 1, MPI_INT, i, GATHER_TAG, 
                              *mc_comm->comm, &stat);
                mpi_exception(rv);

                data = ALLOC_N(char, length + 1);
                rv = MPI_Recv(data, length+1, MPI_INT, i, GATHER_TAG, 
                              *mc_comm->comm, &stat);
                mpi_exception(rv);

                obj = rb_str_new(data, length + 1);
                load = rb_funcall(mMarshal, id_load, 1, obj);

                rb_ary_store(ary, i, load);
            }
        }

        return ary;
    } else {
        VALUE dump;
        MPI_Request reqs[2];
        MPI_Status stats[2];

        dump = rb_funcall(mMarshal, id_dump, 1, obj);
        data = StringValueCStr(dump);

        rv = MPI_Isend(&length, 1, MPI_INT, root, GATHER_TAG, 
                       *mc_comm->comm, &reqs[0]);
        mpi_exception(rv);
        rv = MPI_Isend(data, length+1, MPI_BYTE, root, GATHER_TAG, 
                       *mc_comm->comm, &reqs[1]);
        mpi_exception(rv);

        rv = MPI_Waitall(2, reqs, stats);
        mpi_exception(rv);

        return Qtrue;
    }
}

static VALUE map_in_place(VALUE ary)
{
    return rb_funcall(ary, id_map_bang, 0);
}

static VALUE marshal_load(VALUE mdata, VALUE extra)
{
    return rb_funcall(mMarshal, id_load, 1, mdata);
}

#define ALLGATHER_TAG     43
static VALUE comm_allgather(VALUE self, VALUE obj)
{
    int rv;
    int i, j, jnext;
    int rank, csize, left, right;
    int length_right, length_left;
    char *in, *out;
    struct mpi_comm *mc_comm;
    VALUE dump, ary;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Comm_rank(*mc_comm->comm, &rank);
    mpi_exception(rv);
    rv = MPI_Comm_size(*mc_comm->comm, &csize);
    mpi_exception(rv);
 
    /* Everybody marshals their own data first and that's how it's */
    /* transmitted.  Unmarshalling doesn't happen until everyone  */
    /* has all the data. */
    dump = rb_funcall(mMarshal, id_dump, 1, obj);

    ary = rb_ary_new2(csize);
    rb_ary_store(ary, rank, dump);

    left = (csize + rank - 1) % csize;
    right = (rank + 1) % csize;

    j = rank;
    jnext = left;
    for (i = 1; i < csize; i++) {
        MPI_Status stat;
	
	VALUE pos = rb_ary_entry(ary, j);

        out = StringValueCStr(pos);

        rv = MPI_Sendrecv(&length_right, 1, MPI_INT, right, ALLGATHER_TAG,
                &length_left, 1, MPI_INT, left, ALLGATHER_TAG, *mc_comm->comm,
                &stat);
        mpi_exception(rv);

        in = ALLOC_N(char, length_left + 1);
        
        rv = MPI_Sendrecv(out, length_right + 1, MPI_BYTE, right, ALLGATHER_TAG,
                in, length_left + 1, MPI_BYTE, left, ALLGATHER_TAG, 
                *mc_comm->comm, &stat);
        mpi_exception(rv);

        rb_ary_store(ary, jnext, rb_str_new(in, length_left + 1));

        j = jnext;
        jnext = (csize + jnext - 1) % csize;
    }

    return rb_iterate(map_in_place, ary, marshal_load, Qnil);
}

static VALUE comm_scatter(VALUE self, VALUE ary, VALUE rroot)
{
    int rv, rank, csize, root, longest = 0;
    char *send, *recv;
    struct marshalled {
        char *str;
        int length;
    } *datav;
    struct mpi_comm *mc_comm;
    MPI_Datatype dtype;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    root = FIX2INT(rroot);

    rv = MPI_Comm_rank(*mc_comm->comm, &rank);
    mpi_exception(rv);
    
    rv = MPI_Comm_size(*mc_comm->comm, &csize);
    mpi_exception(rv);

    if (RARRAY(ary)->len != csize) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }
    
    if (rank == root) {
        int i;
        VALUE dump;

        datav = ALLOCA_N(struct marshalled, csize);
        
        for (i = 0; i < csize; i++) {
            dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(ary, i));
            datav[i].str = StringValueCStr(dump);

            if (datav[i].length + 1 > longest)
                longest = datav[i].length + 1;
        }

        send = ALLOCA_N(char, csize * longest);

        for (i = 0; i < csize; i++)
            MEMCPY(send + (i * longest), datav[i].str, char, datav[i].length);
    }

    rv = MPI_Bcast(&longest, 1, MPI_INT, root, *mc_comm->comm);
    mpi_exception(rv);

    rv = MPI_Type_contiguous(longest, MPI_BYTE, &dtype);
    mpi_exception(rv);

    rv = MPI_Type_commit(&dtype);
    mpi_exception(rv);
    
    recv = ALLOCA_N(char, longest);

    rv = MPI_Scatter(send, csize * longest, dtype, recv, longest, dtype, 
                     root, *mc_comm->comm);
    mpi_exception(rv);

    rv = MPI_Type_free(&dtype);
    mpi_exception(rv);
    
    return rb_funcall(mMarshal, id_load, 1, rb_str_new(recv, longest));
}

static VALUE comm_alltoall(VALUE self, VALUE sary)
{
    int rv, i, csize, longest = 0, glongest;
    char *send, *recv;
    struct marshalled {
        char *str;
        int length;
    } *datav;
    struct mpi_comm *mc_comm;
    MPI_Datatype dtype;
    VALUE rary; 
    
    rv = MPI_Comm_size(*mc_comm->comm, &csize);
    mpi_exception(rv);

    if (RARRAY(sary)->len != csize) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }

    datav = ALLOCA_N(struct marshalled, csize);

    for (i = 0; i < csize; i++) {
        VALUE dump;

        dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(sary, i));
        datav[i].str = StringValueCStr(dump);

        if (datav[i].length + 1 > longest)
            longest = datav[i].length + 1;
    }

    send = ALLOCA_N(char, csize * longest);

    for (i = 0; i < csize; i++)
        MEMCPY(send + (i * longest), datav[i].str, char, datav[i].length);

    rv = MPI_Allreduce(&longest, &glongest,1, MPI_INT, MPI_MAX, *mc_comm->comm);
    mpi_exception(rv);

    rv = MPI_Type_contiguous(glongest, MPI_BYTE, &dtype);
    mpi_exception(rv);

    rv = MPI_Type_commit(&dtype);
    mpi_exception(rv);
    
    recv = ALLOCA_N(char, csize * glongest);

    rv = MPI_Alltoall(send, csize, dtype, recv, csize, dtype, *mc_comm->comm);
    mpi_exception(rv);

    rary = rb_ary_new2(csize);

    for (i = 0; i < csize; i++) {
        VALUE str;

        str = rb_str_new(recv + (i * glongest), glongest);
        rb_ary_store(rary, i, rb_funcall(mMarshal, id_load, 1, str));
    }

    rv = MPI_Type_free(&dtype);
    mpi_exception(rv);
    
    return rary;
}

static VALUE comm_reduce(VALUE self, VALUE obj, VALUE rop, VALUE rroot)
{
    int rv, i;
    int rank, root;
    int arylen = -1, arymax, arymin;
    char *send, *recv = NULL, *tmp;
    int length, longest, resize;
    struct mpi_comm *mc_comm;
    MPI_Datatype dtype;
    VALUE dump;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    Check_Type(rroot, T_FIXNUM);
    root = FIX2INT(rroot);
    
    rv = MPI_Comm_rank(*mc_comm->comm, &rank);
    mpi_exception(rv);
    
    /* If an array was passed in, check that it's valid */
    if (TYPE(obj) == T_ARRAY)
        arylen = RARRAY(obj)->len;

    rv = MPI_Allreduce(&arylen, &arymax, 1, MPI_INT, MPI_MAX, *mc_comm->comm);
    mpi_exception(rv);

    rv = MPI_Allreduce(&arylen, &arymin, 1, MPI_INT, MPI_MIN, *mc_comm->comm);
    mpi_exception(rv);

    if (arymin != arymax) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }

    if (arylen >= 0) {
        for (i = 0; i < arylen; i++) {
            dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
            StringValueCStr(dump);

            if (length > longest)
                longest = length;
        }

        length = longest;
    } else {
        dump = rb_funcall(mMarshal, id_dump, 1, obj);
        tmp = StringValueCStr(dump);
    }

    /* Global length check */
    rv = MPI_Allreduce(&length, &longest, 1, MPI_INT, MPI_MAX, *mc_comm->comm);
    mpi_exception(rv);

    rv = MPI_Type_contiguous(longest + 1, MPI_BYTE, &dtype);
    mpi_exception(rv);

    rv = MPI_Type_commit(&dtype);
    mpi_exception(rv);

    /* Allocation and copying */
    if (arylen >= 0) {
        if (rank == root)
            recv = ALLOCA_N(char, (longest + 1) * arylen);

        send = ALLOCA_N(char, (longest + 1) * arylen);
        
        for (i = 0; i < arylen; i++) {
            dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
            tmp = StringValueCStr(dump);

            MEMCPY(send + (i * (longest + 1)), tmp, char, length + 1);
        }

        rv = MPI_Reduce(send, recv, arylen, dtype, *op_get_mpi_op(rop), 
                root, *mc_comm->comm);
        mpi_exception(rv);

        resize = op_get_resize(rop);
        while (resize > 0) {
            op_clear_resize(rop);

            rv = MPI_Type_free(&dtype);
            mpi_exception(rv);

            rv = MPI_Type_contiguous(resize, MPI_BYTE, &dtype);
            mpi_exception(rv);

            rv = MPI_Type_commit(&dtype);
            mpi_exception(rv);

            /* The send buffer is likely to have gotten munged. Reinit. */
            for (i = 0; i < arylen; i++) {
                dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
                tmp = StringValueCStr(dump);

                MEMCPY(send + (i * (longest + 1)), tmp, char, length + 1);
            }

            rv = MPI_Reduce(send, recv, arylen, dtype, *op_get_mpi_op(rop), 
                            root, *mc_comm->comm);
            mpi_exception(rv);

            resize = op_get_resize(rop);
        }

        rv = MPI_Type_free(&dtype);
        mpi_exception(rv);

        if (rank == root) {
            VALUE ary;

            ary = rb_ary_new2(arylen);
            for (i = 0; i < arylen; i++) {
                VALUE str, obj;

                str = rb_str_new(recv + (i * (longest + 1)), longest + 1);
                obj = rb_funcall(mMarshal, id_load, 1, str);
                rb_ary_store(ary, i, obj);
            }

            return ary;
        }
    } else {
        if (rank == root)
            recv = ALLOCA_N(char, longest + 1);

        send = ALLOCA_N(char, longest + 1);
        MEMCPY(send, tmp, char, length + 1);

        rv = MPI_Reduce(send, recv, 1, dtype, *op_get_mpi_op(rop), 
                root, *mc_comm->comm);
        mpi_exception(rv);

        resize = op_get_resize(rop);
        while (resize > 0) {
            op_clear_resize(rop);

            rv = MPI_Type_free(&dtype);
            mpi_exception(rv);

            rv = MPI_Type_contiguous(resize, MPI_BYTE, &dtype);
            mpi_exception(rv);

            rv = MPI_Type_commit(&dtype);
            mpi_exception(rv);

            /* The send buffer is likely to have gotten munged. Reinit. */
            dump = rb_funcall(mMarshal, id_dump, 1, obj);
            tmp = StringValueCStr(dump);
            MEMCPY(send, tmp, char, length + 1);

            rv = MPI_Reduce(send, recv, 1, dtype, *op_get_mpi_op(rop), 
                            root, *mc_comm->comm);
            mpi_exception(rv);

            resize = op_get_resize(rop);
        }

        rv = MPI_Type_free(&dtype);
        mpi_exception(rv);

        if (rank == root)
            return rb_funcall(mMarshal, id_load, 1, rb_str_new(recv,longest+1));
    }

    return Qtrue;
}

static VALUE comm_allreduce(VALUE self, VALUE obj, VALUE rop)
{
    int rv, i;
    int arylen = -1, arymax, arymin;
    char *send, *recv = NULL, *tmp;
    int length, longest, resize;
    struct mpi_comm *mc_comm;
    MPI_Datatype dtype;
    VALUE dump;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    /* If an array was passed in, check that it's valid */
    if (TYPE(obj) == T_ARRAY)
        arylen = RARRAY(obj)->len;

    rv = MPI_Allreduce(&arylen, &arymax, 1, MPI_INT, MPI_MAX, *mc_comm->comm);
    mpi_exception(rv);

    rv = MPI_Allreduce(&arylen, &arymin, 1, MPI_INT, MPI_MIN, *mc_comm->comm);
    mpi_exception(rv);

    if (arymin != arymax) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }

    if (arylen >= 0) {
        for (i = 0; i < arylen; i++) {
            dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
            StringValueCStr(dump);

            if (length > longest)
                longest = length;
        }

        length = longest;
    } else {
        dump = rb_funcall(mMarshal, id_dump, 1, obj);
        tmp = StringValueCStr(dump);
    }

    /* Global length check */
    rv = MPI_Allreduce(&length, &longest, 1, MPI_INT, MPI_MAX, *mc_comm->comm);
    mpi_exception(rv);

    rv = MPI_Type_contiguous(longest + 1, MPI_BYTE, &dtype);
    mpi_exception(rv);

    rv = MPI_Type_commit(&dtype);
    mpi_exception(rv);

    /* Allocation and copying */
    if (arylen >= 0) {
        VALUE ary;

        recv = ALLOCA_N(char, (longest + 1) * arylen);
        send = ALLOCA_N(char, (longest + 1) * arylen);
        
        for (i = 0; i < arylen; i++) {
            dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
            tmp = StringValueCStr(dump);

            MEMCPY(send + (i * (longest + 1)), tmp, char, length + 1);
        }

        rv = MPI_Allreduce(send, recv, arylen, dtype, *op_get_mpi_op(rop), 
                           *mc_comm->comm);
        mpi_exception(rv);

        resize = op_get_resize(rop);
        while (resize > 0) {
            op_clear_resize(rop);

            rv = MPI_Type_free(&dtype);
            mpi_exception(rv);

            rv = MPI_Type_contiguous(resize, MPI_BYTE, &dtype);
            mpi_exception(rv);

            rv = MPI_Type_commit(&dtype);
            mpi_exception(rv);

            /* The send buffer is likely to have gotten munged. Reinit. */
            for (i = 0; i < arylen; i++) {
                dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
                tmp = StringValueCStr(dump);

                MEMCPY(send + (i * (longest + 1)), tmp, char, length + 1);
            }

            rv = MPI_Allreduce(send, recv, arylen, dtype, *op_get_mpi_op(rop), 
                               *mc_comm->comm);
            mpi_exception(rv);

            resize = op_get_resize(rop);
        }

        rv = MPI_Type_free(&dtype);
        mpi_exception(rv);

        ary = rb_ary_new2(arylen);
        for (i = 0; i < arylen; i++) {
            VALUE str, obj;

            str = rb_str_new(recv + (i * (longest + 1)), longest + 1);
            obj = rb_funcall(mMarshal, id_load, 1, str);
            rb_ary_store(ary, i, obj);
        }

        return ary;
    } else {
        recv = ALLOCA_N(char, longest + 1);
        send = ALLOCA_N(char, longest + 1);
        MEMCPY(send, tmp, char, length + 1);

        rv = MPI_Allreduce(send, recv, 1, dtype, *op_get_mpi_op(rop), 
                           *mc_comm->comm);
        mpi_exception(rv);

        resize = op_get_resize(rop);
        while (resize > 0) {
            op_clear_resize(rop);

            rv = MPI_Type_free(&dtype);
            mpi_exception(rv);

            rv = MPI_Type_contiguous(resize, MPI_BYTE, &dtype);
            mpi_exception(rv);

            rv = MPI_Type_commit(&dtype);
            mpi_exception(rv);

            /* The send buffer is likely to have gotten munged. Reinit. */
            dump = rb_funcall(mMarshal, id_dump, 1, obj);
            tmp = StringValueCStr(dump);
            MEMCPY(send, tmp, char, length + 1);

            rv = MPI_Allreduce(send, recv, 1, dtype, *op_get_mpi_op(rop), 
                               *mc_comm->comm);
            mpi_exception(rv);

            resize = op_get_resize(rop);
        }

        rv = MPI_Type_free(&dtype);
        mpi_exception(rv);

        return rb_funcall(mMarshal, id_load, 1, rb_str_new(recv, longest + 1));
    }
}

static VALUE comm_scan(VALUE self, VALUE obj, VALUE rop)
{
    int rv, i;
    int arylen = -1, arymax, arymin;
    char *send, *recv = NULL, *tmp;
    int length, longest, resize;
    struct mpi_comm *mc_comm;
    MPI_Datatype dtype;
    VALUE dump;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    /* If an array was passed in, check that it's valid */
    if (TYPE(obj) == T_ARRAY)
        arylen = RARRAY(obj)->len;

    rv = MPI_Allreduce(&arylen, &arymax, 1, MPI_INT, MPI_MAX, *mc_comm->comm);
    mpi_exception(rv);

    rv = MPI_Allreduce(&arylen, &arymin, 1, MPI_INT, MPI_MIN, *mc_comm->comm);
    mpi_exception(rv);

    if (arymin != arymax) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }

    if (arylen >= 0) {
        for (i = 0; i < arylen; i++) {
            dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
            StringValueCStr(dump);

            if (length > longest)
                longest = length;
        }

        length = longest;
    } else {
        dump = rb_funcall(mMarshal, id_dump, 1, obj);
        tmp = StringValueCStr(dump);
    }

    /* Global length check */
    rv = MPI_Allreduce(&length, &longest, 1, MPI_INT, MPI_MAX, *mc_comm->comm);
    mpi_exception(rv);

    rv = MPI_Type_contiguous(longest + 1, MPI_BYTE, &dtype);
    mpi_exception(rv);

    rv = MPI_Type_commit(&dtype);
    mpi_exception(rv);

    /* Allocation and copying */
    if (arylen >= 0) {
        VALUE ary;

        recv = ALLOCA_N(char, (longest + 1) * arylen);
        send = ALLOCA_N(char, (longest + 1) * arylen);
        
        for (i = 0; i < arylen; i++) {
            dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
            tmp = StringValueCStr(dump);

            MEMCPY(send + (i * (longest + 1)), tmp, char, length + 1);
        }

        rv = MPI_Scan(send, recv, arylen, dtype, *op_get_mpi_op(rop), 
                      *mc_comm->comm);
        mpi_exception(rv);

        resize = op_get_resize(rop);
        while (resize > 0) {
            op_clear_resize(rop);

            rv = MPI_Type_free(&dtype);
            mpi_exception(rv);

            rv = MPI_Type_contiguous(resize, MPI_BYTE, &dtype);
            mpi_exception(rv);

            rv = MPI_Type_commit(&dtype);
            mpi_exception(rv);

            /* The send buffer is likely to have gotten munged. Reinit. */
            for (i = 0; i < arylen; i++) {
                dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
                tmp = StringValueCStr(dump);

                MEMCPY(send + (i * (longest + 1)), tmp, char, length + 1);
            }

            rv = MPI_Scan(send, recv, arylen, dtype, *op_get_mpi_op(rop), 
                          *mc_comm->comm);
            mpi_exception(rv);

            resize = op_get_resize(rop);
        }

        rv = MPI_Type_free(&dtype);
        mpi_exception(rv);

        ary = rb_ary_new2(arylen);
        for (i = 0; i < arylen; i++) {
            VALUE str, obj;

            str = rb_str_new(recv + (i * (longest + 1)), longest + 1);
            obj = rb_funcall(mMarshal, id_load, 1, str);
            rb_ary_store(ary, i, obj);
        }

        return ary;
    } else {
        recv = ALLOCA_N(char, longest + 1);
        send = ALLOCA_N(char, longest + 1);
        MEMCPY(send, tmp, char, length + 1);

        rv = MPI_Scan(send, recv, 1, dtype, *op_get_mpi_op(rop), 
                           *mc_comm->comm);
        mpi_exception(rv);

        resize = op_get_resize(rop);
        while (resize > 0) {
            op_clear_resize(rop);

            rv = MPI_Type_free(&dtype);
            mpi_exception(rv);

            rv = MPI_Type_contiguous(resize, MPI_BYTE, &dtype);
            mpi_exception(rv);

            rv = MPI_Type_commit(&dtype);
            mpi_exception(rv);

            /* The send buffer is likely to have gotten munged. Reinit. */
            dump = rb_funcall(mMarshal, id_dump, 1, obj);
            tmp = StringValueCStr(dump);
            MEMCPY(send, tmp, char, length + 1);

            rv = MPI_Scan(send, recv, 1, dtype, *op_get_mpi_op(rop), 
                          *mc_comm->comm);
            mpi_exception(rv);

            resize = op_get_resize(rop);
        }

        rv = MPI_Type_free(&dtype);
        mpi_exception(rv);

        return rb_funcall(mMarshal, id_load, 1, rb_str_new(recv, longest + 1));
    }
}

static VALUE comm_reduce_scatter(VALUE self, VALUE obj, VALUE rcounts, 
                                 VALUE rop)
{
    int rv, i;
    int rank, csize, my_size;
    int *recvcounts, count = 0;
    int arylen = -1, ary_ok, all_ok;
    char *send, *recv = NULL, *tmp;
    int length, longest, resize;
    struct mpi_comm *mc_comm;
    MPI_Datatype dtype;
    VALUE dump;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Comm_rank(*mc_comm->comm, &rank);
    mpi_exception(rv);
    rv = MPI_Comm_size(*mc_comm->comm, &csize);
    mpi_exception(rv);

    if (RARRAY(rcounts)->len != csize) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }

    /* Get the receive counts */
    recvcounts = ALLOCA_N(int, csize);
    for (i = 0; i < csize; i++)
        recvcounts[i] = FIX2INT(rb_ary_entry(rcounts, i));

    for (i = 0; i < csize; i++)
        count += FIX2INT(rb_ary_entry(rcounts, i));

    /* If an array was passed in, check that it's valid */
    if (TYPE(obj) == T_ARRAY)
        arylen = RARRAY(obj)->len;

    if (arylen >= 0)
        ary_ok = (arylen == count);
    else
        ary_ok = (count == 1);

    rv = MPI_Allreduce(&ary_ok, &all_ok, 1, MPI_INT, MPI_LAND, *mc_comm->comm);
    mpi_exception(rv);

    if (!all_ok) {
        mpi_exception(MPI_ERR_ARG);
        return Qnil;
    }

    if (arylen >= 0) {
        for (i = 0; i < arylen; i++) {
            dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
            StringValueCStr(dump);

            if (length > longest)
                longest = length;
        }

        length = longest;
    } else {
        dump = rb_funcall(mMarshal, id_dump, 1, obj);
        tmp = StringValueCStr(dump);
    }

    /* Global length check */
    rv = MPI_Allreduce(&length, &longest, 1, MPI_INT, MPI_MAX, *mc_comm->comm);
    mpi_exception(rv);

    rv = MPI_Type_contiguous(longest + 1, MPI_BYTE, &dtype);
    mpi_exception(rv);

    rv = MPI_Type_commit(&dtype);
    mpi_exception(rv);

    /* Allocation and copying */
    my_size = FIX2INT(rb_ary_entry(rcounts, rank));
    recv = ALLOCA_N(char, (longest + 1) * my_size);

    if (arylen >= 0) {
        VALUE ary;

        send = ALLOCA_N(char, (longest + 1) * arylen);
        for (i = 0; i < arylen; i++) {
            dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
            tmp = StringValueCStr(dump);

            MEMCPY(send + (i * (longest + 1)), tmp, char, length + 1);
        }

        rv = MPI_Reduce_scatter(send, recv, recvcounts, dtype, 
                                *op_get_mpi_op(rop), *mc_comm->comm);
        mpi_exception(rv);

        resize = op_get_resize(rop);
        while (resize > 0) {
            op_clear_resize(rop);

            rv = MPI_Type_free(&dtype);
            mpi_exception(rv);

            rv = MPI_Type_contiguous(resize, MPI_BYTE, &dtype);
            mpi_exception(rv);

            rv = MPI_Type_commit(&dtype);
            mpi_exception(rv);

            /* The send buffer is likely to have gotten munged. Reinit. */
            for (i = 0; i < arylen; i++) {
                dump = rb_funcall(mMarshal, id_dump, 1, rb_ary_entry(obj, i));
                tmp = StringValueCStr(dump);

                MEMCPY(send + (i * (longest + 1)), tmp, char, length + 1);
            }

            rv = MPI_Reduce_scatter(send, recv, recvcounts, dtype, 
                                    *op_get_mpi_op(rop), *mc_comm->comm);
            mpi_exception(rv);

            resize = op_get_resize(rop);
        }

        rv = MPI_Type_free(&dtype);
        mpi_exception(rv);

        ary = rb_ary_new2(my_size);
        for (i = 0; i < my_size; i++) {
            VALUE str, obj;

            str = rb_str_new(recv + (i * (longest + 1)), longest + 1);
            obj = rb_funcall(mMarshal, id_load, 1, str);
            rb_ary_store(ary, i, obj);
        }

        return ary;
    } else {
        send = ALLOCA_N(char, longest + 1);
        MEMCPY(send, tmp, char, length + 1);

        rv = MPI_Reduce_scatter(send, recv, recvcounts, dtype, 
                                *op_get_mpi_op(rop), *mc_comm->comm);
        mpi_exception(rv);

        resize = op_get_resize(rop);
        while (resize > 0) {
            op_clear_resize(rop);

            rv = MPI_Type_free(&dtype);
            mpi_exception(rv);

            rv = MPI_Type_contiguous(resize, MPI_BYTE, &dtype);
            mpi_exception(rv);

            rv = MPI_Type_commit(&dtype);
            mpi_exception(rv);

            /* The send buffer is likely to have gotten munged. Reinit. */
            dump = rb_funcall(mMarshal, id_dump, 1, obj);
            tmp = StringValueCStr(dump);
            MEMCPY(send, tmp, char, length + 1);

            rv = MPI_Reduce_scatter(send, recv, recvcounts, dtype, 
                                    *op_get_mpi_op(rop), *mc_comm->comm);
            mpi_exception(rv);

            resize = op_get_resize(rop);
        }

        rv = MPI_Type_free(&dtype);
        mpi_exception(rv);

        if (my_size > 0)
            return rb_funcall(mMarshal, id_load, 1, rb_str_new(recv,longest+1));
        else
            return Qnil;
    }
}

/* Probing */
static VALUE comm_probe(VALUE self, VALUE rsrc, VALUE rtag)
{
    int rv, src, tag;
    struct mpi_comm *mc_comm;
    MPI_Status *stat;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    if (rsrc == ANY_SOURCE) {
        src = MPI_ANY_SOURCE;
    } else {
        Check_Type(rsrc, T_FIXNUM);
        src = FIX2INT(rsrc);
    }

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    stat = ALLOC(MPI_Status);

    rv = MPI_Probe(src, tag, *mc_comm->comm, stat);
    mpi_exception(rv);
    
    return status_new(stat);
}

static VALUE comm_iprobe(VALUE self, VALUE rsrc, VALUE rtag)
{
    int rv, src, tag, flag;
    struct mpi_comm *mc_comm;
    MPI_Status *stat;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    if (rsrc == ANY_SOURCE) {
        src = MPI_ANY_SOURCE;
    } else {
        Check_Type(rsrc, T_FIXNUM);
        src = FIX2INT(rsrc);
    }

    if (rtag == ANY_TAG) {
        tag = MPI_ANY_TAG;
    } else {
        Check_Type(rtag, T_FIXNUM);
        tag = FIX2INT(rtag);
    }

    stat = ALLOC(MPI_Status);

    rv = MPI_Iprobe(src, tag, *mc_comm->comm, &flag, stat);
    mpi_exception(rv);
    
    return rb_ary_new3(2, flag ? Qtrue : Qfalse, status_new(stat));
}

/* Comparison */
static VALUE comm_equal(VALUE self, VALUE other)
{
    int rv, result;
    struct mpi_comm *mc_comm1, *mc_comm2;

    Data_Get_Struct(self, struct mpi_comm, mc_comm1);
    Data_Get_Struct(self, struct mpi_comm, mc_comm2);

    rv = MPI_Comm_compare(*mc_comm1->comm, *mc_comm2->comm, &result);
    mpi_exception(rv);

    return result == MPI_IDENT;
}

static VALUE comm_congruent_p(VALUE self, VALUE other)
{
    int rv, result;
    struct mpi_comm *mc_comm1, *mc_comm2;

    Data_Get_Struct(self, struct mpi_comm, mc_comm1);
    Data_Get_Struct(self, struct mpi_comm, mc_comm2);

    rv = MPI_Comm_compare(*mc_comm1->comm, *mc_comm2->comm, &result);
    mpi_exception(rv);

    return result == MPI_CONGRUENT;
}

static VALUE comm_similar_p(VALUE self, VALUE other)
{
    int rv, result;
    struct mpi_comm *mc_comm1, *mc_comm2;

    Data_Get_Struct(self, struct mpi_comm, mc_comm1);
    Data_Get_Struct(self, struct mpi_comm, mc_comm2);

    rv = MPI_Comm_compare(*mc_comm1->comm, *mc_comm2->comm, &result);
    mpi_exception(rv);

    return result == MPI_SIMILAR;
}

/* Intercomms */
static VALUE comm_inter_p(VALUE self)
{
    int rv, flag;
    struct mpi_comm *mc_comm;

    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Comm_test_inter(*mc_comm->comm, &flag);
    mpi_exception(rv);

    return flag ? Qtrue : Qfalse;
}

static VALUE comm_remote_size(VALUE self)
{
    int rv, size;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);
   
    rv = MPI_Comm_remote_size(*mc_comm->comm, &size);
    mpi_exception(rv);

    return rb_fix_new(size);
}

static VALUE comm_remote_group(VALUE self)
{
    int rv;
    struct mpi_comm *mc_comm;
    MPI_Group *grp;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    grp = ALLOC(MPI_Group);
   
    rv = MPI_Comm_remote_group(*mc_comm->comm, grp);
    mpi_exception(rv);

    return group_new(grp);
}

static VALUE comm_attr_put(VALUE self, VALUE keyval, VALUE obj)
{
    int rv;
    struct mpi_attr_val *av;
    struct mpi_comm *mc_comm;
    VALUE dump;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    av = ALLOC(struct mpi_attr_val);
    
    dump = rb_funcall(mMarshal, id_dump, 1, obj);
    av->data = StringValueCStr(dump);

    rv = MPI_Attr_put(*mc_comm->comm, keyval_get_keyval(keyval), av);
    mpi_exception(rv);
    
    return Qtrue;
}

static VALUE comm_attr_get(VALUE self, VALUE keyval)
{
    int rv, flag;
    struct mpi_attr_val *av;
    struct mpi_comm *mc_comm;
    VALUE str;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Attr_get(*mc_comm->comm, keyval_get_keyval(keyval), &av, &flag);
    mpi_exception(rv);
    
    if (flag) {
        str = rb_str_new(av->data, av->len);
        return rb_funcall(mMarshal, id_load, 1, str);
    } else {
        return Qnil;
    }
}

static VALUE comm_attr_delete(VALUE self, VALUE keyval)
{
    int rv;
    struct mpi_comm *mc_comm;
    
    Data_Get_Struct(self, struct mpi_comm, mc_comm);

    rv = MPI_Attr_delete(*mc_comm->comm, keyval_get_keyval(keyval));
    mpi_exception(rv);
    
    return Qtrue;
}

/* Constants */
static VALUE any_tag_to_s(VALUE self)
{
    return rb_str_new("MPI::Comm.ANY_TAG", sizeof("MPI::Comm.ANY_TAG"));
}

static VALUE any_source_to_s(VALUE self)
{
    return rb_str_new("MPI::Comm.ANY_SOURCE", sizeof("MPI::Comm.ANY_SOURCE"));
}

static VALUE self_to_s(VALUE self)
{
    return rb_str_new("MPI::Comm.SELF", sizeof("MPI::Comm.SELF"));
}

static VALUE world_to_s(VALUE self)
{
    return rb_str_new("MPI::Comm.WORLD", sizeof("MPI::Comm.WORLD"));
}

static VALUE graph_to_s(VALUE self)
{
    return rb_str_new("MPI::Comm.GRAPH", sizeof("MPI::Comm.GRAPH"));
}

static VALUE cart_to_s(VALUE self)
{
    return rb_str_new("MPI::Comm.CART", sizeof("MPI::Comm.CART"));
}

/* Initialization */

static void init_predef_comms()
{
    extern MPI_Comm *self, *world;
    struct mpi_comm *mc_self, *mc_world;

    mc_self = ALLOC(struct mpi_comm);
    mc_world = ALLOC(struct mpi_comm);

    mc_self->comm = self;
    mc_self->buffer = NULL;
    mc_self->bufsize = 0L;

    mc_world->comm = world;
    mc_world->buffer = NULL;
    mc_world->bufsize = 0L;

    SELF = intra_comm_new(mc_self);
    rb_define_singleton_method(SELF, "to_s", self_to_s, 0);
    rb_define_const(cComm, "SELF", SELF);

    WORLD = intra_comm_new(mc_world);
    rb_define_singleton_method(WORLD, "to_s", world_to_s, 0);
    rb_define_const(cComm, "WORLD", WORLD);
}

void Init_Comm()
{

	cComm = rb_define_class_under(mMPI, "Comm", rb_cObject);
	cIntraComm = rb_define_class_under(mMPI, "IntraComm", cComm);

    init_predef_comms();

    /* Environmental */
    rb_define_method(cComm, "size", comm_size, 0);
    rb_define_method(cComm, "rank", comm_rank, 0);
    rb_define_method(cComm, "group", comm_group, 0);
    rb_define_method(cComm, "abort", comm_abort, 1);

    /* Constructors */
    rb_define_method(cComm, "dup", comm_dup, 0);
    rb_define_method(cComm, "split", comm_split, 2);
    rb_define_singleton_method(cComm, "create", comm_create, 1);

    /* Topologies */
    rb_define_method(cComm, "cart_create", comm_cart_create, 3);
    rb_define_method(cComm, "graph_create", comm_graph_create, 3);
    rb_define_method(cComm, "topo_test", comm_topo_test, 0);
    rb_define_method(cComm, "graphdims", comm_graphdims_get, 0);
    rb_define_method(cComm, "graphdims_get", comm_graphdims_get, 0);
    rb_define_method(cComm, "graph", comm_graph_get, 0);
    rb_define_method(cComm, "graph_get", comm_graph_get, 0);
    rb_define_method(cComm, "cartdim", comm_cartdim_get, 0);
    rb_define_method(cComm, "cartdim_get", comm_cartdim_get, 0);
    rb_define_method(cComm, "cart", comm_cart_get, 0);
    rb_define_method(cComm, "cart_get", comm_cart_get, 0);
    rb_define_method(cComm, "cart_rank", comm_cart_rank, 1);
    rb_define_method(cComm, "cart_coords", comm_cart_coords, 1);
    rb_define_method(cComm, "graph_neighbors", comm_graph_neighbors, 1);
    rb_define_method(cComm, "graph_neighbors_count", 
                            comm_graph_neighbors_count, 1);
    rb_define_method(cComm, "cart_shift", comm_cart_shift, 2);
    rb_define_method(cComm, "cart_sub", comm_cart_sub, 1);
    rb_define_method(cComm, "cart_map", comm_cart_map, 2);
    rb_define_method(cComm, "graph_map", comm_graph_map, 2);

    /* Point to point */
    rb_define_method(cComm, "send", comm_send, 3);
    rb_define_method(cComm, "recv", comm_recv, 2);
    rb_define_method(cComm, "bsend", comm_bsend, 3);
    rb_define_method(cComm, "ssend", comm_ssend, 3);
    rb_define_method(cComm, "rsend", comm_rsend, 3);
    rb_define_method(cComm, "sendrecv", comm_sendrecv, 5);

    /* Non-blocking */
    rb_define_method(cComm, "isend", comm_isend, 3);
    rb_define_method(cComm, "ibsend", comm_ibsend, 3);
    rb_define_method(cComm, "issend", comm_issend, 3);
    rb_define_method(cComm, "irsend", comm_irsend, 3);
    rb_define_method(cComm, "irecv", comm_irecv, 2);

    /* Persistent */
    rb_define_method(cComm, "send_init", comm_send_init, 3);
    rb_define_method(cComm, "bsend_init", comm_bsend_init, 3);
    rb_define_method(cComm, "ssend_init", comm_ssend_init, 3);
    rb_define_method(cComm, "rsend_init", comm_rsend_init, 3);
    rb_define_method(cComm, "recv_init", comm_recv_init, 2);

    /* Buffering */
    rb_define_method(cComm, "buffer_for", comm_buffer_for, 1);
    rb_define_method(cComm, "unbuffer", comm_unbuffer, 0);
    
    /* Collective */
    rb_define_method(cIntraComm, "barrier", comm_barrier, 2);
    rb_define_method(cIntraComm, "bcast", comm_bcast, 2);
    rb_define_method(cIntraComm, "gather", comm_gather, 2);
    rb_define_method(cIntraComm, "allgather", comm_allgather, 1);
    rb_define_method(cIntraComm, "scatter", comm_scatter, 2);
    rb_define_method(cIntraComm, "alltoall", comm_alltoall, 1);
    rb_define_method(cIntraComm, "reduce", comm_reduce, 3);
    rb_define_method(cIntraComm, "allreduce", comm_allreduce, 2);
    rb_define_method(cIntraComm, "scan", comm_scan, 2);
    rb_define_method(cIntraComm, "reduce_scatter", comm_reduce_scatter, 3);

    /* Probing */
    rb_define_method(cComm, "probe", comm_probe, 2);
    rb_define_method(cComm, "iprobe", comm_iprobe, 2);
    
    /* Comparison */
    rb_define_method(cComm, "==", comm_equal, 1);
    rb_define_method(cComm, "congruent?", comm_congruent_p, 1);
    rb_define_method(cComm, "similar?", comm_similar_p, 1);
    
    /* Intercomms */
    rb_define_method(cComm, "inter?", comm_inter_p, 0);
    rb_define_method(cComm, "intercomm_create", comm_intercomm_create, 4);
    rb_define_method(cComm, "intercomm_merge", comm_intercomm_merge, 4);
    rb_define_method(cComm, "remote_size", comm_remote_size, 0);
    rb_define_method(cComm, "remote_group", comm_remote_group, 0);
    
    /* Attributes */
    rb_define_method(cComm, "attr_put", comm_attr_put, 2);
    rb_define_method(cComm, "attr_get", comm_attr_get, 1);
    rb_define_method(cComm, "attr_delete", comm_attr_delete, 1);

    /* Constants */
    ANY_TAG = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(ANY_TAG, "to_s", any_tag_to_s, 0);
    rb_define_const(cComm, "ANY_TAG", ANY_TAG);

    ANY_SOURCE = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(ANY_SOURCE, "to_s", any_source_to_s, 0);
    rb_define_const(cComm, "ANY_SOURCE", ANY_SOURCE);

    GRAPH = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(GRAPH, "to_s", graph_to_s, 0);
    rb_define_const(cComm, "GRAPH", GRAPH);

    CART = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(CART, "to_s", cart_to_s, 0);
    rb_define_const(cComm, "CART", CART);
}
