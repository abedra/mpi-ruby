#ifndef MPI_RUBY_EXCEPTION_H
#define MPI_RUBY_EXCEPTION_H

#include "ruby.h"

extern VALUE cException;
extern void mpi_exception(int retcode);
extern void Init_MPI_Exception();

#endif /* MPI_RUBY_EXCEPTION_H */
