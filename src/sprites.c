#include "src/globals.h"
#include "lib/neslib.h"
#include "src/sprites.h"

#pragma rodataseg ("ROM_01")
#pragma codeseg ("ROM_01")

static char sprite_directions[12];
static char sprite_direction_time[12];

const unsigned char sprite_data[] = {
	SPRITE_TYPE_LEVEL_START, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_0, 0xee, 1,
	SPRITE_TYPE_SLIDER_UD, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_0, 0x64, 1,
	SPRITE_TYPE_SLIDER_LR, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_0, 0x64, 1,
	SPRITE_TYPE_SLIDER_UD, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_0, 0x64, 2,
	SPRITE_TYPE_SLIDER_LR, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_0, 0x64, 2,
	SPRITE_TYPE_MAGNET_SLIDER, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_1, 0x64, 1,
	SPRITE_TYPE_MAGNET_SLIDER, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_1, 0x64, 2,
	SPRITE_TYPE_CHASE_SLIDER, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_2, 0x64, 1,
	SPRITE_TYPE_CHASE_SLIDER, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_2, 0x64, 2
};

void banked_draw_sprites() {
	for (i = 0; i < 12; ++i) {
		sprite_directions[i] = SPRITE_DIRECTION_UNDEF;
		sprite_direction_time[i] = 0; // Don't think I'm using this, but eh, keep it around.
	}

	for (i = 0; i < 12 && currentLevel[MAP_TILE_SIZE + (i<<1)] != 255; ++i) {

		tempPosition = currentLevel[MAP_TILE_SIZE + (i<<1)];
		tempSpriteId = currentLevel[MAP_TILE_SIZE + (i<<1)+1];
		tempY = (tempPosition >> 4) << 4;
		tempX = ((tempPosition % 16) << 4);

		if (tempSpriteId == SPRITE_TYPE_LEVEL_START) {
			// Position to be unpacked
			sheepY = (tempPosition >> 4) << 8; // Yes, I'm serious. And don't call me shirley.
			sheepX = (tempPosition % 16) << 8;
			sheepYlo = sheepY >> 4;
			sheepXlo = sheepX >> 4;
			// Aaaand, let's not have this count against our sprite limit, because that's silly.
			tempX = tempY = NESLIB_SPRITE_GONE;
		}


		extendedSpriteData[(i<<2)  ] = sprite_data[(tempSpriteId<<2)];
		extendedSpriteData[(i<<2)+1] = sprite_data[(tempSpriteId<<2)+1];
		extendedSpriteData[(i<<2)+2] = sprite_data[(tempSpriteId<<2)+2];
		extendedSpriteData[(i<<2)+3] = sprite_data[(tempSpriteId<<2)+3];

		// Man this logic is ugly... get the palette using some science and such
		scratch = sprite_data[((currentLevel[MAP_TILE_SIZE + (i<<1)+1])<<2)+1] & SPRITE_PALETTE_MASK;
		scratch2 = extendedSpriteData[(i<<2)+2];

		// If we add collectibles,  put the logic here.

		oam_spr(tempX, tempY, scratch2, scratch, FIRST_ENEMY_SPRITE_ID + (i << 4));
		oam_spr(tempX+8, tempY, scratch2+1, scratch, (FIRST_ENEMY_SPRITE_ID + 4) + (i << 4));
		oam_spr(tempX, tempY+8, scratch2+0x10, scratch, (FIRST_ENEMY_SPRITE_ID + 8) + (i << 4));
		currentSpriteId = oam_spr(tempX+8, tempY+8, scratch2+0x11, scratch, (FIRST_ENEMY_SPRITE_ID + 12) + (i << 4));
	}
	oam_hide_rest(currentSpriteId);
}

