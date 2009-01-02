#
# This file is only used for documentation.  It has no functioning code! 
# All of these functions are in the module MPI which is automatically 
# 'required' into the mpi_ruby environment.
#

=begin

= MPI::Op

== Description

A wrapper class around MPI operators 

== Methods

=== Constructors

--- MPI::Op.create(proc, commute)
    
    Returns a new MPI::Op based on the Proc object ((|proc|)).  ((|proc|))
    must accept two arguments.  If ((|commute|)) is true, ((|proc|)) is 
    considered to be commutative.

    C equivalent: (({MPI_Op_create()}))
