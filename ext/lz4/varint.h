#include <stdint.h>

int varint_encoded_size(int value);

int varint_encode(int value, char* buffer);

int varint_decode(int* value, const char* buffer);
