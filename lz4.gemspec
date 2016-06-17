require File.expand_path('../lib/lz4/version', __FILE__)

Gem::Specification.new do |s|
  s.name = 'lz4'
  s.version = LZ4::VERSION
  s.summary = ''
  s.author = 'Jarred Holman'
  s.licenses = ['MIT']

  s.files = Dir['{lib,ext}/**/*.rb']
  s.files = Dir['vendor/**/*']
  
  s.platform = Gem::Platform::RUBY
  s.extensions << 'ext/lz4/extconf.rb'

  s.require_paths = ['lib', 'ext']
end