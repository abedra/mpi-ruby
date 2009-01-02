hello = "Hello"

case MPI::Comm::WORLD.rank()
	when 0 
		ary = MPI::Comm::WORLD.gather(hello, 0)
		ary.each_with_index do |msg, i|
			print "#{i} said: '#{msg}'\n"
		end
	else
		hello += ", I'm #{MPI::Comm::WORLD.rank()}"
		MPI::Comm::WORLD.gather(hello, 0)
end
