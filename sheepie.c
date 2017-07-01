#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#include "src/globals.h"

// Suggestion: Define smart names for your banks and use defines like this. 
// This is just to make a clear example, and I didn't want to suggest using bank #s directly.

// Ditto, same advice here.
#define CHR_BANK_0 0
#define CHR_BANK_1 1

#define SONG_TITLE 0

// FIXME: Put me in a freakin bank, ya fool
const unsigned char sine[256] = {
  0x80, 0x83, 0x86, 0x89, 0x8C, 0x90, 0x93, 0x96,
  0x99, 0x9C, 0x9F, 0xA2, 0xA5, 0xA8, 0xAB, 0xAE,
  0xB1, 0xB3, 0xB6, 0xB9, 0xBC, 0xBF, 0xC1, 0xC4,
  0xC7, 0xC9, 0xCC, 0xCE, 0xD1, 0xD3, 0xD5, 0xD8,
  0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8,
  0xEA, 0xEB, 0xED, 0xEF, 0xF0, 0xF1, 0xF3, 0xF4,
  0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFA, 0xFB, 0xFC,
  0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFD,
  0xFD, 0xFC, 0xFB, 0xFA, 0xFA, 0xF9, 0xF8, 0xF6,
  0xF5, 0xF4, 0xF3, 0xF1, 0xF0, 0xEF, 0xED, 0xEB,
  0xEA, 0xE8, 0xE6, 0xE4, 0xE2, 0xE0, 0xDE, 0xDC,
  0xDA, 0xD8, 0xD5, 0xD3, 0xD1, 0xCE, 0xCC, 0xC9,
  0xC7, 0xC4, 0xC1, 0xBF, 0xBC, 0xB9, 0xB6, 0xB3,
  0xB1, 0xAE, 0xAB, 0xA8, 0xA5, 0xA2, 0x9F, 0x9C,
  0x99, 0x96, 0x93, 0x90, 0x8C, 0x89, 0x86, 0x83,
  0x80, 0x7D, 0x7A, 0x77, 0x74, 0x70, 0x6D, 0x6A,
  0x67, 0x64, 0x61, 0x5E, 0x5B, 0x58, 0x55, 0x52,
  0x4F, 0x4D, 0x4A, 0x47, 0x44, 0x41, 0x3F, 0x3C,
  0x39, 0x37, 0x34, 0x32, 0x2F, 0x2D, 0x2B, 0x28,
  0x26, 0x24, 0x22, 0x20, 0x1E, 0x1C, 0x1A, 0x18,
  0x16, 0x15, 0x13, 0x11, 0x10, 0x0F, 0x0D, 0x0C,
  0x0B, 0x0A, 0x08, 0x07, 0x06, 0x06, 0x05, 0x04,
  0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03,
  0x03, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x0A,
  0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x13, 0x15,
  0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24,
  0x26, 0x28, 0x2B, 0x2D, 0x2F, 0x32, 0x34, 0x37,
  0x39, 0x3C, 0x3F, 0x41, 0x44, 0x47, 0x4A, 0x4D,
  0x4F, 0x52, 0x55, 0x58, 0x5B, 0x5E, 0x61, 0x64,
  0x67, 0x6A, 0x6D, 0x70, 0x74, 0x77, 0x7A, 0x7D
};

// Globals! Defined as externs in src/globals.h
#pragma bssseg (push,"ZEROPAGE")
#pragma dataseg(push,"ZEROPAGE")
unsigned char currentPadState, lastPadState, staticPadState, gameState;
unsigned char i;

unsigned char magnetX, magnetY, magnetPos, magnetPosAbs, magnetId, magnetScratch, sheepXVel, sheepYVel, currentSpriteId;
unsigned char sheepXlo, sheepYlo;
unsigned int sheepX, sheepY, magnetXhi, magnetYhi;

// Local to this file.
static unsigned char playMusic;
static unsigned char chrBank;
static unsigned char mirrorMode;
static char screenBuffer[20];
#pragma bssseg (pop)
#pragma dataseg(pop)

// Put a string on the screen at X/Y coordinates given in adr.
void put_str(unsigned int adr, const char *str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++)-0x20);//-0x20 because ASCII code 0x20 is placed in tile 0 of the CHR
	}
}

void write_screen_buffer(unsigned char x, unsigned char y, char* data) {
	screenBuffer[0] = MSB(NTADR_A(x, y)) | NT_UPD_HORZ;
	screenBuffer[1] = LSB(NTADR_A(x, y));
	screenBuffer[2] = 16u;
	for (i = 0; data[i] != '\0'; ++i) 
		screenBuffer[i+3u] = data[i]-0x20;
	screenBuffer[19] = NT_UPD_EOF;
	set_vram_update(screenBuffer);
}

