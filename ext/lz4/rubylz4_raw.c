#include "rubylz4.h"

#include <liblz4/lz4.h>
#include <liblz4/lz4hc.h>

static VALUE rubylz4_raw_compressBound(VALUE input_size_value)
{
    Check_Type(input_size_value, RUBY_T_FIXNUM);
    int input_size = FIX2INT(input_size_value);

    int max_compressed_size = LZ4_compressBound(input_size);

    return INT2FIX(max_compressed_size);
}

static VALUE rubylz4_raw_compress(VALUE self, VALUE input, VALUE compression_level_value)
{
  Check_Type(input, RUBY_T_STRING);
  const char* input_data = RSTRING_PTR(input);
  int input_size = RSTRING_LEN(input);

  Check_Type(compression_level_value, RUBY_T_FIXNUM);
  int compression_level = FIX2INT(compression_level_value);

  // do compress
  int max_compressed_size = LZ4_compressBound(input_size);

  VALUE output = rb_str_new(NULL, max_compressed_size);
  char* output_data = RSTRING_PTR(output);

  int compressed_size = LZ4_compress_fast(input_data, output_data,
                                          input_size, max_compressed_size,
                                          compression_level);

  if (compressed_size == 0) {
    rb_raise(rb_eRuntimeError, "%s", "compress failed");
  } else {
    rb_str_resize(output, compressed_size);
    return output;
  }
}

static VALUE rubylz4_raw_compressHC(VALUE self, VALUE input, VALUE compression_level_value)
{
  Check_Type(input, RUBY_T_STRING);
  const char* input_data = RSTRING_PTR(input);
  int input_size = RSTRING_LEN(input);

  Check_Type(compression_level_value, RUBY_T_FIXNUM);
  int compression_level = FIX2INT(compression_level_value);

  // do compress
  int max_compressed_size = LZ4_compressBound(input_size);

  VALUE output = rb_str_new(NULL, max_compressed_size);
  char* output_data = RSTRING_PTR(output);

  int compressed_size = LZ4_compress_HC(input_data, output_data,
                                        input_size, max_compressed_size,
                                        compression_level);

  if (compressed_size == 0) {
    rb_raise(rb_eRuntimeError, "%s", "compress failed");
  } else {
    rb_str_resize(output, compressed_size);
    return output;
  }
}

static VALUE rubylz4_raw_decompress(VALUE self, VALUE input, VALUE max_decompresed_size_value)
{
  Check_Type(input, RUBY_T_STRING);
  const char* input_data = RSTRING_PTR(input);
  int input_size = RSTRING_LEN(input);

  Check_Type(max_decompresed_size_value, RUBY_T_FIXNUM);
  int max_decompresed_size = FIX2INT(max_decompresed_size_value);

  VALUE output = rb_str_new(NULL, max_decompresed_size);
  char* output_data = RSTRING_PTR(output);

  int decompress_size = LZ4_decompress_safe(input_data, output_data,
                                            input_size, max_decompresed_size);

  if (decompress_size < 0) {
    rb_raise(rb_eRuntimeError, "%s", "decompress failed");
  } else {
    rb_str_resize(output, decompress_size);
    return output;
  }
}

void rubylz4_add_raw_module(VALUE mLz4)
{
  VALUE mRaw = rb_define_module_under(mLz4, "Raw");

  rb_define_module_function(mRaw, "compressBound", rubylz4_raw_compressBound, 1);
  rb_define_module_function(mRaw, "compress", rubylz4_raw_compress, 2);
  rb_define_module_function(mRaw, "compressHC", rubylz4_raw_compressHC, 2);
  rb_define_module_function(mRaw, "decompress", rubylz4_raw_decompress, 2);
}
