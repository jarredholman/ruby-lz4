
#include "rubylz4.h"
#include "varint.h"

#include <liblz4/lz4.h>
#include <liblz4/lz4hc.h>

/**
 * Raw lz4 stream with a simple varint header
 */
static VALUE rubylz4_simple_compress(int argc, VALUE *argv, VALUE obj)
{
  VALUE input_value;
  VALUE compression_level_value;
  rb_scan_args(argc, argv, "11", &input_value, &compression_level_value);
  
  Check_Type(input_value, T_STRING);
  const char* input_data = RSTRING_PTR(input_value);
  int input_size = RSTRING_LEN(input_value);

  int compression_level;
  if (NIL_P(compression_level_value)) {
    compression_level = 1;
  } else {
    Check_Type(compression_level_value, RUBY_T_FIXNUM);
    compression_level = FIX2INT(compression_level_value);
  }

  // do compress
  int max_compressed_size = LZ4_compressBound(input_size);
  int varint_size = varint_encoded_size(input_size);

  VALUE output = rb_str_new(NULL, max_compressed_size + varint_size);
  char* output_data = RSTRING_PTR(output);

  varint_encode(input_size, output_data);

  int compressed_size = LZ4_compress_fast(input_data, output_data + varint_size,
                                          input_size, max_compressed_size,
                                          compression_level);

  if (compressed_size == 0) {
    // special encoding if it fails
    int zero_varint_size = varint_encode(0, output_data);
    memcpy(output_data + varint_size, input_data, input_size);
    rb_str_resize(output, zero_varint_size + input_size);
  } else {
    rb_str_resize(output, varint_size + compressed_size);
  }

  return output;
}

static VALUE rubylz4_simple_compressHC(int argc, VALUE *argv, VALUE obj)
{
  VALUE input_value;
  VALUE compression_level_value;
  rb_scan_args(argc, argv, "11", &input_value, &compression_level_value);
  
  Check_Type(input_value, T_STRING);
  const char* input_data = RSTRING_PTR(input_value);
  int input_size = RSTRING_LEN(input_value);

  int compression_level;
  if (NIL_P(compression_level_value)) {
    compression_level = 1;
  } else {
    Check_Type(compression_level_value, RUBY_T_FIXNUM);
    compression_level = FIX2INT(compression_level_value);
  }

  // do compress
  int max_compressed_size = LZ4_compressBound(input_size);
  int varint_size = varint_encoded_size(input_size);

  VALUE output = rb_str_new(NULL, max_compressed_size + varint_size);
  char* output_data = RSTRING_PTR(output);

  varint_encode(input_size, output_data);

  int compressed_size = LZ4_compress_HC(input_data, output_data + varint_size,
                                        input_size, max_compressed_size,
                                        compression_level);

  if (compressed_size == 0) {
    // special encoding if it fails
    int zero_varint_size = varint_encode(0, output_data);
    memcpy(output_data + varint_size, input_data, input_size);
    rb_str_resize(output, zero_varint_size + input_size);
  } else {
    rb_str_resize(output, varint_size + compressed_size);
  }

  return output;
}

static VALUE rubylz4_simple_decompress(VALUE self, VALUE input)
{
  Check_Type(input, T_STRING);
  const char* input_data = RSTRING_PTR(input);
  int input_size = RSTRING_LEN(input);

  // do decompress
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
    rb_raise(rb_eRuntimeError, "%s", "decompress failed");
  } else {
    return output;
  }
}

void rubylz4_add_simple_module(VALUE mLz4)
{
  VALUE mLz4Simple = rb_define_module_under(mLz4, "Simple");

  rb_define_module_function(mLz4Simple, "compress", rubylz4_simple_compress, -1);
  rb_define_module_function(mLz4Simple, "compressHC", rubylz4_simple_compressHC, -1);
  rb_define_module_function(mLz4Simple, "decompress", rubylz4_simple_decompress, 1);
}
