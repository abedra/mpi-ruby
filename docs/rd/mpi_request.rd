#
# This file is only used for documentation.  It has no functioning code! 
# All of these functions are in the module MPI which is automatically 
# 'required' into the mpi_ruby environment.
#

=begin

= MPI::Request

== Description

A wrapper class around MPI requests 

== Methods

=== Informational Methods

--- MPI::Request#null?()
    Returns true if the request is null.

    C equivalent: (({request == MPI_REQUEST_NULL}))

=== Wait Methods

--- MPI::Request#wait()
    Waits until the operation identified by the receiver request is complete.  
    If the communication object associated with this request was created by a 
    nonblocking send or receive call, then the request becomes null.
    Returns an MPI::Status object.

    C equivalent: (({MPI_Wait()}))

--- MPI::Request.waitany(ary)
    Waits until any of the operations identified by the requests in the 
    array of requests ((|ary|)) is complete.  If the communication object 
    associated with this request was created by a nonblocking send or 
    receive call, then the request becomes null.  Returns an array of 
    two elements containing the index of the completed request and an 
    MPI::Status object, respectively.

    C equivalent: (({MPI_Waitany()}))

--- MPI::Request.waitall(ary)
    Waits until all of the operations identified by the requests in the 
    array of requests ((|ary|)) are complete.  If the communication object 
    associated with any of these requests was created by a nonblocking send or 
    receive call, then those requests become null.  Returns an array of
    MPI::Status objects for each request in ((|ary|)).  The status of a
    request is at the same index in the return array as the request index
    in ((|ary|)).

    C equivalent: (({MPI_Waitall()}))

--- MPI::Request.waitsome(ary)
    Waits until at least one of the operations identified by the requests in 
    the array of requests ((|ary|)) are complete.  If the communication object 
    associated with any of these requests was created by a nonblocking send or 
    receive call, then those requests become null.  Returns an array of two
    elements containing an array of the completed requests and an array of
    MPI::Status objects for each completed request, respectively.  The status 
    of a request is at the same index in the return status array as the 
    request index in the return request array.

    C equivalent: (({MPI_Waitsome()}))

=== Test Methods

--- MPI::Request#test()
    Tests if the operation identified by request is complete.  If the 
    communication object associated with this request was created by a 
    nonblocking send or receive call, then the request becomes null.
    Returns an array of two elements containing a flag (true or false, 
    as the result of the test), and an MPI::Status object, respectively.
    The status object should be ignored if the flag is false.

    C equivalent: (({MPI_Test()}))

--- MPI::Request.testany(ary)
    Tests if any of the operations identified by the requests in the 
    array of requests ((|ary|)) is complete.  If the communication object 
    associated with this request was created by a nonblocking send or 
    receive call, then the request becomes null.  Returns an array of 
    three elements containing a flag (true or false, depending on whether 
    any of the requests were completed), the index of the completed request, 
    and an MPI::Status object, respectively.  The last two array elements
    should be ignored if the flag is false.

    C equivalent: (({MPI_Testany()}))

--- MPI::Request.testall(ary)
    Tests if all of the operations identified by the requests in the 
    array of requests ((|ary|)) are complete.  If the communication object 
    associated with any of these requests was created by a nonblocking send or 
    receive call, then those requests become null.  Returns an array of
    two elements containing a flag (true or false, depending on whether 
    all of the requests were completed) and an array of MPI::Status objects 
    for each request in ((|ary|)), respectively.  The status of a request is 
    at the same index in the return array as the request index in ((|ary|)).  
    The status array should be ignored if the flag is false.

    C equivalent: (({MPI_Testall()}))

--- MPI::Request.testsome(ary)
    Equivalent to (({MPI::Request#waitsome(ary)})) except that 
    (({MPI::Request#testsome(ary)})) does not block.
    
    C equivalent: (({MPI_Testsome()}))

=== Initiation and Cancellation

--- MPI::Request#cancel()
    Cancels the request.  Returns true.

    C equivalent: (({MPI_Cancel()}))

--- MPI::Request#start()
    Starts a request.  Returns true.

    C equivalent: (({MPI_Start()}))

--- MPI::Request.startall(ary)
    Starts all the requests in the array of requests ((|ary|)).  Returns true.

    C equivalent: (({MPI_Startall()}))

=== Accessors

--- MPI::Request#object
    Returns the object associated with the request.

    C equivalent: Accessing the buffer given in the creation of the request.

=end
