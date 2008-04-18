hello = "Hello, I'm #{MPI::Comm::WORLD.rank()}"

ary = MPI::Comm::WORLD.allgather(hello)
ary.each_with_index do |msg, i|
	print "#{MPI::Comm::WORLD.rank()}: #{i} said '#{msg}'\n"
end
