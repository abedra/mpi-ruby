#
# This file is only used for documentation.  It has no functioning code! 
# All of these functions are in the module MPI which is automatically 
# 'required' into the mpi_ruby environment.
#

=begin

= MPI::Comm

== Description

A wrapper class around MPI communicators

== Methods

=== Constructors

--- MPI::Comm#dup()

    Duplicates the communicator and returns a new communicator.
	
    C equivalent: (({MPI_Comm_dup()}))

--- MPI::Comm.create(grp)

    Creates a new communicator from the ((|grp|)) (of type MPI::Group).

    C equivalent: (({MPI_Comm_create()}))

--- MPI::Comm#split(color, key)

    Splits the receiver into new communicators indicated by ((|color|)).
    ((|key|)) is used to determine ordering in the new communicators.

    C equivalent: (({MPI_Comm_split()}))

--- MPI::Comm#intercomm_create(local_leader, peer, remote_leader, tag)

    Creates a new intercommunicator using the receiver and ((|peer|)).

    C equivalent: (({MPI_Intercomm_create()}))

--- MPI::Comm#intercomm_merge(high)

    Creates a new intracommunicator from the two communicators of the receiver.
    ((|high|)) is used to determine the ranking in the new communicator.

    C equivalent: (({MPI_Intercomm_merge()}))

==== Topology Constructors

--- MPI::Comm#cart_create(dims, periods, reorder)

    Creates a new communicator to which cartesian topology information is
    attached.  ((|dims|)) is an array specifying the number of processes in
    each dimension.  ((|periods|)) is an array of logical values specifying
    whether the grid is periodic or not in each dimension.  ((|reorder|)) is
    a logical value indicating whether the processes may be reordered in the
    new topology.

    C equivalent: (({MPI_Cart_create()}))

--- MPI::Comm#graph_create(index, edges, reorder)

    Creates a new communicator to which graph topology information is
    attached.  ((|index|)) is an array in which the ((|i|))-th entry is the
    the total number of neighbors of the first ((|i|)) nodes.  ((|edges|))
    is a flattened array of the edge lists for nodes 0, 1, ...(({index.length}))
    ((|reorder|)) is a logical value indicating whether the processes may 
    be reordered in the new topology.

    C equivalent: (({MPI_Graph_create()}))

=== Attributes

--- MPI::Comm#attr_put(keyval, obj)

    Sets the attribute associated with MPI::Keyval ((|keyval|)) and the
    receiver communicator to ((|obj|)).  Returns true.

    C equivalent:  (({MPI_Attr_put()}))

--- MPI::Comm#attr_get(keyval)

    Returns the object attribute associated with MPI::Keyval ((|keyval|)) and
    the receiver communicator if one exists and nil otherwise.

    C equivalent:  (({MPI_Attr_get()}))
    
--- MPI::Comm#attr_delete(keyval)

    Deletes the object attribute associated with MPI::Keyval ((|keyval|)) and
    the receiver communicator if one exists.  Returns true.

    C equivalent:  (({MPI_Attr_delete()}))
    
=== Informational Methods

--- MPI::Comm#size()

    Returns the size of the communicator.

    C equivalent: (({MPI_Comm_size()}))

--- MPI::Comm#rank()

    Returns the rank of the current process within the communicator.

    C equivalent: (({MPI_Comm_rank()}))

--- MPI::Comm#group()

    Returns a handle to the group of the receiver communicator.

    C equivalent: (({MPI_Comm_group()}))

--- MPI::Comm#inter?()

    Returns true if the receiver is an intercommunicator.

    C equivalent: (({MPI_Comm_test_inter()}))

--- MPI::Comm#remote_size()

    Returns the size of the remote communicator of the intercommunicator.

    C equivalent: (({MPI_Comm_remote_size()}))

--- MPI::Comm#remote_group()

    Returns the remote group communicator of the intercommunicator.

    C equivalent: (({MPI_Comm_remote_group()}))

==== Topology Specific Information Methods

--- MPI::Comm#topo_test()

    Returns MPI::Comm.GRAPH if the topology associated with the receiver
    communicator is a graph, MPI::Comm.CART if the topology is cartesian, and
    MPI::UNDEFINED if there is no associated topology.

    C equivalent: (({MPI_Topo_test()}))

--- MPI::Comm#graphdims()

    Returns an array of two elements: the first being an integer describing the
    number of nodes and the second being an integer describing the number of 
    edges in the graph topology.

    C equivalent: (({MPI_Graphdims_get()}))

--- MPI::Comm#graphdims_get()

    An alias for MPI::Comm#graphdims().

    C equivalent: (({MPI_Graphdims_get()}))
    
