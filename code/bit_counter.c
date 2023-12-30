/* Adapted from:
 * https://web.archive.org/web/20151229003112/http://blogs.msdn.com/b/jeuge/archive/2005/06/08/hakmem-bit-count.aspx */
#include <stdio.h>

#include "bit_counter.h"

/* dark magic */
unsigned int bit_count(uint64_t bb)
{
	uint64_t c2, c4;
	uint64_t m2 = 0x3333333333333333;
	c2 = bb - ((bb >> 1) & 0x5555555555555555);
	c4 = (c2 & m2) + ((c2 >> 2) & m2);
	return ((c4 + (c4 >> 4)) & 0x0F0F0F0F0F0F0F0F) % 255;
}
