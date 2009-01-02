# User-defined operation example

$union = MPI::Op.create(Proc.new {|a, b| a | b}, true)

$rank = MPI::Comm::WORLD.rank
# Note that since we are passing an array in to be reduced, the allreduce
# assumes that we intend to give a list of things to be reduced, so this
# must be done by an array of an array of 1 element
$rankary = [[$rank]]

$rankary = MPI::Comm::WORLD.allreduce($rankary, $union)

class Array
	def to_s
		s = "["
		self.each { |e|
			s += e.to_s + ", "
		}
		s = s[0...-2]
		s += "]"
	end
end
puts "#{$rank}: #{$rankary}"
