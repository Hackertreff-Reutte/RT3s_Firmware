#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define RB_EMPTY 1
#define RB_FULL 2
#define RB_READY 3

// uint8_t, because otherwise we get the pointer arithmetic error
typedef struct {
	uint8_t *read;
	uint8_t *write;
	size_t size;
    size_t typeSize;
	uint8_t *start;
	uint8_t status;
} RING_BUFFER;

void RB_setup(RING_BUFFER* buffer, size_t size, size_t typeSize);
uint8_t RB_write(RING_BUFFER* buffer, void* data);
uint8_t RB_read(RING_BUFFER* buffer, void* data);

#endif

