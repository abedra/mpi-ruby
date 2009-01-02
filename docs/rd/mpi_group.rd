#
# This file is only used for documentation.  It has no functioning code! 
# All of these functions are in the module MPI which is automatically 
# 'required' into the mpi_ruby environment.
#

=begin

= MPI::Group

== Description

A wrapper class around MPI groups 

== Methods

=== Constructors

=== Informational Methods

--- MPI::Group#size()

    Returns the size of the group.

    C equivalent: (({MPI_Group_size()}))

--- MPI::Group#rank()

    Returns the rank of the current process within the communicator.

    C equivalent: (({MPI_Comm_rank()}))

--- MPI::Group#translate_ranks(ary, grp)

    Translates the group rankings listed in ((|ary|)) for the receiver group
    into rankings in ((|grp|)).  The new rankings are returned in an array, 
    ordered respectively to ((|ary|)).  If any of the rankings in ((|ary|)) 
    do not correspond to a ranking in ((|grp|)), the corresponding entry in
    the return array is (({MPI::UNDEFINED})).

    C equivalent: (({MPI_Group_translate_ranks()}))

--- MPI::Group#==(grp)

    Returns true if the receiver and ((|grp|)) are precisely equal.

    C equivalent: (({MPI_Group_compare()}))

--- MPI::Group#similar?(grp)

    Returns true if the receiver and ((|grp|)) are precisely similar.

    C equivalent: (({MPI_Group_compare()}))

=== Constructors
    
--- MPI::Group#|(grp)

    Returns the union of the receiver group and ((|grp|)).

    C equivalent: (({MPI_Group_union()}))

--- MPI::Group#union(grp)

    An alias for MPI::Group#|()

    C equivalent: (({MPI_Group_union()}))

--- MPI::Group#-(grp)

    Returns the difference of the receiver group and ((|grp|)).

    C equivalent: (({MPI_Group_difference()}))

--- MPI::Group#difference(grp)

    An alias for MPI::Group#-()

    C equivalent: (({MPI_Group_difference()}))

--- MPI::Group#&(grp)

    Returns the intersection of the receiver group and ((|grp|)).

    C equivalent: (({MPI_Group_intersection()}))

--- MPI::Group#intersection(grp)

    An alias for MPI::Group#&()

    C equivalent: (({MPI_Group_intersection()}))

--- MPI::Group#incl(ary)

    Returns a new group with only those ranks listed in ((|ary|))

    C equivalent: (({MPI_Group_incl()}))

--- MPI::Group#range_incl(ary)

    Returns a new group with only those ranks in an array of ranges, 
    ((|ary|)).  ((|ary|)) is an array in which each element is an array of 
    size three, describing a range.  The first element of one of these size 3
    arrays is the start of the range, the second element is the end of the
    range, and the third element is the stride taken between the range 
    endpoints.  The stride may be negative, but not zero.  The ranks must be
    distinct.

    C equivalent: (({MPI_Group_range_incl()}))

--- MPI::Group#excl(ary)

    Returns a new group with only those ranks not listed in ((|ary|))

    C equivalent: (({MPI_Group_excl()}))

--- MPI::Group#range_excl(ary)

    Returns a new group with only those ranks not in an array of ranges, 
    ((|ary|)).  ((|ary|)) is an array in which each element is an array of 
    size three, describing a range.  The first element of one of these size 3
    arrays is the start of the range, the second element is the end of the
    range, and the third element is the stride taken between the range 
    endpoints.  The stride may be negative, but not zero.  The ranks must be
    distinct.

    C equivalent: (({MPI_Group_range_excl()}))
