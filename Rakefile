require 'rake'
require 'rake/testtask'

desc "Clean the build files"
task :clean do
  system("make clean")
end

desc "Build MPI Ruby"
task :build do 
  system("./configure && make && sudo make install")
end

task :default => ['clean','build'] do
end