void show_title() {
	// TODO: Load palette file
	pal_col(1,0x19);//set dark green color
	pal_col(17,0x19);
	music_play(SONG_TITLE);
	music_pause(0);


	// Show a message to the user.
	put_str(NTADR_A(2,8), "MagnaSheep");
	put_str(NTADR_A(2,20), "Press Start");

	// Also show some cool build info because we can.
	put_str(NTADR_A(2,24), "Built: " BUILD_DATE);
	put_str(NTADR_A(2,25), "Build #" BUILD_NUMBER_STR " (" GIT_COMMIT_ID_SHORT " - " GIT_BRANCH ")");
	put_str(NTADR_A(2,26), "Commit counter: " COMMIT_COUNT_STR);
	ppu_on_all();

	while (!(pad_trigger(0) & PAD_A+PAD_START)) {
		ppu_wait_nmi();
	}
	music_pause(1);

}

void show_level() {
	magnetPos = 0;
	sheepX = 800;
	sheepY = 800;
	ppu_off();
	pal_bg(game_palette);
	pal_spr(game_palette);
	ppu_on_all();
}

void do_magnet_movement() {
	if (currentPadState & PAD_LEFT) {
		magnetPos-=2;

		if ((magnetPos & 254) == 254) {
			magnetPos = 128;
			magnetPosAbs = !magnetPosAbs;
		}


	} else if (currentPadState & PAD_RIGHT) {
		magnetPos+=2;
		if ((magnetPos & 254) == 128) {
			magnetPos = 0;
			magnetPosAbs = !magnetPosAbs;
		}


	}

	if (magnetPosAbs) {
		magnetX = sine[magnetPos+32];
		magnetY = sine[magnetPos+96];
	} else {
		magnetX = 0 - sine[magnetPos+32];
		magnetY = 0 - sine[magnetPos+96];
	}

	if (magnetX > 8) 
		magnetX -= 8;
	else 
		magnetX = 0;

	if (magnetY > 16)
		magnetY -= 16;
	else
		magnetY = 0;

	// You want I should give him the clamps? (  <
	if (magnetY > 216)
		magnetY = 216;
	if (magnetY < 8)
		magnetY = 8;

	if (magnetX > 240)
		magnetX = 240;

	// Rotate that magnet (man it'd be nice if I could draw these rotated 45 degrees...)
	magnetScratch = magnetPos;
	if (magnetScratch > 64 && magnetPosAbs)
		magnetId = 2;
	else if (magnetScratch > 64)
		magnetId = 0;
	else if (magnetPosAbs)
		magnetId = 4;
	else
		magnetId = 6;
}

void do_sheep_movement() {
	// If you're not holding A, the sheep is holding on for dear life!
	if (currentPadState & PAD_A) {
		// Sheepy let go... okay, let's start moving...

		// Magnet is lo-res by default... bring it on up. (Just makes calculations easier...)
		magnetXhi = magnetX << 4;
		magnetYhi = magnetY << 4; 
		
		// TODO: This should be velocity based, and be very slippery if the sheep isn't velcroed down
		// (I sound like a complete crazy person, don't I? No sheep were harmed in the making of this logic.)
		if (sheepX > magnetXhi) {
			sheepX -= 2;
		} else {
			sheepX += 2;
		}

		if (sheepY > magnetYhi) {
			sheepY -= 2;
		} else {
			sheepY += 2;
		}

	}

	sheepXlo = sheepX >> 4;
	sheepYlo = sheepY >> 4;
}

// Main entry point for the application.
void main(void) {

	playMusic = 0;
	set_mirroring(MIRROR_VERTICAL);
	set_chr_bank_0(CHR_BANK_0);
	set_chr_bank_1(CHR_BANK_1);
	gameState = GAME_STATE_INIT;


	// Now we wait for input from the user, and do dumb things!
	while(1) {
		lastPadState = currentPadState;
		staticPadState = pad_trigger(0);
		currentPadState = pad_state(0);
		if (gameState == GAME_STATE_INIT) {
			show_title();
			gameState = GAME_STATE_START_LEVEL;
		} else if (gameState == GAME_STATE_START_LEVEL) {
			show_level();
			gameState = GAME_STATE_RUNNING;
		} else if (gameState == GAME_STATE_RUNNING) {
			do_magnet_movement();
			do_sheep_movement();
			currentSpriteId = oam_spr(magnetX, magnetY, MAGNET_SPRITE_TILE+magnetId, 2, MAGNET_SPRITE_ID);
			currentSpriteId = oam_spr(magnetX+8, magnetY, MAGNET_SPRITE_TILE+magnetId+1, 2, currentSpriteId);
			currentSpriteId = oam_spr(magnetX, magnetY+8, MAGNET_SPRITE_TILE+magnetId+16, 2, currentSpriteId);
			currentSpriteId = oam_spr(magnetX+8, magnetY+8, MAGNET_SPRITE_TILE+magnetId+17, 2, currentSpriteId);

			currentSpriteId = oam_spr(sheepXlo, sheepYlo, SHEEP_SPRITE_TILE, 0, SHEEP_SPRITE_ID);
			currentSpriteId = oam_spr(sheepXlo+8, sheepYlo, SHEEP_SPRITE_TILE+1, 0, currentSpriteId);
			currentSpriteId = oam_spr(sheepXlo, sheepYlo+8, SHEEP_SPRITE_TILE+16, 0, currentSpriteId);
			currentSpriteId = oam_spr(sheepXlo+8, sheepYlo+8, SHEEP_SPRITE_TILE+17, 0, currentSpriteId);


		}
		ppu_wait_nmi();
	}
}
