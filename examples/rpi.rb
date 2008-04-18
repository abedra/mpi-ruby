# Parallel computation of pi in Ruby

PI25DT = 3.141592653589793238462643
NINTERVALS = 10000

$rank = MPI::Comm::WORLD.rank()
$size = MPI::Comm::WORLD.size()

$startwtime = MPI.wtime()
$h = 1.0 / NINTERVALS
$sum = 0.0
($rank + 1).step(NINTERVALS, $size) do |i|
	x = $h * (i - 0.5)
	$sum += (4.0 / (1.0 + x**2))
end
mypi = $h * $sum

$pi = MPI::Comm::WORLD.reduce(mypi, MPI::Op::SUM, 0)

if $rank == 0 then
	printf "pi is ~= %.16f, error = %.16f\n", $pi, ($pi - PI25DT).abs
	$endwtime = MPI.wtime()
	puts "wallclock time = #{$endwtime-$startwtime}"
end
