#ifndef BIT_COUNTER_H
#define BIT_COUNTER_H

#include <stdint.h>

unsigned int bit_count(uint64_t bitboard);
unsigned int bit_count32(uint32_t number);
void test32();
void test64();

#endif
