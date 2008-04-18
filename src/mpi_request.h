#ifndef MPI_RUBY_REQUEST_H
#define MPI_RUBY_REQUEST_H

extern VALUE cRequest;

/* P = Persistant */
typedef enum {NORMAL, PSEND, PBSEND, PSSEND, PRSEND, RECV} req_type; 

/*
   request_new(NORMAL, MPI_Request *req);
   request_new(P[BSR]SEND, VALUE obj, int dest, int tag, MPI_Comm *comm);
   request_new(RECV, int src, int tag, MPI_Comm *comm);
*/

extern VALUE request_new(req_type type, ...);
extern void Init_Request();
extern VALUE request_start(VALUE self);
#endif /* MPI_RUBY_REQUEST_H */
