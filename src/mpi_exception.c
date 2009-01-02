#include "mpi.h"
#include "ruby.h"
#include "global.h"

VALUE cException;

void mpi_exception(int retcode)
{
    switch (retcode) {
        case MPI_SUCCESS:
            break;
        case MPI_ERR_BUFFER:
            rb_raise(cException, "Invalid buffer pointer");
            break;
        case MPI_ERR_COUNT:
            rb_raise(cException, "Invalid count argument");
            break;
        case MPI_ERR_TYPE:
            rb_raise(cException, "Invalid datatype argument");
            break;
        case MPI_ERR_TAG:
            rb_raise(cException, "Invalid tag argument");
            break;
        case MPI_ERR_COMM:
            rb_raise(cException, "Invalid communicator");
            break;
        case MPI_ERR_RANK:
            rb_raise(cException, "Invalid rank");
            break;
        case MPI_ERR_REQUEST:
            rb_raise(cException, "Invalid request handle");
            break;
        case MPI_ERR_ROOT:
            rb_raise(cException, "Invalid root");
            break;
        case MPI_ERR_GROUP:
            rb_raise(cException, "Invalid group");
            break;
        case MPI_ERR_OP:
            rb_raise(cException, "Invalid operation");
            break;
        case MPI_ERR_TOPOLOGY:
            rb_raise(cException, "Invalid topology");
            break;
        case MPI_ERR_DIMS:
            rb_raise(cException, "Invalid dimension argument");
            break;
        case MPI_ERR_ARG:
            rb_raise(cException, "Invalid argument");
            break;
        case MPI_ERR_UNKNOWN:
            rb_raise(cException, "Unknown error");
            break;
        case MPI_ERR_TRUNCATE:
            rb_raise(cException, "Message truncated on receive");
            break;
        case MPI_ERR_OTHER:
            rb_raise(cException, "Other error");
            break;
        case MPI_ERR_INTERN:
            rb_raise(cException, "Internal MPI error");
            break;
        case MPI_ERR_PENDING:
            rb_raise(cException, "Operation not complete");
            break;
        case MPI_ERR_IN_STATUS:
            rb_raise(cException, "Error available in status");
            break;
        default:
            rb_raise(cException, "Unknown error");
            break;
    }
}

void Init_MPI_Exception()
{
    cException = rb_define_class_under(mMPI, "Exception", rb_eException);
}
