#include "mpi.h"
#include "ruby/ruby.h"
#include "global.h"

VALUE cStatus;

VALUE status_new(MPI_Status *stat)
{
    VALUE tdata;

    tdata = Data_Wrap_Struct(cStatus, NULL, free, stat);
    rb_obj_call_init(tdata, 0, NULL);

    return tdata;
}

static VALUE status_source(VALUE self)
{
    MPI_Status *stat;

    Data_Get_Struct(self, MPI_Status, stat);

    return rb_fix_new(stat->MPI_SOURCE);
}

static VALUE status_tag(VALUE self)
{
    MPI_Status *stat;

    Data_Get_Struct(self, MPI_Status, stat);

    return rb_fix_new(stat->MPI_TAG);
}

static VALUE status_cancelled_p(VALUE self)
{	
    int rv, flag;
    MPI_Status *stat;

    Data_Get_Struct(self, MPI_Status, stat);

    rv = MPI_Test_cancelled(stat, &flag);

    return flag ? Qtrue : Qfalse;
}

void Init_Status()
{
    cStatus = rb_define_class_under(mMPI, "Status", rb_cObject);

    rb_define_method(cStatus, "source", status_source, 0);
    rb_define_method(cStatus, "tag", status_tag, 0);
    rb_define_method(cStatus, "cancelled?", status_cancelled_p, 0);
}
