#
# This file is only used for documentation.  It has no functioning code! 
# All of these functions are in the module MPI which is automatically 
# 'required' into the mpi_ruby environment.
#

=begin

= MPI

== Description

The module containing all MPI classes and constants

== Methods

=== Informational Methods

--- MPI.wtime()

    Returns a double precision floating-point number that is the time in seconds
    since some arbitrary point of time in the past.  The point is guaranteed
    not to change during the lifetime of the process.

    C equivalent: (({MPI_Wtime()}))

--- MPI.wtick()

    Returns a double precision floating-point number that is the time in seconds
    between successive ticks of the clock.

    C equivalent: (({MPI_Wtick()}))

--- MPI.initialized?()

    Returns true if MPI has been initialized.  (Note that because of the
    runtime system of MPI Ruby, this will always return true.)

    C equivalent: (({MPI_Initialized()}))

--- MPI.processor_name()

    Returns the processor name of the current processor.

    C equivalent: (({MPI_Get_processor_name()}))

--- MPI.dims_create(nnodes, dims)

    Returns an array containing cartesian dimensions suitable for use with
    MPI::Comm#cart_create().  ((|nnodes|)) is an integer specifying the total
    number of processes.  The array ((|dims|)) should have length equal to 
    the desired number of dimensions and should contain integers.  If an entry
    ((|i|)) in ((|dims|)) is positive, the ((|i|))-th entry in the returned
    array will be precisely ((|dims[i]|)).  If ((|dims[i]|)) is 0, the
    corresponding entry in the returned array will be determined by the
    method.  A negative entry in ((|dims|)) is incorrect.

    C equivalent: (({MPI_Dims_create()}))

=== Constants

((|MPI::UNDEFINED|))
    
    An opaque value that is used by various methods to specify that a value is
    undefined.
    
=end
