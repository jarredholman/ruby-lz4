
#include "rubylz4.h"

void Init_lz4()
{
  VALUE mLz4 = rb_const_get(rb_cObject, rb_intern("LZ4"));

  rubylz4_add_frame_module(mLz4);
  rubylz4_add_raw_module(mLz4);
  rubylz4_add_simple_module(mLz4);
}
