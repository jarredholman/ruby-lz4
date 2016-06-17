#include "varint.h"

int varint_encoded_size(int value)
{
    uint64_t remaining = value;
    int size = 1;
    remaining = (remaining >> 7);
    while (remaining) {
        size += 1;
        remaining = (remaining >> 7);
    }
    return size;
}

int varint_encode(int value, char* buffer)
{
    uint64_t remaining = value;
    int pos = 0;
    while (1) {
        uint8_t part = (uint8_t)(remaining & 0x7F);
        remaining = (remaining >> 7);

        if (remaining) {
            part |= 0x80;
            buffer[pos] = (char)part;
            pos += 1;
        } else {
            *buffer = (char)part;
            return pos + 1;
        }
    }
}

int varint_decode(int* outputValue, const char* buffer)
{
    int value = 0;
    int pos = 0;
    while (1) {
        uint8_t part = (uint8_t)buffer[pos];

        if (part & 0x80) {
            value = (value << 7) | (part & 0x7F);
            pos += 1;
        } else {
            value |= part;
            *outputValue = value;
            return pos + 1;
        }
    }
}


