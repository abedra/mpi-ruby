printf("Hello, I am %d of %d\n", MPI::Comm::WORLD.rank(), MPI::Comm::WORLD.size())

$rank = MPI::Comm::WORLD.rank()
$sum = MPI::Comm::WORLD.reduce($rank, MPI::Op::SUM, 0)
if $rank == 0
	puts "sum of the ranks = #{$sum}"
end
