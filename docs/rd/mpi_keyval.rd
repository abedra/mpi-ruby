#
# This file is only used for documentation.  It has no functioning code! 
# All of these functions are in the module MPI which is automatically 
# 'required' into the mpi_ruby environment.
#

=begin

= MPI::Keyval

== Description

A wrapper class around MPI keyvals 

== Methods

=== Constructors

--- MPI::Keyval.create(uniq)
    
    Returns a new MPI::Keyval.  If ((|uniq|)) is true, then the attribute
    associated with a keyval will be deleted from the original communicator
    to which it was attached when that communicator is duplicated.
    If ((|uniq|)) is false, then the attribute will simply be copied to the
    new communicator.

    C equivalent: (({MPI_Keyval_create()}))
