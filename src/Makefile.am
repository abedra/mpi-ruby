CFLAGS=@CFLAGS@ @RUBY_CFLAGS@
LIBS=@LIBS@ @RUBY_LIBS@
LD_RUN_PATH=@RB_LIB_DIR@

bin_PROGRAMS = mpi_ruby
mpi_ruby_SOURCES = main.c mpi.c mpi_comm.c mpi_exception.c mpi_group.c \
                   mpi_keyval.c mpi_op.c mpi_request.c mpi_status.c \
				   global.h mpi_comm.h mpi_exception.h mpi_group.h \
				   mpi_keyval.h mpi_op.h mpi_request.h mpi_status.h
mpi_ruby_DEPENDENCIES = mpi_op_fns.c 
DISTCLEANFILES = mpi_op_fns.c

EXTRA_DIST = ops.rb
mpi_op_fns.c: ops.rb
	@RUBY@ ops.rb @MAX_OPS@ > mpi_op_fns.c

mpi_op.$(OBJEXT): mpi_op_fns.c