--- MPI::Comm#graph()

    Returns an array of two elements containing the arrays ((|index|)) and
    ((|edges|)) as given to the MPI::Comm#graph_create() method, respectively.

    C equivalent: (({MPI_Graph_get()}))
    
--- MPI::Comm#graph_get()

    An alias for MPI::Comm#graph()

    C equivalent: (({MPI_Graph_get()}))
    
--- MPI::Comm#cartdim()

    Returns the number of dimensions in the cartesian topology attached to the
    receiver.
    
    C equivalent: (({MPI_Cartdim_get()}))

--- MPI::Comm#cartdim_get()

    An alias for MPI::Comm#cartdim().
    
    C equivalent: (({MPI_Cartdim_get()}))

--- MPI::Comm#cart()

    Returns an array of 3 elements containing a ((|dims|)) array, ((|periods|))
    array, and ((|coords|)) array as given to cart_create().

    C equivalent: (({MPI_Cart_get()}))

--- MPI::Comm#cart_get()

    An alias for MPI::Comm#cart()

    C equivalent: (({MPI_Cart_get()}))

--- MPI::Comm#cart_rank(coords)

    Returns the rank of the process at the coordinates given in the array
    ((|coords|)).

    C equivalent: (({MPI_Cart_rank()}))

--- MPI::Comm#cart_coords(rank)

    Returns an array of the coordinates of the process specified by ((|rank|)).

    C equivalent: (({MPI_Cart_coords()}))

--- MPI::Comm#graph_neigbhors_count(rank)

    Returns an integer specifying the number of neighbors of the process 
    specified by ((|rank|)).

    C equivalent: (({MPI_Graph_neighbors_count()}))

--- MPI::Comm#graph_neigbhors(rank)

    Returns an array of the ranks of the neighbors of the process specified 
    by ((|rank|)).

    C equivalent: (({MPI_Graph_neighbors()}))

--- MPI::Comm#cart_shift(dir, disp)

    Returns an array of two elements containing the rank of the source process
    and the rank of the destination process, respectively, as might be used
    in MPI::Comm#sendrecv().  ((|dir|)) indicates the direction of the shift
    and ((|disp|)) indicates the displacement.

    C equivalent: (({MPI_Cart_shift()}))

--- MPI::Comm#cart_map(dims, periods)

    Returns an integer specifying the optimal placement for the calling 
    process on the physical machine.  ((|dims|)) and ((|periods|)) are as for
    MPI::Comm#cart_create().
    
    C equivalent: (({MPI_Cart_map()}))

--- MPI::Comm#graph_map(index, edges)

    Returns an integer specifying the optimal placement for the calling 
    process on the physical machine.  ((|index|)) and ((|edges|)) are as for
    MPI::Comm#graph_create()
    
    C equivalent: (({MPI_Graph_map()}))

=== Point-to-Point Communications

--- MPI::Comm#send(obj, dest, tag)

    Sends object ((|obj|)) to process ((|dest|)) within the communicator with
    tag ((|tag|)).  Returns true on success.

    C equivalent: (({MPI_Send()}))

--- MPI::Comm#recv(src, tag)

    Receives an object from process ((|src|)) within the communicator with tag
    ((|tag|)).  Returns an array of two elements containing the object
    received and an MPI::Status object, respectively.

    C equivalent: (({MPI_Recv()}))

