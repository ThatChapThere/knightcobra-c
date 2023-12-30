/* Adapted from:
 * https://web.archive.org/web/20151229003112/http://blogs.msdn.com/b/jeuge/archive/2005/06/08/hakmem-bit-count.aspx */
#include <stdio.h>

#include "bit_counter.h"

/*
 * Differences from 32 bit:
 *
 * We need to take a modulus of 127 at the end, because 6 bits can't hold the number 64.
 * That said, for chess purposes we can have a maximum of 10 of a single type of piece. So maybe we could make a faster
 * one for chessman bitboards. If we have more complex bitboards we'll need the full one though.
 * For now lets just make a full one.
 *
 * Another difference is that 64 is two less than a muliple of 3 (66) while 32 is 1 less (33). So we'll need a single
 * lonely bit rather than a couple at the start.
 *
 * We might also consider 8 bit bins rather than 7 bit bins.
 * Actually I'm not sure how 7 bit bins would even work. You'd have to add some unholy combination of 3 and 4 bit bins.
 * I suspect that's doable though.
 *
 * I'm also thinking that the 32 bit solution could have gone 2 > 4 > 8 instead of 3 > 6, since both require 3 shifts.
 * I wonder how costly the mod 255 operation is then, given that they didn't opt for this.
 * 
 * 2 > 4 > 8 straight up doesn't work
 *
 * This is because we actually need to fit a count into the PREVIOUS bin size. Three works because 6 can be expressed
 * as three digits (110), but 4 cannot ((1)00).
 *
 * So we always need to start with 3.
 */
unsigned int bit_count(uint64_t u)
{
	uint64_t three_bit_counts,
	         six_bit_counts;
	unsigned int final_count;
	three_bit_counts = u - ((u >> 1) & 01333333333333333333333) - ((u >> 2) & 01111111111111111111111);
	six_bit_counts = ((three_bit_counts + (three_bit_counts >> 3)) & 0707070707070707070707);
	final_count = ((six_bit_counts + (six_bit_counts >> 6)) & 01700770077007700770077) % 4095;
	return final_count;
}

/*
 * Breakdown of why this works:
 *
 * Part 1: We split the 32 bits into 3 bit bins
 *
 * Obviously 32 isn't a multiple of 3. This doesn't matter because the the first two bits are quite happy being counted
 * as a bin - they don't need a bit before them.
 * Each bin is then turned into a count.
 *
 * Part 2: How are these counts?
 *
 * Lets say you have a string of n bits, and you want to count the ones. Obviously this is what we're doing here with
 * n = 32, but for n = 3 (and to prevent infinite recursion) we need something simpler.
 * The most obvious way is to have a counter integer and increment it every time we find a one bit
 * eg.
 *     unsigned int count
 *     for(; u; u >> 1) count += u & 1;
 * However, there is a slightly fancier way. We can take u and subtract every possible right bit shift.
 * At first this seems absurd, but if we consider each bit individually it's obvious.
 * Every bit shift of 100 is just 10 and 1, every bit shift of 10 is just 1 and so on.
 * So every bit contributes 1000... - 0111... = 1 to the final sum.
 *
 * Part 3: Why don't these counts overlap?
 * 
 * Subtracting shifts on an individual number worked because bits were getting shifted off the edge. When we're dealing
 * with bins in a larger number, shifting is obviously not off the edge. To solve for this we simply use the & operator
 * with a series of octal 1s and 3s to ensure that the shifts disappear as required. We use 11 octal digits because
 * this gives us 33 > 32 bits.
 * Octal 3 gives the last two bits of each bin, octal 1 gives the last bit of each bin.
 * 
 * Part 4: 6 bit bins are now easy to make
 *
 * We can now simply add all of the bins in pairs. We can do this by bitshifting everything right by three bits and
 * adding this to the original number. Now each bin is the sum of itself and the bin before it.
 * Finally we bitmask every other bin so that we get groups of 6 bit bins. Note that this has to include the last 3
 * bits, and 07070707070 wouldn't work because the last bin would be lost. Octal 7, of course, is just 111 which
 * masks 3 bit chunks.
 *
 * Part 5: modulo magic
 *
 * Now that we have 6 bit bins, we have sectiona which correspond to multiples of 64. Now, 63 and 64 are coprime.
 * In fact, all consecutive integer pairs are coprime.
 * So we can take a modulus of 63. All powers of 64 are 1 in modulus 63.
 * Analogously, all decimal numbers modulo 9 are the sum of their digits.
 * Another nice propery of the number 63 is that it's more than 32, so we can now get a full count
 */
unsigned int bit_count32(uint32_t u)
{
	uint32_t three_bit_counts;
	unsigned int final_count;
	three_bit_counts = u - ((u >> 1) & 033333333333) - ((u >> 2) & 011111111111);
	final_count = ((three_bit_counts + (three_bit_counts >> 3)) & 030707070707) % 63;
	return final_count;
}

void test32()
{
	for(unsigned int i = 0; i < 32; i++)
	{
		uint32_t test_number = 0;
		for(unsigned int j = 0; j <= i; j++)
		{
			test_number += 1u << j;
		}
		printf("%b contains %d true bits\n", test_number, bit_count32(test_number));
	}
}

void test64()
{
	for(unsigned int i = 0; i < 64; i++)
	{
		uint64_t test_number = 0;
		for(unsigned int j = 0; j <= i; j++)
		{
			test_number += 1ull << j;
		}
		printf("%llb contains %d true bits\n", test_number, bit_count(test_number));
	}
}
