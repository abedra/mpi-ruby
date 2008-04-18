myrank = MPI::Comm::WORLD.rank()
csize = MPI::Comm::WORLD.size()

if myrank % 2 == 0 then
	if myrank + 1 != csize then
		hello = "Hello, I'm #{myrank}, you must be #{myrank+1}"
		MPI::Comm::WORLD.send(hello, myrank + 1, 0)
	end
else
	msg, status = MPI::Comm::WORLD.recv(myrank - 1, 0)
	puts "I'm #{myrank} and this message came from #{status.source} with tag #{status.tag}: '#{msg}'"
end
