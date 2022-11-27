#include <stdint.h>
#include <stdio.h>

// Prints a 32-bit integer in binary. This may be useful for debugging.
void
print32asbinary(uint32_t n)
{
	// 32: A uint32_t has 32 bits.
	for (int i = 0; i < 32; i++) {
		putchar(((n & (1 << i)) >> i) + '0');
	}
	putchar('\n');
}
