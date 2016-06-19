#include "rubylz4.h"

#include <liblz4/lz4frame.h>

static VALUE rubylz4_frame_compress(int argc, VALUE *argv, VALUE obj)
{
  VALUE input_value;
  VALUE compression_level_value;
  rb_scan_args(argc, argv, "11", &input_value, &compression_level_value);
  
  Check_Type(input_value, RUBY_T_STRING);
  const char* input_data = RSTRING_PTR(input_value);
  size_t input_size = RSTRING_LEN(input_value);

  int compression_level;
  if (NIL_P(compression_level_value)) {
    compression_level = 1;
  } else {
    Check_Type(compression_level_value, RUBY_T_FIXNUM);
    compression_level = FIX2INT(compression_level_value);
  }

  // do compress
  LZ4F_preferences_t pref = { 0 };
  pref.frameInfo.contentSize = input_size;
  pref.autoFlush = 1;
  pref.compressionLevel = compression_level;

  size_t max_compressed_size = LZ4F_compressFrameBound(input_size, &pref);

  VALUE output = rb_str_new(NULL, max_compressed_size);
  char* output_data = RSTRING_PTR(output);

  size_t compressed_size = LZ4F_compressFrame(output_data, max_compressed_size,
                                              input_data, input_size,
                                              &pref);

  if (LZ4F_isError(compressed_size)) {
    return Qnil;
  } else {
    rb_str_resize(output, compressed_size);
    return output;
  }
}

static VALUE rubylz4_frame_decompress(VALUE self, VALUE input)
{
  Check_Type(input, RUBY_T_STRING);
  const char* input_data = RSTRING_PTR(input);
  size_t input_size = RSTRING_LEN(input);

  LZ4F_decompressionContext_t decompressionContext;
  LZ4F_errorCode_t errorCode = LZ4F_createDecompressionContext(&decompressionContext, LZ4F_VERSION);

  if (LZ4F_isError(errorCode)) {
    LZ4F_freeDecompressionContext(decompressionContext);
    rb_raise(rb_eRuntimeError, "%s", "Could not create decompression context");
    return Qnil;
  }

  LZ4F_frameInfo_t frameInfo;
  LZ4F_decompressOptions_t decompressOptions = {0};

  size_t frameInfoSize = input_size;
  size_t requiredOutputBufferSize = LZ4F_getFrameInfo(decompressionContext, &frameInfo, input_data, &frameInfoSize);

  if (LZ4F_isError(requiredOutputBufferSize)) {
    LZ4F_freeDecompressionContext(decompressionContext);
    rb_raise(rb_eRuntimeError, "%s", "Could not read frame info");
    return Qnil;
  }

  size_t read_output_size = frameInfo.contentSize;
  VALUE output = rb_str_new(NULL, read_output_size);
  char* output_data = RSTRING_PTR(output);

  size_t read_input_size = input_size;
  size_t remaining_hint = LZ4F_decompress(decompressionContext,
                                          (void*)output_data, &read_output_size,
                                          (const void*)(input_data + frameInfoSize), &read_input_size,
                                          &decompressOptions);

  LZ4F_freeDecompressionContext(decompressionContext);

  if (LZ4F_isError(remaining_hint)) {
    rb_raise(rb_eRuntimeError, "%s", "decompress error");
  } else if (remaining_hint != 0) {
    rb_raise(rb_eRuntimeError, "%s %zu %zu %zu %zu %zu", "Could not decompress", remaining_hint, read_output_size, read_input_size, input_size, frameInfoSize);
  } else {
    return output;
  }
}

void rubylz4_add_frame_module(VALUE mLz4)
{
  VALUE mFrame = rb_define_module_under(mLz4, "Frame");

  rb_define_module_function(mFrame, "compress", rubylz4_frame_compress, -1);
  rb_define_module_function(mFrame, "decompress", rubylz4_frame_decompress, 1);
}
