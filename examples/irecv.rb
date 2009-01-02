hello = "Hello"

case MPI::Comm::WORLD.rank()
	when 0 
		printf("I'm 0 and I'm sending this message: '%s'\n", hello)
		MPI::Comm::WORLD.send(hello, 1, 0)
	when 1 
		req = MPI::Comm::WORLD.irecv(0, 0)
		printf("I'm 1 and I started a receive...\n")
		printf("Now I'm going to sleep for 2 seconds\n")
		sleep 2
		printf("Let's see if the message is ready...")
		testval = req.test
		if (testval[0] == true) 
			printf("yes\n")
		else
			printf("no\n")
		end
		req.wait
		printf("Ok, it's here: '%s'\n", req.object)
end
