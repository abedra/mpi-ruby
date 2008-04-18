#ifndef MPI_RUBY_KEYVAL_H
#define MPI_RUBY_KEYVAL_H

struct mpi_attr_val {
    int len;
    char *data;
};

extern VALUE cKeyval;

extern int keyval_get_keyval(VALUE rkv);

extern void Init_Keyval();

#endif /* MPI_RUBY_KEYVAL_H */
