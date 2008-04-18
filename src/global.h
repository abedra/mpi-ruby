#ifndef GLOBAL_H
#define GLOBAL_H

extern VALUE mMPI;

extern VALUE UNDEFINED;

extern VALUE mMarshal;
extern ID id_dump;
extern ID id_load;
extern ID id_map_bang;

extern void Init_MPI();

#include "mpi_comm.h"
#include "mpi_group.h"
#include "mpi_keyval.h"
#include "mpi_request.h"
#include "mpi_status.h"
#include "mpi_op.h"
#include "mpi_exception.h"

#endif /* GLOBAL_H */
