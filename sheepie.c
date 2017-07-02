#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#include "src/globals.h"
#include "src/level_manip.h"
#include "src/movement.h"
#include "levels/processed/lvl1_tiles.h"

// Suggestion: Define smart names for your banks and use defines like this. 
// This is just to make a clear example, and I didn't want to suggest using bank #s directly.

// Ditto, same advice here.
#define CHR_BANK_0 0
#define CHR_BANK_1 1

#define PRG_LEVELMANIP 1
#define PRG_MOVEMENT 1
#define PRG_FIRST_LEVEL 2

#define SONG_TITLE 0

// TODO: Why on earth is this necessary?
#pragma bssseg (push,"BSS")
#pragma dataseg(push,"BSS")
unsigned char currentLevel[256];


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
unsigned char i, j;
unsigned char currentLevelId, playerOverworldPosition;

unsigned char magnetX, magnetY, magnetPos, magnetPosAbs, magnetId, magnetScratch,currentSpriteId;
unsigned char sheepXlo, sheepYlo, sheepXRlo, sheepYBlo, sheepRotation;
int sheepX, sheepY, sheepXnext, sheepYnext, magnetXhi, magnetYhi;
int sheepXVel, sheepYVel;
unsigned char touchingVelcro;
unsigned char scratch;

// Local to this file.
static unsigned char playMusic;
static unsigned char chrBank;
static unsigned char mirrorMode;
char screenBuffer[0x30];
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

void clear_screen() {
	// Clear the screen to start
	vram_adr(0x2000);
	vram_fill(0, 0x0400);
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
	animate_fadeout(5);
	music_pause(1);

}

void show_level_finished() {
	animate_fadeout(5);
	ppu_off();
	clear_screen();
	put_str(NTADR_A(8, 12), "Level complete!");
	ppu_on_bg();
	animate_fadein(5);
	delay(60);
	animate_fadeout(5);
	ppu_on_all();
}

void show_level_failed() {
	animate_fadeout(5);
	ppu_off();
	clear_screen();
	put_str(NTADR_A(8, 12), "Your sheep");
	put_str(NTADR_A(8, 14), "is sheared!");
	ppu_on_bg();
	animate_fadein(5);
	delay(60);
	animate_fadeout(5);
	ppu_on_all();
}


void show_level() {
	magnetPos = (128-32);
	magnetPosAbs = 1;

	// Load up the data into currentLevel
	set_prg_bank(PRG_FIRST_LEVEL + currentLevelId);
	// TODO: Take this from a pseudosprite?
	sheepX = (*(char*)(lvl_details+1)) << 4;
	sheepY = *(char*)(lvl_details+2) << 4;

	ppu_off();
	clear_screen();
	pal_bg(game_palette);
	pal_spr(game_palette);


	// NOTE: Yes, this says lvl1 - it'll line up with whatever we get though.
	memcpy(currentLevel, lvl1 + (playerOverworldPosition << 8), 256);

	set_prg_bank(PRG_LEVELMANIP);
	banked_draw_level();

	ppu_on_all();
	animate_fadein(5);

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
	set_prg_bank(PRG_MOVEMENT);
	do_banked_movement();
}

unsigned char test_collision(unsigned char tileId) {
	tileId = tileId & 0x3f;
	switch (tileId) {
		case 57: 
			touchingVelcro = 1;
			return 0;
		case 24:
		case 25:
		case 58:
			return 1;
		default:
			return 0;
	}
}

// Main entry point for the application.
void main(void) {

	playMusic = 0;
	set_mirroring(MIRROR_VERTICAL);
	set_chr_bank_0(CHR_BANK_0);
	set_chr_bank_1(CHR_BANK_1);
	gameState = GAME_STATE_INIT;
	currentLevelId = 0;
	playerOverworldPosition = 0;


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
		} else if (gameState == GAME_STATE_LEVEL_COMPLETE) {
			show_level_finished();
			playerOverworldPosition++;
			gameState = GAME_STATE_START_LEVEL;
		} else if (gameState == GAME_STATE_LEVEL_FAILED) {
			show_level_failed();
			gameState = GAME_STATE_START_LEVEL;
		} else if (gameState == GAME_STATE_RUNNING) {
			do_magnet_movement();
			do_sheep_movement();
			if (abs(sheepXVel) > 1 || abs(sheepYVel) > 1) {
				sheepRotation = ((FRAME_COUNTER >> 2) & 0xfe) % 16;
			}
			currentSpriteId = oam_spr(magnetX, magnetY, MAGNET_SPRITE_TILE+magnetId, 2, MAGNET_SPRITE_ID);
			currentSpriteId = oam_spr(magnetX+8, magnetY, MAGNET_SPRITE_TILE+magnetId+1, 2, currentSpriteId);
			currentSpriteId = oam_spr(magnetX, magnetY+8, MAGNET_SPRITE_TILE+magnetId+16, 2, currentSpriteId);
			currentSpriteId = oam_spr(magnetX+8, magnetY+8, MAGNET_SPRITE_TILE+magnetId+17, 2, currentSpriteId);

			currentSpriteId = oam_spr(sheepXlo, sheepYlo, SHEEP_SPRITE_TILE+sheepRotation, 0, SHEEP_SPRITE_ID);
			currentSpriteId = oam_spr(sheepXlo+8, sheepYlo, SHEEP_SPRITE_TILE+sheepRotation+1, 0, currentSpriteId);
			currentSpriteId = oam_spr(sheepXlo, sheepYlo+8, SHEEP_SPRITE_TILE+sheepRotation+16, 0, currentSpriteId);
			currentSpriteId = oam_spr(sheepXlo+8, sheepYlo+8, SHEEP_SPRITE_TILE+sheepRotation+17, 0, currentSpriteId);


		}
		ppu_wait_nmi();
	}
}

void animate_fadeout(unsigned char _delay) {
	pal_bright(3);
	delay(_delay);
	pal_bright(2);
	delay(_delay);;
	pal_bright(1);
	delay(_delay);;
	pal_bright(0);
}

void animate_fadein(unsigned char _delay) {
	pal_bright(1);
	delay(_delay);;
	pal_bright(2);
	delay(_delay);;
	pal_bright(3);
	delay(_delay);;
	pal_bright(4);

}