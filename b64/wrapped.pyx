# -*- coding: utf-8 -*-

from cpython.buffer cimport (
    PyBUF_SIMPLE,
    PyBUF_WRITABLE,
    PyObject_CheckBuffer,
    PyObject_GetBuffer,
    PyBuffer_Release
)
from libc.stdio cimport FILE, fopen, fwrite, fclose, stdout


cdef extern from "lib/base64.h":
    size_t encode_buffer(
        unsigned char* const buf_in, const size_t buf_in_size,
        unsigned char* const buf_out, const size_t buf_out_size,
        const size_t pre_pad, const size_t padding, size_t* written);

    size_t decode_buffer(
        unsigned char* const buf_in, const size_t buf_in_size,
        unsigned char* const buf_out, const size_t buf_out_size,
        size_t* written);


cpdef object encode_file(object fin, str fout):
    cdef bytes py_filename = fout.encode()
    cdef char* cy_filename = py_filename
    cdef bytes py_mode = "w".encode()
    cdef const char* mode = py_mode
    cdef FILE* fd
    if fout != "-":
        fd = fopen(<const char*>py_filename, <const char*>mode)
        if <size_t>fd == 0:
            raise FileNotFoundError(fout)
    else:
        fd = stdout

    cdef unsigned char out_buff[4096]
    cdef size_t written = 0
    cdef size_t total_written = 0
    cdef size_t used = 0
    cdef size_t total_used = 0
    cdef size_t pad_start = 80

    if not PyObject_CheckBuffer(fin):
        raise TypeError("fin must follow the buffer protocol")
    cdef Py_buffer char_view
    PyObject_GetBuffer(fin, &char_view, PyBUF_SIMPLE)
    
    cdef size_t total_chars = char_view.len
    cdef size_t total_expected_chars = (total_chars // 3) * 3
    cdef size_t requested = 0

    while total_used < total_expected_chars:
        requested = total_chars - total_used;
        used = encode_buffer(
            &(<unsigned char*>char_view.buf)[total_used],
            requested,
            out_buff,
            4096,
            pad_start,
            80,
            &written,
        )
        if used == 0:
            PyBuffer_Release(&char_view)
            raise Exception(
                "Couldn't write any more data. Bytes written: {}".format(
                    total_used,
                ),
            )
        total_written += written
        pad_start = 80 - total_written % 81
        total_used += used
        if fwrite(out_buff, sizeof(char), written, fd) != written:
            fclose(fd)
            PyBuffer_Release(&char_view)
            raise Exception(
                "Couldn't write to file: {}. Bytes written: {}".format(
                    fout, total_used,
                ),
            )

    cdef unsigned char extra[4]
    extra[1] = extra[2] = extra[3] = 0
    if total_chars - total_expected_chars == 1:
        extra[0] = (<unsigned char*>char_view.buf)[total_used]
        encode_buffer(extra, 4, out_buff, 256, pad_start, 80, &written)
        out_buff[2] = 61
        out_buff[3] = 61
    elif total_chars - total_expected_chars == 2:
        extra[0] = (<unsigned char*>char_view.buf)[total_used]
        extra[1] = (<unsigned char*>char_view.buf)[total_used + 1]
        encode_buffer(extra, 4, out_buff, 256, pad_start, 80, &written)
        out_buff[3] = 61

    if fwrite(out_buff, sizeof(char), 4, fd) != 4:
        fclose(fd)
        PyBuffer_Release(&char_view)
        raise Exception(
            "Couldn't write to file: {}. Bytes written: {}".format(
                fout, total_used,
            ),
        )
    total_written += written

    PyBuffer_Release(&char_view)
    fclose(fd)

    return total_written
