printf("Hello, I am %d of %d\n", MPI::Comm::WORLD.rank(), MPI::Comm::WORLD.size())

hello = "Hello"

case MPI::Comm::WORLD.rank()
	when 0 
		printf("I'm 0 and I'm sending this message: '%s'\n", hello)
		MPI::Comm::WORLD.bcast(hello, 0)
	else
		msg = MPI::Comm::WORLD.bcast(nil, 0)
		printf("I'm %d and I got this message: '%s'\n", MPI::Comm::WORLD.rank(),
			   msg)
end

