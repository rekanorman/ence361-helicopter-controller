// ****************************************************************************
//
// File: circBufT.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Attribution: Based on circBufT.c, written by P.J. Bones UCECE.
//
// A variation of a circular buffer using a single read/write index, which
// provides the following operations:
// - writing to the buffer
// - reading the oldest value in the buffer (the value which will next be
//   overwritten).
//
// ****************************************************************************

#include <stdint.h>
#include "stdlib.h"

#include "circBufT.h"

// ****************************************************************************
// Initialise the circBuf instance, setting index to the start of the buffer.
// Dynamically allocate and clear the the memory and return a pointer to
// the data.  Return NULL if allocation fails.
// ****************************************************************************
uint32_t *initCircBuf(circBuf_t *buffer, uint32_t size) {
    buffer->index = 0;
    buffer->size = size;
    buffer->data = (uint32_t *) calloc(size, sizeof(uint32_t));
    return buffer->data;
}

// ****************************************************************************
// Insert the given entry at the current index location, advancing the index
// modulo (buffer size).
// ****************************************************************************
void circBufWrite(circBuf_t *buffer, uint32_t entry) {
    buffer->data[buffer->index] = entry;
    buffer->index++;
    if (buffer->index >= buffer->size) {
        buffer->index = 0;
    }
}

// ****************************************************************************
// Read the oldest value in the buffer (the value which will next be
// overwritten).
// ****************************************************************************
uint32_t circBufRead(circBuf_t *buffer) {
    return buffer->data[buffer->index];
}

// ****************************************************************************
// Releases the memory allocated to the buffer data, setting the pointer
// to NULL and other fields to 0.
// ****************************************************************************
void circBufFree(circBuf_t * buffer) {
    buffer->index = 0;
    buffer->size = 0;
    free(buffer->data);
    buffer->data = NULL;
}

