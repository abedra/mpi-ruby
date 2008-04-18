# The Ruby Execution Daemon (red) for MPI Programs
# Original idea by Rob Ross

require "socket"

MAXPACKET = 100
$rank = MPI::Comm::WORLD.rank

if ($rank == 0)
	sockname = "/tmp/red.#{`whoami`.strip}"
	begin
		sock = UNIXServer.new(sockname)
	rescue Errno::EADDRINUSE
		puts "Error: Socket #{sockname} already in use!"
		MPI::Comm::WORLD.abort
	end
	while true do
		cxn = sock.accept
		code = ""
		while packet = cxn.recv(MAXPACKET) do
			code += packet
			if packet.length < MAXPACKET then
				break
			end
		end
		cxn.close

		MPI::Comm::WORLD.bcast(code, 0)

		if code == "shutdown" then
			File.delete(sockname)
			exit 0
		end

		# Create a new module to sandbox off the namespace
		mod = Module.new
		mod.module_eval code

		# Clean up after every job
		GC.start
	end
else
	while true do
		code = MPI::Comm::WORLD.bcast(nil, 0)

		if code == "shutdown" then
			exit 0
		end

		# Create a new module to sandbox off the namespace
		mod = Module.new
		mod.module_eval code

		# Clean up after every job
		GC.start
	end
end
