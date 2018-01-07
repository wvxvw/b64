#include <stddef.h>
#include <stdio.h>

static const unsigned char* alphabet = (unsigned char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZ"   \
                                       "abcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char reverse_alphabet[140] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  63, 0,  0,  63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,
    0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  0,  0,  26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

size_t encode_buffer(
    unsigned char* const buf_in, const size_t buf_in_size,
    unsigned char* const buf_out, const size_t buf_out_size,
    const size_t pre_pad, const size_t padding, size_t* written) {

    /* printf("encoding(%d) %s\n", (int)buf_in_size, buf_in); */
    
    size_t i = 0, j = 0;
    size_t buf_in_blocks = (buf_in_size / 3) * 3;
    size_t buf_out_blocks = (buf_out_size / 4) * 4;
    size_t pad = pre_pad;
    size_t bytes[4];

    /* printf("encoding(%d/%d)\n", (int)buf_in_blocks, (int)buf_out_blocks); */

    while (i < buf_in_blocks && j < buf_out_blocks) {
        bytes[0] = buf_in[i] >> 2;
        bytes[1] = ((buf_in[i] & 0x3) << 4) | (buf_in[i + 1] >> 4);
        bytes[2] = ((buf_in[i + 1] & 0xF) << 2) | (buf_in[i + 2] >> 6);
        bytes[3] = buf_in[i + 2] & 0x3F;
        i += 3;

        size_t k = 0;
        while (k < 4) {
            if (j + k < pad) {
                buf_out[j + k] = alphabet[bytes[k]];
            } else {
                buf_out[j + k] = '\n';
                j++;
                buf_out[j + k] = alphabet[bytes[k]];
                pad += padding + 1;
            }
            k++;
        }
        j += 4;
    }
    *written = j;
    /* printf("encoded(%d) %s\n", (int)j, buf_out); */
    return i;
}

size_t decode_buffer(
    unsigned char* const buf_in, const size_t buf_in_size,
    unsigned char* const buf_out, const size_t buf_out_size,
    size_t* written) {

    size_t i = 0, j = 0;
    size_t buf_in_blocks = (buf_in_size / 4) * 4;
    size_t buf_out_blocks = (buf_out_size / 3) * 3;
    size_t bytes[3];
    size_t state = 0;

    while (i < buf_in_blocks && j < buf_out_blocks) {
        unsigned char c = buf_in[i];
        i++;
        if (c & 0x80 || (reverse_alphabet[c] == 0 && c != 'A')) {
            continue;
        }
        switch (state) {
            case 0:
                bytes[0] = reverse_alphabet[c] << 2;
                break;
            case 1:
                bytes[0] = bytes[0] | (reverse_alphabet[c] >> 4);
                bytes[1] = (reverse_alphabet[c] & 0xf) << 4;
                break;
            case 2:
                bytes[1] = bytes[1] | (reverse_alphabet[c] >> 2);
                bytes[2] = (reverse_alphabet[c] & 0x3) << 6;
                break;
            case 3:
                bytes[2] = bytes[2] | reverse_alphabet[c];
                buf_out[j] = bytes[0];
                buf_out[j + 1] = bytes[1];
                buf_out[j + 2] = bytes[2];
                j += 3;
                break;
        }
        state++;
        state = state % 4;
    }

    *written = j;
    return i;
}
