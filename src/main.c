#include "mpi.h"
#include "ruby.h"
#include <signal.h>

#include "global.h"

MPI_Comm *self, *world;

int main(int argc, char *argv[])
{
    void (*sigusr1)(int), (*sigusr2)(int);
    
	MPI_Init(&argc, &argv);

    /* ruby_run() calls exit() (why?), so we have to call finalize this way. */
    atexit((void (*)(void))MPI_Finalize);

    /* Allow errors to be returned as exceptions in ruby */
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    /* This seems legitimate because comms can be passed by value to fns. */
    self = malloc(sizeof(MPI_Comm));
    if (self == NULL) {
        perror("Unable to allocate MPI::Comm::SELF");
        MPI_Finalize();
        exit(1);
    }
    *self = MPI_COMM_SELF;

    world = malloc(sizeof(MPI_Comm));
    if (self == NULL) {
        perror("Unable to allocate MPI::Comm::WORLD");
        MPI_Finalize();
        exit(1);
    }
    *world = MPI_COMM_WORLD;

    MPI_Barrier(*world);
    sigusr1 = signal(SIGUSR1, SIG_IGN);
    sigusr2 = signal(SIGUSR2, SIG_IGN);

	ruby_init();
  ruby_init_loadpath();
	ruby_options(argc, argv);

    signal(SIGUSR1, sigusr1);
    signal(SIGUSR2, sigusr2);
    MPI_Barrier(*world);

    Init_MPI();

	ruby_run();

    /* Unreachable */
	
	MPI_Finalize();

	return 0;
}
