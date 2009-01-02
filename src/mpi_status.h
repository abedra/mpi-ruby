#ifndef MPI_RUBY_STATUS_H
#define MPI_RUBY_STATUS_H

extern VALUE cStatus;

extern VALUE status_new(MPI_Status *stat);
extern void Init_Status();

#endif /* MPI_RUBY_STATUS_H */
