require 'mkmf'

$CFLAGS = '-I. -O3 -std=c99 -pedantic -DXXH_NAMESPACE=LZ4_'

Dir.chdir File.expand_path('..', __FILE__) do
    $srcs = Dir['**/*.c']
end

# add include path to the internal folder
# $(srcdir) is a root folder, where "extconf.rb" is stored
$INCFLAGS << " -I$(srcdir) -I$(srcdir)/liblz4"

# add folder, where compiler can search source files
$VPATH << "$(srcdir)" << "$(srcdir)/liblz4"


create_makefile 'lz4/lz4'
