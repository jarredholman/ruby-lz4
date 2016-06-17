require "rake/extensiontask"

if Gem.win_platform?
  task :devkit do
    begin
      require 'devkit'
    rescue LoadError
      warn 'Failed to load devkit, installation might fail'
    end
  end

  task :compile => [:devkit]
end

GEMSPEC = Gem::Specification.load('lz4.gemspec')

Rake::ExtensionTask.new("lz4", GEMSPEC) do |task|
  task.lib_dir = "lib/lz4"
end