void banked_update_sprites() {
	// TODO: Um, probably worth doing something here, huh?
	for (i = 0; i < 12; ++i) {
		if (*(unsigned char*)(0x200 + FIRST_ENEMY_SPRITE_ID+(i<<4)) == NESLIB_SPRITE_GONE)
			continue;
		
		// Give ourselves a nice (Albeit ugly as sin) way to get at sprite data
		scratchInt = (0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4));

		tempX = *(unsigned char*)(scratchInt+3);
		tempY = *(unsigned char*)(scratchInt);

		if (extendedSpriteData[i<<2] == SPRITE_TYPE_SLIDER_UD) {
			tempPosition = tempY;
			if (sprite_directions[i] == SPRITE_DIRECTION_UP) {
				tempPosition -= SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				if (test_collision(currentLevel[((tempX)>>4)+((((tempY)>>4))<<4)], 0) || test_collision(currentLevel[((tempX+SPRITE_WIDTH)>>4)+((((tempY)>>4))<<4)], 0)) {
					sprite_directions[i] = SPRITE_DIRECTION_DOWN;
				} else {
					tempY = tempPosition;
				}
			} else if (sprite_directions[i] == SPRITE_DIRECTION_DOWN || sprite_directions[i] == SPRITE_DIRECTION_UNDEF) {
				tempPosition += SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				if (test_collision(currentLevel[((tempX)>>4)+((((tempY+SPRITE_HEIGHT)>>4))<<4)], 0) || test_collision(currentLevel[((tempX+SPRITE_WIDTH)>>4)+((((tempY+SPRITE_HEIGHT)>>4))<<4)], 0)) {
					sprite_directions[i] = SPRITE_DIRECTION_UP;
				} else {
					tempY = tempPosition;
				}
			}
		} else if (extendedSpriteData[i<<2] == SPRITE_TYPE_SLIDER_LR) {
			tempPosition = tempX;
			if (sprite_directions[i] == SPRITE_DIRECTION_LEFT) {
				tempPosition -= SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				if (test_collision(currentLevel[((tempX)>>4)+((((tempY)>>4))<<4)], 0) || test_collision(currentLevel[((tempX)>>4)+((((tempY+SPRITE_HEIGHT)>>4))<<4)], 0)) {
					sprite_directions[i] = SPRITE_DIRECTION_RIGHT;
				} else {
					tempX = tempPosition;
				}
			} else if (sprite_directions[i] == SPRITE_DIRECTION_RIGHT || sprite_directions[i] == SPRITE_DIRECTION_UNDEF) {
				tempPosition += SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				if (test_collision(currentLevel[((tempX+SPRITE_WIDTH)>>4)+((((tempY)>>4))<<4)], 0) || test_collision(currentLevel[((tempX+SPRITE_WIDTH)>>4)+((((tempY+SPRITE_HEIGHT)>>4))<<4)], 0)) {
					sprite_directions[i] = SPRITE_DIRECTION_LEFT;
				} else {
					tempX = tempPosition;
				}
			}

		} else if (extendedSpriteData[i<<2] == SPRITE_TYPE_CHASE_SLIDER || extendedSpriteData[i<<2] == SPRITE_TYPE_MAGNET_SLIDER) {
			// Set tempX2 and tempY2 to desired locations.
			tempX2 = tempX;
			tempY2 = tempY;
			if (extendedSpriteData[i<<2] == SPRITE_TYPE_CHASE_SLIDER) {
				if (tempX > sheepXlo) {
					tempX2 -= SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				} else if (tempX < sheepXlo) {
					tempX2 += SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				}

				if (tempY > sheepYlo) {
					tempY2 -= SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				} else if (tempY < sheepYlo) {
					tempY2 += SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				}
			} else {
				if (tempX > magnetX) {
					tempX2 -= SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				} else if (tempX < magnetX) {
					tempX2 += SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				}

				if (tempY > magnetY) {
					tempY2 -= SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				} else if (tempY < magnetY) {
					tempY2 += SPRITE_MOVEMENT_SPEED*(extendedSpriteData[(i<<2)+3]);
				}
			}

			// Okay, we've got our new and old positions... do we hit anything, or can we just move it?
			if (tempX2 < tempX) {
				if (test_collision(currentLevel[((tempX2)>>4)+((((tempY)>>4))<<4)], 0) || test_collision(currentLevel[((tempX2)>>4)+((((tempY+SPRITE_HEIGHT)>>4))<<4)], 0)) {
					// If you hit, force back to original position.
					tempX2 = tempX;
				}

			} else if (tempX2 > tempX) {
				if (test_collision(currentLevel[((tempX2+SPRITE_WIDTH)>>4)+((((tempY)>>4))<<4)], 0) || test_collision(currentLevel[((tempX2+SPRITE_WIDTH)>>4)+((((tempY+SPRITE_HEIGHT)>>4))<<4)], 0)) {
					tempX2 = tempX;
				}
			}

			if (tempY2 > tempY) {
				if (test_collision(currentLevel[((tempX)>>4)+((((tempY2+SPRITE_HEIGHT)>>4))<<4)], 0) || test_collision(currentLevel[((tempX+SPRITE_WIDTH)>>4)+((((tempY2+SPRITE_HEIGHT)>>4))<<4)], 0)) {
					tempY2 = tempY;
				}
			} else if (tempY2 < tempY) {
				if (test_collision(currentLevel[((tempX)>>4)+((((tempY2)>>4))<<4)], 0) || test_collision(currentLevel[((tempX+SPRITE_WIDTH)>>4)+((((tempY2)>>4))<<4)], 0)) {
					tempY2 = tempY;
				}
			}

			tempX = tempX2;
			tempY = tempY2;
		}

		*(unsigned char*)(scratchInt) = tempY;
		*(unsigned char*)(scratchInt+4) = tempY;
		*(unsigned char*)(scratchInt+8) = tempY+8;
		*(unsigned char*)(scratchInt+12) = tempY+8;

		*(unsigned char*)(scratchInt+3) = tempX;
		*(unsigned char*)(scratchInt+7) = tempX+8;
		*(unsigned char*)(scratchInt+11) = tempX;
		*(unsigned char*)(scratchInt+15) = tempX+8;


	}
}