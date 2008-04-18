# Ruby Execution Daemon (red) for MPI Programs submitter
# Original idea by Rob Ross

if ARGV.length < 1 then
	puts "usage: ruby redsubmit.rb <filename> [<filename> ...]"
end

require "socket"

sock = UNIXSocket.open("/tmp/red.#{`whoami`.strip}")

ARGV.each { |script|
	sock.send(File.new(script).read, 0)
}
