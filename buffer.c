/*! \file buffer.c \brief Multipurpose byte buffer structure and methods. */
//*****************************************************************************
//
// File Name	: 'buffer.c'
// Title		: Multipurpose byte buffer structure and methods
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 9/23/2001
// Revised		: 9/23/2001
// Version		: 1.0
// Target MCU	: any
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "buffer.h"
#include "global.h"
#include <string.h>
#include "avr/io.h"
#include <util/atomic.h>

// global variables

// initialization

void bufferInit(cBuffer* buffer, unsigned char *start, unsigned short size)
{
  // begin critical section
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // set start pointer of the buffer
    buffer->dataptr = start;
    buffer->size = size;
    // initialize index and length
    buffer->dataindex = 0;
    buffer->datalength = 0;
  } // end critical section
}

// access routines
unsigned char  bufferGetFromFront(cBuffer* buffer)
{
  unsigned char data = 0;
  // begin critical section
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // check to see if there's data in the buffer
    if(buffer->datalength)
      {
	// get the first character from buffer
	data = buffer->dataptr[buffer->dataindex];
	// move index down and decrement length
	buffer->dataindex++;
	if(buffer->dataindex >= buffer->size)
	  {
	    buffer->dataindex -= buffer->size;
	  }
	buffer->datalength--;
      }
  } // end critical section
  // return
  return data;
}

void bufferDumpFromFront(cBuffer* buffer, unsigned short numbytes)
{
  // begin critical section
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // dump numbytes from the front of the buffer
    // are we dumping less than the entire buffer?
    if(numbytes < buffer->datalength)
      {
	// move index down by numbytes and decrement length by numbytes
	buffer->dataindex += numbytes;
	if(buffer->dataindex >= buffer->size)
	  {
	    buffer->dataindex -= buffer->size;
	  }
	buffer->datalength -= numbytes;
      }
    else
      {
	// flush the whole buffer
	buffer->datalength = 0;
      }
  } // end critical section
}

unsigned char bufferGetAtIndex(cBuffer* buffer, unsigned short index)
{
  unsigned char data;
  // begin critical section
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // return character at index in buffer
    data = buffer->dataptr[(buffer->dataindex+index)%(buffer->size)];
  } // end critical section
  return data;
}

unsigned char bufferAddToEnd(cBuffer* buffer, unsigned char data)
{
  unsigned char status = 0;
  // begin critical section
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // make sure the buffer has room
    if(buffer->datalength < buffer->size)
      {
	// save data byte at end of buffer
	buffer->dataptr[(buffer->dataindex + buffer->datalength) % buffer->size] = data;
	// increment the length
	buffer->datalength++;
	// return success
	status = -1;
      }
  } // end critical section
  return status;
}

unsigned char bufferAddToEndBuff(cBuffer* buffer, unsigned char *data, unsigned short len) {
  unsigned char status = 0;
  // begin critical section
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // make sure buffer has room
    if ((buffer->size - buffer->datalength) >= len) {
      if ((buffer->dataindex + buffer->datalength + len) > buffer->size) {
	unsigned short first = buffer->size - (buffer->dataindex + buffer->datalength);
	memcpy(buffer->dataptr + buffer->dataindex + buffer->datalength, data, first);
	memcpy(buffer->dataptr, data + first, len - first);
      } else {
	memcpy(buffer->dataptr + buffer->dataindex + buffer->datalength, data, len);
      }
      buffer->datalength += len;
      status = -1;
    }
  } // end critical section
  return status;
}

unsigned short bufferIsNotFull(cBuffer* buffer)
{
  unsigned short bytesleft = 0;
  // begin critical section
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // check to see if the buffer has room
    // return true if there is room
    bytesleft = (buffer->size - buffer->datalength);
  } // end critical section
  return bytesleft;
}

void bufferFlush(cBuffer* buffer)
{
  // begin critical section
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // flush contents of the buffer
    buffer->datalength = 0;
  } // end critical section
}

