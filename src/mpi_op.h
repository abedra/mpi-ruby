#ifndef MPI_RUBY_OP_H
#define MPI_RUBY_OP_H

#include "mpi.h"
#include "ruby/ruby.h"

extern void op_clear_resize(VALUE rop);
extern int op_get_resize(VALUE rop);
extern MPI_Op *op_get_mpi_op(VALUE rop);
extern void Init_Op();

#endif /* MPI_RUBY_OP_H */
