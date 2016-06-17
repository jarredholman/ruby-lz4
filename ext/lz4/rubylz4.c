
#include <ruby.h>
#include <ruby/encoding.h>

#include <liblz4/lz4.h>
#include "varint.h"


static VALUE compress(VALUE self, VALUE input)
{
  Check_Type(input, T_STRING);
  const char* input_data = RSTRING_PTR(input);
  int input_size = RSTRING_LEN(input);

  int max_compressed_size = LZ4_compressBound(input_size);
  int varint_size = varint_encoded_size(input_size);

  VALUE output = rb_str_new(NULL, max_compressed_size + varint_size);
  char* output_data = RSTRING_PTR(output);

  varint_encode(input_size, output_data);

  int compressed_size = LZ4_compress_default(input_data, output_data + varint_size, input_size, max_compressed_size);

  if (compressed_size == 0) {
    int zero_varint_size = varint_encode(0, output_data);
    memcpy(output_data + varint_size, input_data, input_size);
    rb_str_resize(output, zero_varint_size + input_size);
  } else {
    rb_str_resize(output, varint_size + compressed_size);
  }

  return output;
}

static VALUE decompress(VALUE self, VALUE input)
{
  Check_Type(input, T_STRING);
  const char* input_data = RSTRING_PTR(input);
  int input_size = RSTRING_LEN(input);

  int uncompressed_size;
  int varint_size = varint_decode(&uncompressed_size, input_data);

  if (uncompressed_size == 0) {
    return rb_str_substr(input, varint_size, input_size - varint_size);
  }

  VALUE output = rb_str_new(NULL, uncompressed_size);
  char* output_data = RSTRING_PTR(output);

  int decompress_size = LZ4_decompress_safe(input_data + varint_size, output_data,
                                            input_size - varint_size, uncompressed_size);

  if (decompress_size < 0) {
    return Qnil;
  }

  return output;
}


void Init_lz4()
{
  VALUE mLz4 = rb_const_get(rb_cObject, rb_intern("LZ4"));

  rb_define_module_function(mLz4, "compress", compress, 1);
  rb_define_module_function(mLz4, "decompress", decompress, 1);
}
