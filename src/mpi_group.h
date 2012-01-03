#ifndef MPI_RUBY_GROUP_H
#define MPI_RUBY_GROUP_H

#include "mpi.h"
#include "ruby/ruby.h"

extern VALUE group_new(MPI_Group *grp);
extern MPI_Group *group_get_mpi_group(VALUE rgrp);

extern void Init_Group();

#endif /* MPI_RUBY_GROUP_H */
