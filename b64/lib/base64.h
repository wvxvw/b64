#ifndef BASE64_H_
#define BASE64_H_

#include <stddef.h>

size_t encode_buffer(
    const unsigned char* buf_in, const size_t buf_in_size,
    const unsigned char* buf_out, const size_t buf_out_size,
    const size_t pre_pad, const size_t padding, size_t* written);

size_t decode_buffer(
    unsigned char* const buf_in, const size_t buf_in_size,
    unsigned char* const buf_out, const size_t buf_out_size,
    size_t* written);

#endif // BASE64_H_
