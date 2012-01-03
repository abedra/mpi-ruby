#include "mpi.h"
#include "ruby/ruby.h"
#include "global.h"

VALUE cGroup;

static void group_free(MPI_Group *grp)
{
    int rv;

    rv = MPI_Group_free(grp);
    mpi_exception(rv);
    free(grp);
}

VALUE group_new(MPI_Group *grp)
{
    VALUE tdata;

    tdata = Data_Wrap_Struct(cGroup, NULL, group_free, grp);
    rb_obj_call_init(tdata, 0, NULL);

    return tdata;
}

MPI_Group *group_get_mpi_group(VALUE rgrp)
{
    MPI_Group *grp;

    Data_Get_Struct(rgrp, MPI_Group, grp);

    return grp;
}

static VALUE group_size(VALUE self)
{
    int rv, size;
    MPI_Group *grp;

    Data_Get_Struct(self, MPI_Group, grp);

    rv = MPI_Group_size(*grp, &size);
    mpi_exception(rv);

    return rb_fix_new(size);
}

static VALUE group_rank(VALUE self)
{
    int rv, rank;
    MPI_Group *grp;

    Data_Get_Struct(self, MPI_Group, grp);

    rv = MPI_Group_rank(*grp, &rank);
    mpi_exception(rv);

    if (rank == MPI_UNDEFINED)
        return UNDEFINED;

    return rb_fix_new(rank);
}

static VALUE group_translate_ranks(VALUE self, VALUE ary, VALUE rgrp2)
{
    VALUE outary;
    MPI_Group *grp1, *grp2;
    int rv, i, len, *ranks1, *ranks2;

    Data_Get_Struct(self, MPI_Group, grp1);
    Data_Get_Struct(grp2, MPI_Group, grp2);

    len = RARRAY(ary)->len;
    ranks1 = ALLOCA_N(int, len);
    ranks2 = ALLOCA_N(int, len);

    for (i = 0; i < len; i++)
        ranks1[i] = FIX2INT(rb_ary_entry(ary, i));

    rv = MPI_Group_translate_ranks(*grp1, len, ranks1, *grp2, ranks2);
    mpi_exception(rv);

    outary = rb_ary_new2(len);
    for (i = 0; i < len; i++) {
        if (ranks2[i] == MPI_UNDEFINED)
            rb_ary_store(outary, i, UNDEFINED);
        else
            rb_ary_store(outary, i, rb_fix_new(ranks2[i]));
    }

    return outary;
}

static VALUE group_equal(VALUE self, VALUE rgrp2)
{
    int rv, flag;
    MPI_Group *grp1, *grp2;

    Data_Get_Struct(self, MPI_Group, grp1);
    Data_Get_Struct(grp2, MPI_Group, grp2);

    rv = MPI_Group_compare(*grp1, *grp2, &flag);
    mpi_exception(rv);

    return flag == MPI_IDENT ? Qtrue : Qfalse;
}

static VALUE group_similar_p(VALUE self, VALUE rgrp2)
{
    int rv, flag;
    MPI_Group *grp1, *grp2;

    Data_Get_Struct(self, MPI_Group, grp1);
    Data_Get_Struct(grp2, MPI_Group, grp2);

    rv = MPI_Group_compare(*grp1, *grp2, &flag);
    mpi_exception(rv);

    return flag == MPI_SIMILAR ? Qtrue : Qfalse;
}

static VALUE group_union(VALUE self, VALUE rgrp2)
{
    int rv;
    MPI_Group *grp1, *grp2, *newgrp;

    Data_Get_Struct(self, MPI_Group, grp1);
    Data_Get_Struct(grp2, MPI_Group, grp2);

    newgrp = ALLOC(MPI_Group);

    rv = MPI_Group_union(*grp1, *grp2, newgrp);
    mpi_exception(rv);

    return group_new(newgrp);
}

static VALUE group_intersection(VALUE self, VALUE rgrp2)
{
    int rv;
    MPI_Group *grp1, *grp2, *newgrp;

    Data_Get_Struct(self, MPI_Group, grp1);
    Data_Get_Struct(grp2, MPI_Group, grp2);

    newgrp = ALLOC(MPI_Group);

    rv = MPI_Group_intersection(*grp1, *grp2, newgrp);
    mpi_exception(rv);

    return group_new(newgrp);
}