--- MPI::Comm#bsend(obj, dest, tag)

    Sends object ((|obj|)) to process ((|dest|)) within the communicator with
    tag ((|tag|)).  Buffering is used (see ((<MPI::Comm#buffer_for()>))). 
    Returns true on success.

    C equivalent: (({MPI_Bsend()}))

--- MPI::Comm#ssend(obj, dest, tag)

    Sends object ((|obj|)) to process ((|dest|)) within the communicator with
    tag ((|tag|)) synchronously.  Returns true on success.

    C equivalent: (({MPI_Ssend()}))

--- MPI::Comm#rsend(obj, dest, tag)

    Sends object ((|obj|)) to process ((|dest|)) within the communicator with
    tag ((|tag|)) with the assumption that the receiver has already begun a
    receiving call.
    Returns true on success.

    C equivalent: (({MPI_Rsend()}))

--- MPI::Comm#sendrecv(obj, dest, dtag, src, stag)

    Sends object ((|obj|)) to process ((|dest|)) within the communicator with
    tag ((|dtag|)) and simultaneously starts a receive from process ((|src|))
    with tag ((|stag|)).  Returns an array of two elements containing the 
    object received and an MPI::Status object, respectively.

    C equivalent: (({MPI_Sendrecv()}))

==== Non-blocking Point-to-Point Communications

--- MPI::Comm#isend(obj, dest, tag)

    Performs a non-blocking send of object ((|obj|)) to process ((|dest|)) 
    within the communicator with tag ((|tag|)).  Returns a new MPI::Request
    object.

    C equivalent: (({MPI_Isend()}))

--- MPI::Comm#irecv(src, tag)

    Performs a non-blocking receive of an object from process ((|src|)) within 
    the communicator with tag ((|tag|)).  Returns a new MPI::Request object.

    C equivalent: (({MPI_Irecv()}))

--- MPI::Comm#ibsend(obj, dest, tag)

    Performs a non-blocking send of object ((|obj|)) to process ((|dest|)) 
    within the communicator with tag ((|tag|)).  Buffering is used 
    (see ((<MPI::Comm#buffer_for()>))).  Returns a new MPI::Request
    object.

    C equivalent: (({MPI_Ibsend()}))

--- MPI::Comm#issend(obj, dest, tag)

    Performs a non-blocking, synchronous send of object ((|obj|)) to process 
    ((|dest|)) within the communicator with tag ((|tag|)) tag ((|tag|)) 
    syncronously.  Returns a new MPI::Request object.

    C equivalent: (({MPI_Issend()}))

--- MPI::Comm#irsend(obj, dest, tag)

    Performs a non-blocking send of object ((|obj|)) to process ((|dest|)) 
    within the communicator with tag ((|tag|)) with the assumption that the 
    receiver has already begun a receiving call.  Returns a new MPI::Request 
    object.

    C equivalent: (({MPI_Irsend()}))

==== Persistent Communications

--- MPI::Comm#send_init(obj, dest, tag)

    Sets up a persistent send of object ((|obj|)) to process ((|dest|)) 
    within the communicator with tag ((|tag|)).  Returns a new MPI::Request
    object.

    C equivalent: (({MPI_Send_init()}))

--- MPI::Comm#recv_init(src, tag)

    Sets up a persistent receive of an object from process ((|src|)) within 
    the communicator with tag ((|tag|)).  Returns a new MPI::Request object.

    C equivalent: (({MPI_Recv_init()}))

--- MPI::Comm#bsend_init(obj, dest, tag)

    Sets up a persistent send of object ((|obj|)) to process ((|dest|)) 
    within the communicator with tag ((|tag|)).  Buffering is used 
    (see ((<MPI::Comm#buffer_for()>))).  Returns a new MPI::Request
    object.

    C equivalent: (({MPI_Bsend_init()}))

--- MPI::Comm#ssend_init(obj, dest, tag)

    Sets up a persistent, synchronous send of object ((|obj|)) to process 
    ((|dest|)) within the communicator with tag ((|tag|)) tag ((|tag|)) 
    syncronously.  Returns a new MPI::Request object.

    C equivalent: (({MPI_Ssend_init()}))

--- MPI::Comm#rsend_init(obj, dest, tag)

    Sets up a persistent send of object ((|obj|)) to process ((|dest|)) 
    within the communicator with tag ((|tag|)) with the assumption that the 
    receiver has already begun a receiving call.  Returns a new MPI::Request 
    object.

    C equivalent: (({MPI_Rsend_init()}))

=== Buffering Methods

--- MPI::Comm#buffer_for(ary)

    Sets up buffering on the receiver communicator to account for the objects
    in the array ((|ary|)).  Returns true.

    C equivalent: (({MPI_Buffer_attach()}))

--- MPI::Comm#unbuffer()

    Removes the buffer associated with the receiver communicator.  Returns
    true.

    C equivalent: (({MPI_Buffer_detach()}))

=== Collective Operations

--- MPI::Comm#barrier()

    Performs a barrier operation on the communicator.

    C equivalent: (({MPI_Barrier()}))

--- MPI::Comm#bcast(obj, root)

    Broadcasts ((|obj|)) from process ((|root|)) to all other processes.
    The value of ((|obj|)) is irrelevant for all processes other than 
    ((|root|)).  Returns a copy of ((|obj|)) all processes other than ((|root|))
    and ((|obj|)) itself on process ((|root|)).

    C equivalent: (({MPI_Bcast}))

--- MPI::Comm#gather(obj, root)

    Gathers ((|obj|)) from all processes in the communicator into an array
    and returns this array at process ((|root|)).  Given a rank ((|r|)), the
    ((|r|))-th element of the array is a copy of ((|obj|)) on process ((|r|)).
    Returns true on all processes other than ((|root|)).

    C equivalent: (({MPI_Gather(), MPI_Gatherv()}))

--- MPI::Comm#allgather(obj)

    Gathers ((|obj|)) from all processes in the communicator into an array
    and returns this array at all processes.  Given a rank ((|r|)), the
    ((|r|))-th element of the array is a copy of ((|obj|)) on process ((|r|)).

    C equivalent: (({MPI_Allgather(), MPI_Allgatherv()}))

--- MPI::Comm#scatter(ary, root)

    Scatters the contents of the array ((|ary|)) on the process with rank
    ((|root|)) to all processes in the communicator.  The array must have
    length equal to the number of processes in the communicator.  Returns
    the ((|i|))-th object in ((|ary|)) at the ((|i|))-th process.

    C equivalent: (({MPI_Scatter()}))

--- MPI::Comm#alltoall(ary)

    Scatters the contents of the array ((|ary|)) on every process to all 
    processes in the communicator.  The array must have length equal to 
    the number of processes in the communicator.  Returns an array on each
    process where the ((|i|))-th object in the array on the ((|j|))-th 
    process is ((|j|))-th object in ((|ary|)) at the ((|i|))-th process.

    C equivalent: (({MPI_Alltoall()}))

--- MPI::Comm#reduce(obj, op, root)

    Reduces the object ((|obj|)) given using the MPI::Op ((|op|)) and returns
    the result of the reduction on the process with rank ((|root|)).  On all 
    other processes, the method returns true.  If ((|obj|)) is an array, it 
    must have the same length on all processes and the operation is applied 
    to each object in the array.  To perform an operation on an array, pass 
    an array of the array to which the operation will be applied.

    C equivalent: (({MPI_Reduce()}))

--- MPI::Comm#allreduce(obj, op)

    Reduces the object ((|obj|)) given using the MPI::Op ((|op|)) and returns
    the result of the reduction to all processes.  If ((|obj|)) is an array, 
    it must have the same length on all processes and the operation is applied 
    to each object in the array.  To perform an operation on an array, pass 
    an array of the array to which the operation will be applied.

    C equivalent: (({MPI_Allreduce()}))

--- MPI::Comm#scan(obj, op)

    At each process ((|i|)), returns the value of a reduction on processes 
    0...((|i|)), inclusively.  If ((|obj|)) is an array, it must have the same 
    length on all processes and the operation is applied to each object in the 
    array.  To perform an operation on an array, pass an array of the array 
    to which the operation will be applied.

    C equivalent: (({MPI_Scan()}))

--- MPI::Comm#reduce_scatter(ary, counts, op)

    Performs an element-wise reduction on ((|ary|)) with MPI::Op ((|op|)) 
    and scatters the result according to the array ((|counts|)).  Process 0
    receives an array of the first ((|counts[0]|)) elements of the result of 
    the reduction, process 1 receives an array of the next ((|counts[1]|))
    elements of the result of the reduction, and so on.  ((|ary|)) must have
    the same length on all processes and this length must equal the sum of
    the values in ((|counts|)).

    C equivalent: (({MPI_Reduce_scatter()}))

=== Probing

--- MPI::Comm#probe(src, tag)

    Blocks until a message is available from process ((|src|)) with tag 
    ((|tag|)).  Returns a new MPI::Status object.

    C equivalent: (({MPI_Probe()}))

--- MPI::Comm#iprobe(src, tag)

    Returns an array of two elements.  The first element is a flag which
    is true if there is a message from the given source ((|src|)) with the
    given ((|tag|)).  If the flag is true, then the second element of the
    return array is a new MPI::Status object.  If the flag is false, the
    second element of the return array is to be ignored.

    C equivalent: (({MPI_Iprobe()}))

=== Comparison

--- MPI::Comm#==(comm)

    Returns true if the receiver and the given communicator ((|comm|)) are
    the same communicator.  Returns false otherwise.

    C equivalent: (({MPI_Comm_compare()}))

--- MPI::Comm#congruent?(comm)

    Returns true if the receiver and the given communicator ((|comm|)) are
    precisely congruent.  Returns false otherwise.

    C equivalent: (({MPI_Comm_compare()}))

--- MPI::Comm#similar?(comm)

    Returns true if the receiver and the given communicator ((|comm|)) are
    precisely similar.  Returns false otherwise.

    C equivalent: (({MPI_Comm_compare()}))

=== Abortive Methods

--- MPI::Comm#abort(errorcode)

    Aborts the MPI program with the given error code.

    C equivalent: (({MPI_Abort()}))

== Constants

((|MPI::Comm.ANY_TAG|))
    
    A value which may be passed to any method expecting a tag.  It specifies
    that any tag will match in that call.

((|MPI::Comm.ANY_SOURCE|))
    
    A value which may be passed to any method expecting a source rank.  It 
    specifies that any source rank will match in that call.

== Notes

The MPI binding to Ruby is intended solely for Single Program, Multiple Data
(SPMD) parallel computations.  Thus, none of these communication methods can
be intermixed with MPI programs written in other languages.  

Because of the Multiple Data aspect, sending objects should be viewed as
creating a copy of the original object on the recipient process.  There are
no remote method invocation concepts explicitly supported in this binding.

=end
