#
# This file is only used for documentation.  It has no functioning code! 
# All of these functions are in the module MPI which is automatically 
# 'required' into the mpi_ruby environment.
#

=begin

= MPI::Status

== Description

A wrapper class around MPI status 

== Methods

=== Informational Methods

--- MPI::Status#source()

    Returns the source of the message to which the status object refers.

    C equivalent: (({status.MPI_STATUS}))

--- MPI::Status#tag()

    Returns the tag of the message to which the status object refers.

    C equivalent: (({status.MPI_TAG}))

--- MPI::Status#cancelled?()

    Returns whether or not the message to which the status object refers was
    cancelled.

    C equivalent: (({MPI_Test_cancelled()}))

