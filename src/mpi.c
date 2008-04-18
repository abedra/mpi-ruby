#include "mpi.h"
#include "ruby.h"

#include "global.h"

VALUE mMPI;

VALUE UNDEFINED;

VALUE mMarshal;
ID id_dump;
ID id_load;
ID id_map_bang;

static VALUE mpi_wtime(VALUE self)
{
    return rb_float_new(MPI_Wtime());
}

static VALUE mpi_wtick(VALUE self)
{
    return rb_float_new(MPI_Wtick());
}

static VALUE mpi_initialized_p(VALUE self)
{
    int rv, flag;
    
    rv = MPI_Initialized(&flag);
    mpi_exception(rv);
    
    return flag ? Qtrue : Qfalse;
}

static VALUE mpi_processor_name(VALUE self)
{
    int rv, len;
    char buf[MPI_MAX_PROCESSOR_NAME];
    
    rv = MPI_Get_processor_name(buf, &len);
    mpi_exception(rv);
    
    return rb_str_new(buf, len);
}

static VALUE mpi_dims_create(VALUE self, VALUE rnnodes, VALUE rdims)
{
    int rv, i, nnodes, ndims, *dims;
    VALUE ary;

    Check_Type(rnnodes, T_FIXNUM);
    nnodes = FIX2INT(rnnodes);
    
    ndims = RARRAY(rdims)->len;
    
    dims = ALLOCA_N(int, ndims);
    for (i = 0; i < ndims; i++)
        dims[i] = FIX2INT(rb_ary_entry(rdims, i));
    
    rv = MPI_Dims_create(nnodes, ndims, dims);
    mpi_exception(rv);
    
    ary = rb_ary_new2(ndims);
    for (i = 0; i < ndims; i++)
        rb_ary_store(ary, i, dims[i]);
    
    return ary;
}

static VALUE undefined_to_s(VALUE self)
{
    return rb_str_new("MPI::UNDEFINED", sizeof("MPI::UNDEFINED"));
}

void Init_MPI()
{
    int initialized;

    MPI_Initialized(&initialized);
    if (!initialized) {
        printf("Error!!! MPI Uninitialized!!!\n");
        return;
    }
    
    mMPI = rb_define_module("MPI");
    rb_define_module_function(mMPI, "wtime", mpi_wtime, 0);
    rb_define_module_function(mMPI, "wtick", mpi_wtick, 0);
    rb_define_module_function(mMPI, "initialized?", mpi_initialized_p, 0);
    rb_define_module_function(mMPI, "processor_name", mpi_processor_name, 0);
    rb_define_module_function(mMPI, "dims_create", mpi_dims_create, 2);
    
    UNDEFINED = rb_obj_alloc(rb_cObject);
    rb_define_singleton_method(UNDEFINED, "to_s", undefined_to_s, 0);
    rb_define_const(mMPI, "UNDEFINED", UNDEFINED);
 
    mMarshal = rb_const_get(rb_cObject, rb_intern("Marshal"));
    id_dump = rb_intern("dump");
    id_load = rb_intern("load");
    id_map_bang = rb_intern("map!");
    Init_Comm();
    Init_Group();
    Init_Status();
    Init_Request();
    Init_Op();
    Init_MPI_Exception();
}
