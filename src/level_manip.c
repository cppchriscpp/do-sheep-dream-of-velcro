#include "src/globals.h"
#include "lib/neslib.h"
#pragma rodataseg ("ROM_01")
#pragma codeseg ("ROM_01")

static unsigned int scratchInt;

void banked_draw_level() {
	vram_inc(0);
	vram_adr(NAMETABLE_A);

	// Make sure neslib doesn't try to draw this. Bad things will happen.
	set_vram_update(NULL);
	for (i = 0; i < sizeof(screenBuffer); i++)
		screenBuffer[i] = 0;
	j = -1; 
	// And unpack into vram
	for (i = 0; i != 192; ++i) {
		
		scratch = (currentLevel[i] & 0x3f); // Skip top 2 bytes. That's our palette bytes.
		scratchInt = ((i / 16) << 6) + ((i % 16) << 1);

		scratch = (((scratch >> 3)) << 5) + ((scratch % 8) << 1);

		vram_adr(NAMETABLE_A+scratchInt);
		vram_put(scratch);
		vram_put(scratch+1);
		vram_adr(NAMETABLE_A + scratchInt + 32);
		vram_put(scratch + 16);
		vram_put(scratch + 17);

		scratch = (currentLevel[i] & 0xc0); // Top 2 bits are the palette bytes.

		if (i % 32 == 16) 
			j -= 8;
		if (i % 2 == 0) 
			j++;
		if (i % 2 == 0) {
			// Even/left
			if ((i / 16) % 2 == 0) {
				// top
				scratch >>= 6;
			} else {
				//bottom
				scratch >>= 2;
			}
		} else {
			// Odd/right
			if ((i / 16) % 2 == 0) {
				// Top
				scratch >>= 4;
			} else {
				// Bottom 
				scratch >>= 0;
			}
		}
		screenBuffer[j] += scratch;
		vram_adr(NAMETABLE_A + scratchInt>>2);
	}
	vram_adr(NAMETABLE_A + 0x3c0);
	vram_write(screenBuffer, 0x30);
}