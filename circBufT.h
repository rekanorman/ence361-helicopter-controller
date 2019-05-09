// ****************************************************************************
//
// File: circBufT.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Attribution: Based on circBufT.h, written by P.J. Bones UCECE.
//
// A variation of a circular buffer using a single read/write index, which
// provides the following operations:
// - writing to the buffer
// - reading the oldest value in the buffer (the value which will next be
//   overwritten).
//
// ****************************************************************************

#ifndef CIRCBUFT_H_
#define CIRCBUFT_H_


// ****************************************************************************
// Buffer structure
// ****************************************************************************
typedef struct {
    uint32_t size;     // Number of entries in buffer
    uint32_t index;    // Index for reading and writing
    uint32_t *data;    // Pointer to the data
} circBuf_t;

// ****************************************************************************
// Initialise the circBuf instance, setting index to the start of the buffer.
// Dynamically allocate and clear the the memory and return a pointer to
// the data.  Return NULL if allocation fails.
// ****************************************************************************
uint32_t *initCircBuf(circBuf_t *buffer, uint32_t size);

// ****************************************************************************
// Insert the given entry at the current index location, advancing the index
// modulo (buffer size).
// ****************************************************************************
void circBufWrite(circBuf_t *buffer, uint32_t entry);

// ****************************************************************************
// Read the oldest value in the buffer (the value which will next be
// overwritten).
// ****************************************************************************
uint32_t circBufRead(circBuf_t *buffer);

// ****************************************************************************
// Releases the memory allocated to the buffer data, setting the pointer
// to NULL and other fields to 0.
// ****************************************************************************
void circBufFree(circBuf_t *buffer);


#endif  // CIRCBUFT_H_
