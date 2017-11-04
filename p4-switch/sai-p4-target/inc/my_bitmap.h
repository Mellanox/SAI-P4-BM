#ifndef _BITMAP_H
#define _BITMAP_H

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

typedef unsigned char* bitmap_t;

void alloc_bitmap(bitmap_t *bitmap, uint64_t size) {
	int num_of_bytes = (size % CHAR_BIT == 0) ? size / CHAR_BIT : size / CHAR_BIT + 1;
	*bitmap = (bitmap_t) calloc(num_of_bytes, 1);
	return;
}

void free_bitmap(bitmap_t bitmap) {
	free(bitmap);
}

void set_bitmap(bitmap_t bitmap, uint64_t pos) {
	bitmap[pos/CHAR_BIT] |= 1 << (pos % CHAR_BIT); 
}

void reset_bitmap(bitmap_t bitmap, uint64_t pos) {
	bitmap[pos/CHAR_BIT] &= ~(1 << (pos % CHAR_BIT)); 
}

int get_bitmap(bitmap_t bitmap, uint64_t pos) {
	return (bitmap[pos/CHAR_BIT] >> (pos%CHAR_BIT)) & 1;
}

void clear_all_bitmap(bitmap_t bitmap, uint64_t size) {
	int num_of_bytes = (size % CHAR_BIT == 0) ? size / CHAR_BIT : size / CHAR_BIT + 1;
	memset(bitmap, 0, num_of_bytes);
}

#endif
