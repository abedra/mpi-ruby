printf("Hello, I am %d of %d\n", MPI::Comm::WORLD.rank(), MPI::Comm::WORLD.size())

$rank = MPI::Comm::WORLD.rank()
$sum = MPI::Comm::WORLD.allreduce($rank, MPI::Op::SUM)
puts "sum of the ranks = #{$sum}"
