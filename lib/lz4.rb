require_relative 'lz4/version'
require_relative 'lz4/lz4'

module LZ4
    module_function

    def compress(value)
        LZ4::Frame.compress(value)
    end

    def decompress(value)
        LZ4::Frame.decompress(value)
    end
end
