# Ruby Execution Daemon (red) for MPI Programs shutdown program
# Original idea by Rob Ross

require "socket"

sock = UNIXSocket.open("/tmp/red.#{`whoami`.strip}")

sock.send("shutdown", 0)