static VALUE group_difference(VALUE self, VALUE rgrp2)
{
    int rv;
    MPI_Group *grp1, *grp2, *newgrp;

    Data_Get_Struct(self, MPI_Group, grp1);
    Data_Get_Struct(grp2, MPI_Group, grp2);

    newgrp = ALLOC(MPI_Group);

    rv = MPI_Group_difference(*grp1, *grp2, newgrp);
    mpi_exception(rv);

    return group_new(newgrp);
}

static VALUE group_incl(VALUE self, VALUE ary)
{
    int rv, i, len, *ranks;
    MPI_Group *grp, *newgrp;

    Data_Get_Struct(self, MPI_Group, grp);

    newgrp = ALLOC(MPI_Group);

    len = RARRAY(ary)->len;
    ranks = ALLOCA_N(int, len);

    for (i = 0; i < len; i++)
        ranks[i] = FIX2INT(rb_ary_entry(ary, i));

    rv = MPI_Group_incl(*grp, len, ranks, newgrp);
    mpi_exception(rv);

    return group_new(newgrp);
}

static VALUE group_excl(VALUE self, VALUE ary)
{
    int rv, i, len, *ranks;
    MPI_Group *grp, *newgrp;

    Data_Get_Struct(self, MPI_Group, grp);

    newgrp = ALLOC(MPI_Group);

    len = RARRAY(ary)->len;
    ranks = ALLOCA_N(int, len);

    for (i = 0; i < len; i++)
        ranks[i] = FIX2INT(rb_ary_entry(ary, i));

    rv = MPI_Group_excl(*grp, len, ranks, newgrp);
    mpi_exception(rv);

    return group_new(newgrp);
}

static VALUE group_range_incl(VALUE self, VALUE ary)
{
    int rv, i, len, **ranks;
    MPI_Group *grp, *newgrp;

    Data_Get_Struct(self, MPI_Group, grp);

    newgrp = ALLOC(MPI_Group);

    len = RARRAY(ary)->len;
    ranks = ALLOCA_N(int *, len);

    for (i = 0; i < len; i++) {
        int j;
        VALUE range;
        
        range = rb_ary_entry(ary, i);
        ranks[i] = ALLOCA_N(int, 3);
        
        for (j = 0; j < 3; j++)
            ranks[i][j] = FIX2INT(rb_ary_entry(range, j));
    }

    /* Thank you, cdecl */
    rv = MPI_Group_range_incl(*grp, len, (int (*)[3])ranks, newgrp);
    mpi_exception(rv);

    return group_new(newgrp);
}

static VALUE group_range_excl(VALUE self, VALUE ary)
{
    int rv, i, len, **ranks;
    MPI_Group *grp, *newgrp;

    Data_Get_Struct(self, MPI_Group, grp);

    newgrp = ALLOC(MPI_Group);

    len = RARRAY(ary)->len;
    ranks = ALLOCA_N(int *, len);

    for (i = 0; i < len; i++) {
        int j;
        VALUE range;
        
        range = rb_ary_entry(ary, i);
        ranks[i] = ALLOCA_N(int, 3);
        
        for (j = 0; j < 3; j++)
            ranks[i][j] = FIX2INT(rb_ary_entry(range, j));
    }

    /* Thank you, cdecl */
    rv = MPI_Group_range_excl(*grp, len, (int (*)[3])ranks, newgrp);
    mpi_exception(rv);

    return group_new(newgrp);
}

void Init_Group()
{
    cGroup = rb_define_class_under(mMPI, "Group", rb_cObject);

    rb_define_method(cGroup, "size", group_size, 0);
    rb_define_method(cGroup, "rank", group_rank, 0);

    rb_define_method(cGroup, "translate_ranks", group_translate_ranks, 2);

    rb_define_method(cGroup, "==", group_equal, 1);
    rb_define_method(cGroup, "similar?", group_similar_p, 1);

    rb_define_method(cGroup, "|", group_union, 1);
    rb_define_method(cGroup, "union", group_union, 1);
    rb_define_method(cGroup, "-", group_difference, 1);
    rb_define_method(cGroup, "difference", group_difference, 1);
    rb_define_method(cGroup, "&", group_intersection, 1);
    rb_define_method(cGroup, "intersection", group_intersection, 1);

    rb_define_method(cGroup, "incl", group_incl, 1);
    rb_define_method(cGroup, "range_incl", group_range_incl, 1);
    rb_define_method(cGroup, "excl", group_excl, 1);
    rb_define_method(cGroup, "range_excl", group_range_excl, 1);
}
