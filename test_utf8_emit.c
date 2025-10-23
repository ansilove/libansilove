#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "src/cp437_unicode.h"
#include "src/dos_colors.h"

void
test_cp437_to_utf8(void)
{
	uint8_t buf[4];
	int len;

	printf("Testing CP437→UTF-8 conversion:\n");

	uint8_t test_chars[] = {
		0x41,  
		0xB0,  
		0xB1,  
		0xB2,  
		0xC0,  
		0xC3,  
	};

	for (size_t i = 0; i < sizeof(test_chars); i++) {
		len = cp437_to_utf8(test_chars[i], buf);
		printf("  CP437 0x%02X: %d byte(s) UTF-8 =", test_chars[i], len);
		for (int j = 0; j < len; j++)
			printf(" %02X", buf[j]);
		printf("\n");
	}
}

void
test_dos_color_to_ansi256(void)
{
	printf("\nTesting DOS color → ANSI256:\n");

	for (uint8_t color = 0; color < 16; color++) {
		uint8_t ansi = dos_color_to_ansi256(color);
		printf("  DOS color %2d → ANSI256 %3d\n", color, ansi);
	}
}

int
main(void)
{
	printf("=== UTF-8 ANSI Terminal Backend Unit Tests ===\n\n");

	test_cp437_to_utf8();
	test_dos_color_to_ansi256();

	printf("\nAll tests completed.\n");
	return 0;
}
