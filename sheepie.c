#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#include "src/globals.h"
#include "src/level_manip.h"
#include "src/movement.h"
#include "src/sprites.h"
#include "graphics/title_rle.h"
#include "graphics/credits_rle.h"
#include "graphics/pause_rle.h"
#include "graphics/halp_rle.h"
#include "graphics/sheared_rle.h"
#include "graphics/complete_rle.h"
#include "levels/processed/lvl1_tiles.h"

// Suggestion: Define smart names for your banks and use defines like this. 
// This is just to make a clear example, and I didn't want to suggest using bank #s directly.

// Ditto, same advice here.
#define CHR_BANK_0 0
#define CHR_BANK_1 1

#define PRG_LEVELMANIP 1
#define PRG_MOVEMENT 1
#define PRG_FIRST_LEVEL 2
#define PRG_SPRITES 1

#define SONG_TITLE 0
#define SONG_GAME_1 1
#define SONG_GAME_END 4

#define SFX_BOOM 0
#define SFX_SUCCESS 1
#define SFX_BOOP_DOWN 3
#define SFX_BOOP_UP 2
#define SFX_MENU_BOOP 4

// TODO: Why on earth is this necessary?
#pragma bssseg (push,"BSS")
#pragma dataseg(push,"BSS")
unsigned char currentLevel[256];

char extendedSpriteData[56];


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

unsigned char magnetX, magnetY, magnetPos, magnetPosAbs, magnetId, magnetScratch, currentSpriteId;
unsigned char sheepXlo, sheepYlo, sheepXRlo, sheepYBlo, sheepRotation;
int sheepX, sheepY, sheepXnext, sheepYnext, magnetXhi, magnetYhi;
int sheepXVel, sheepYVel;
unsigned char touchingVelcro;
unsigned char scratch, scratch2;
unsigned char tempX, tempY, tempPosition, tempSpriteId;
unsigned char tempX2, tempY2;
unsigned char sheepVelocityLock;
unsigned char prettyLevel, prettyLives;
unsigned int scratchInt;
char magnetSpeed;
unsigned char magnetDirection, magnetDirectionLock;

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
	vram_fill(0, 0x03c0);
	// Set the cleared palette to be red by default, to make hud simpler.
	vram_fill(0xaa, 0x40);
}

void write_screen_buffer(unsigned char x, unsigned char y, char* data) {
	screenBuffer[0] = MSB(NTADR_A(x, y)) | NT_UPD_HORZ;
	screenBuffer[1] = LSB(NTADR_A(x, y));
	screenBuffer[2] = 20u;
	for (i = 0; data[i] != '\0'; ++i) 
		screenBuffer[i+3u] = data[i]-0x20;
	for (; i < 23; ++i)
		screenBuffer[i+3u] = ' '-0x20;
	screenBuffer[23] = NT_UPD_EOF;
	set_vram_update(screenBuffer);
}

void show_title() {
	music_play(SONG_TITLE);
	music_pause(0);


	// Show a message to the user.
	pal_bg(game_palette);
	pal_spr(sprite_palette);
	vram_adr(NAMETABLE_A);
	vram_unrle(title_rle);

	// Also show some cool build info because we can.
	// Let's hide this for the final build! (It shows up in the ending anyway)
	// put_str(NTADR_A(2,25), "Built: " BUILD_DATE);
	// put_str(NTADR_A(2,26), "Build #" BUILD_NUMBER_STR " (" GIT_COMMIT_ID_SHORT " - " GIT_BRANCH ")");
	// put_str(NTADR_A(2,27), "Commit counter: " COMMIT_COUNT_STR);
	ppu_on_all();

	sheepX = 1280;
	sheepY = 1880;
	sheepXlo = 80;
	sheepYlo = 136;
	magnetX = 0xf0;
	magnetY = 0xf0;
	// We have a copy of this room at 0x62 in the map... use it to load up a collision table.
	set_prg_bank(PRG_FIRST_LEVEL);
	memcpy(currentLevel, lvl1 + (62 << 8), 256);



	while (!(pad_trigger(0) & PAD_A+PAD_START)) {
		if (!(FRAME_COUNTER & 32)) {
			tempX = rand8();
			tempY = rand8();
		}
		magnetX = tempX;
		magnetY = tempY;
		do_sheep_movement();
		if (abs(sheepXVel) > 1 || abs(sheepYVel) > 1) {
			sheepRotation = ((FRAME_COUNTER >> 2) & 0xfe) % 16;
		}

		magnetX = 0xf0;
		magnetY = 0xf0;
		oam_hide_rest(FIRST_ENEMY_SPRITE_ID);
		draw_sprites();
		ppu_wait_nmi();
	}
	oam_hide_rest(0); // BYE SPRITES
	sfx_play(SFX_BOOP_UP, 0);
	animate_fadeout(5);

	ppu_off();
	vram_adr(NAMETABLE_A);
	vram_unrle(halp_rle);
	ppu_on_all();
	animate_fadein(5);
	while (!(pad_trigger(0) & PAD_A+PAD_START)) {
		ppu_wait_nmi();
	}
	sfx_play(SFX_BOOP_UP, 0);
	

	animate_fadeout(5);
	music_play(SONG_GAME_1);
	music_pause(1);

}

void show_level_finished() {
	sfx_play(SFX_SUCCESS, 0);
	animate_fadeout(5);
	ppu_off();

	clear_screen();
	oam_hide_rest(0);
	vram_adr(NAMETABLE_A);
	vram_unrle(complete_rle);
	ppu_on_bg();
	animate_fadein(5);
	delay_or_button(120);
	animate_fadeout(5);
	ppu_on_all();
}

void show_game_finished() {
	music_pause(1);
	animate_fadeout(5);
	set_vram_update(NULL);
	ppu_off();
	music_play(SONG_GAME_END);
	music_pause(0);

	// Show a message to the user.
	pal_bg(game_palette);
	pal_spr(sprite_palette);
	clear_screen();
	vram_adr(NAMETABLE_A);
	vram_unrle(credits_rle);
	oam_hide_rest(0);

	// Also show some cool build info because we can.
	put_str(NTADR_A(3,25), "Built " BUILD_DATE);
	put_str(NTADR_A(3,24), "Build #" BUILD_NUMBER_STR " (" GIT_COMMIT_ID_SHORT "-" GIT_BRANCH ")");
	put_str(NTADR_A(3,23), "Commit counter: " COMMIT_COUNT_STR);

	vram_adr(NTADR_A(18, 16));
	vram_put(('0' + (prettyLives >> 4)) - 0x20);
	vram_put(('0' + (prettyLives % 16)) - 0x20);

	ppu_on_all();

	magnetX = 4<<3;
	magnetY = 2<<3;
	sheepXlo = 26<<3;
	sheepYlo = 2<<3;

	animate_fadein(5);
	while (!(pad_trigger(0) & PAD_START)) {

		magnetId = ((FRAME_COUNTER >> 2) & 0xfe) % 16;
		sheepRotation = (16 - magnetId) % 16;
		magnetX = 4<<3;
		magnetY = 2<<3;
		sheepXlo = 26<<3;
		sheepYlo = 2<<3;
		currentSpriteId = oam_spr(magnetX, magnetY, SHEEP_SPRITE_TILE+magnetId, 0, MAGNET_SPRITE_ID);
		currentSpriteId = oam_spr(magnetX+8, magnetY, SHEEP_SPRITE_TILE+magnetId+1, 0, currentSpriteId);
		currentSpriteId = oam_spr(magnetX, magnetY+8, SHEEP_SPRITE_TILE+magnetId+16, 0, currentSpriteId);
		currentSpriteId = oam_spr(magnetX+8, magnetY+8, SHEEP_SPRITE_TILE+magnetId+17, 0, currentSpriteId);
		currentSpriteId = oam_spr(sheepXlo, sheepYlo, SHEEP_SPRITE_TILE+sheepRotation, 0, SHEEP_SPRITE_ID);
		currentSpriteId = oam_spr(sheepXlo+8, sheepYlo, SHEEP_SPRITE_TILE+sheepRotation+1, 0, currentSpriteId);
		currentSpriteId = oam_spr(sheepXlo, sheepYlo+8, SHEEP_SPRITE_TILE+sheepRotation+16, 0, currentSpriteId);
		currentSpriteId = oam_spr(sheepXlo+8, sheepYlo+8, SHEEP_SPRITE_TILE+sheepRotation+17, 0, currentSpriteId);

		magnetX = 2<<3;
		magnetY = 10<<3;
		sheepXlo = 28<<3;
		sheepYlo = 10<<3;
		currentSpriteId = oam_spr(magnetX, magnetY, SHEEP_SPRITE_TILE+magnetId, 0, currentSpriteId);
		currentSpriteId = oam_spr(magnetX+8, magnetY, SHEEP_SPRITE_TILE+magnetId+1, 0, currentSpriteId);
		currentSpriteId = oam_spr(magnetX, magnetY+8, SHEEP_SPRITE_TILE+magnetId+16, 0, currentSpriteId);
		currentSpriteId = oam_spr(magnetX+8, magnetY+8, SHEEP_SPRITE_TILE+magnetId+17, 0, currentSpriteId);
		currentSpriteId = oam_spr(sheepXlo, sheepYlo, SHEEP_SPRITE_TILE+sheepRotation, 0, currentSpriteId);
		currentSpriteId = oam_spr(sheepXlo+8, sheepYlo, SHEEP_SPRITE_TILE+sheepRotation+1, 0, currentSpriteId);
		currentSpriteId = oam_spr(sheepXlo, sheepYlo+8, SHEEP_SPRITE_TILE+sheepRotation+16, 0, currentSpriteId);
		currentSpriteId = oam_spr(sheepXlo+8, sheepYlo+8, SHEEP_SPRITE_TILE+sheepRotation+17, 0, currentSpriteId);



		ppu_wait_nmi();
	}
	animate_fadeout(5);
	ppu_off();
	clear_screen();
	animate_fadein(1);
}


void show_level_failed() {
	sfx_play(SFX_BOOM, 0);
	// Reset song
	music_play(SONG_GAME_1);
	music_pause(1);
	animate_fadeout(5);
	ppu_off();

	clear_screen();
	oam_hide_rest(0);
	vram_adr(NAMETABLE_A);
	vram_unrle(sheared_rle);
	ppu_on_bg();
	animate_fadein(5);
	delay_or_button(120);
	animate_fadeout(5);
	ppu_on_all();
}


void show_level() {
	magnetPos = (128-32);
	magnetPosAbs = 1;
	magnetSpeed = MAGNET_SPEED_1;
	magnetDirection = 0;
	sheepXVel = 0;
	sheepYVel = 0;
	sheepVelocityLock = 0;

	// Load up the data into currentLevel
	set_prg_bank(PRG_FIRST_LEVEL + currentLevelId);
	// TODO: Take this from a pseudosprite?
	sheepX = (*(char*)(lvl_details+1)) << 4;
	sheepY = *(char*)(lvl_details+2) << 4;

	ppu_off();
	clear_screen();
	pal_bg(game_palette);
	pal_spr(sprite_palette);


	// NOTE: Yes, this says lvl1 - it'll line up with whatever we get though.
	memcpy(currentLevel, lvl1 + (playerOverworldPosition << 8), 256);
	vram_adr(NAMETABLE_B);
	vram_unrle(pause_rle);

	set_prg_bank(PRG_LEVELMANIP);
	banked_draw_level();
	banked_draw_sprites();

	ppu_on_all();
	animate_fadein(5);
	music_pause(0);
	banked_draw_hold_a();

}

void do_magnet_movement() {

	#if MAGNET_CONTROL 
		if (currentPadState & PAD_LEFT) {
			magnetPos-=2;

			if ((magnetPos & 254) == 254) {
				magnetPos = 128;
				magnetPosAbs = !magnetPosAbs;
			}


		} else if (currentPadState & PAD_RIGHT) {
			magnetPos+=2;
			if ((magnetPos & 254) == 254) {
				magnetPos = 0;
				magnetPosAbs = !magnetPosAbs;
			}
		}
	#else 
		if (magnetDirection == 0)
			magnetPos += magnetSpeed;
		else
			magnetPos -= magnetSpeed;

		if (magnetDirection == 0) {
			if (magnetPos > 128) {
				magnetPos = 0;
				magnetPosAbs = !magnetPosAbs;
			}
		} else {
			if (magnetPos >= 240) {
				magnetPos = 128-magnetSpeed;
				magnetPosAbs = !magnetPosAbs;
			}

		}

	#endif

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

void draw_sprites() {
	currentSpriteId = oam_spr(magnetX, magnetY, MAGNET_SPRITE_TILE+magnetId, 2, MAGNET_SPRITE_ID);
	currentSpriteId = oam_spr(magnetX+8, magnetY, MAGNET_SPRITE_TILE+magnetId+1, 2, currentSpriteId);
	currentSpriteId = oam_spr(magnetX, magnetY+8, MAGNET_SPRITE_TILE+magnetId+16, 2, currentSpriteId);
	currentSpriteId = oam_spr(magnetX+8, magnetY+8, MAGNET_SPRITE_TILE+magnetId+17, 2, currentSpriteId);

	currentSpriteId = oam_spr(sheepXlo, sheepYlo, SHEEP_SPRITE_TILE+sheepRotation, 0, SHEEP_SPRITE_ID);
	currentSpriteId = oam_spr(sheepXlo+8, sheepYlo, SHEEP_SPRITE_TILE+sheepRotation+1, 0, currentSpriteId);
	currentSpriteId = oam_spr(sheepXlo, sheepYlo+8, SHEEP_SPRITE_TILE+sheepRotation+16, 0, currentSpriteId);
	currentSpriteId = oam_spr(sheepXlo+8, sheepYlo+8, SHEEP_SPRITE_TILE+sheepRotation+17, 0, currentSpriteId);
}

unsigned char test_collision(unsigned char tileId, unsigned char isPlayer) {
	tileId = tileId & 0x3f;
	switch (tileId) {
		case 57: 
			if (isPlayer)
				touchingVelcro = 1;
			return 0;
		case 24:
		case 25:
		case 31:
		case 58:
			return 1;
		default:
			return 0;
	}
}

void inc_lives() {
	++prettyLives;
	if ((prettyLives & 0x0f) == 10) {
		prettyLives += 6;
	}
}

void show_pause() {
	sfx_play(SFX_BOOP_UP, 0);
	animate_fadeout(5);
	ppu_off();
	ppu_on_bg();
	scroll(255, 0);
	animate_fadein(5);
	scratch = 0;
	while (1) {
		staticPadState = pad_trigger(0); 
		if (staticPadState & (PAD_START | PAD_A)) {
			break;
		}

		if (staticPadState & PAD_DOWN && scratch < 2) {
			++scratch;
			sfx_play(SFX_MENU_BOOP, 1);
		} else if (staticPadState & PAD_UP && scratch > 0) {
			--scratch;
			sfx_play(SFX_MENU_BOOP, 1);
		}

		screenBuffer[0] = MSB(NTADR_B(8, 16));
		screenBuffer[1] = LSB(NTADR_B(8, 16));
		screenBuffer[2] = scratch == 0 ? TILE_ARROW : 0;
		screenBuffer[3] = MSB(NTADR_B(8, 18));
		screenBuffer[4] = LSB(NTADR_B(8, 18));
		screenBuffer[5] = scratch == 1 ? TILE_ARROW : 0;
		screenBuffer[6] = MSB(NTADR_B(8, 20));
		screenBuffer[7] = LSB(NTADR_B(8, 20));
		screenBuffer[8] = scratch == 2 ? TILE_ARROW : 0;
		screenBuffer[9] = NT_UPD_EOF;
		set_vram_update(screenBuffer);
		ppu_wait_nmi();

	}
	set_vram_update(NULL);
	sfx_play(SFX_BOOP_DOWN, 0);
	animate_fadeout(5);
	ppu_off();
	ppu_on_all();
	scroll(0, 0);
	if (staticPadState & PAD_A) {
		if (scratch == 1) {
			inc_lives();
			oam_hide_rest(0);
			gameState = GAME_STATE_START_LEVEL;
			return;
		} else if (scratch == 2) {
			gameState = GAME_STATE_INIT;
			ppu_off();
			clear_screen();
			animate_fadein(1);
			return;
		}
	}
	animate_fadein(5);
	gameState = GAME_STATE_RUNNING;

	
}

// Main entry point for the application.
void main(void) {

	set_mirroring(MIRROR_VERTICAL);
	set_chr_bank_0(CHR_BANK_0);
	set_chr_bank_1(CHR_BANK_0);

	gameState = GAME_STATE_INIT;


	// Now we wait for input from the user, and do dumb things!
	while(1) {
		lastPadState = currentPadState;
		staticPadState = pad_trigger(0);
		currentPadState = pad_state(0);
		if (gameState == GAME_STATE_INIT) {
			playMusic = 0;

			currentLevelId = 0;
			playerOverworldPosition = FIRST_LEVEL;
			prettyLevel = FIRST_LEVEL+1; // Start on level 1, not 0.
			prettyLives = 0;

			show_title();
			gameState = GAME_STATE_START_LEVEL;
		} else if (gameState == GAME_STATE_START_LEVEL) {
			show_level();
			gameState = GAME_STATE_RUNNING;
		} else if (gameState == GAME_STATE_LEVEL_COMPLETE) {
			playerOverworldPosition++;
			prettyLevel++;
			if (playerOverworldPosition == NUMBER_OF_LEVELS) {
				gameState = GAME_STATE_WIN;
				continue;
			}
			show_level_finished();
			if ((prettyLevel & 0x0f) == 10) {
				prettyLevel += 6;
			}
			gameState = GAME_STATE_START_LEVEL;
			
		} else if (gameState == GAME_STATE_LEVEL_FAILED) {
			show_level_failed();
			inc_lives();
			gameState = GAME_STATE_START_LEVEL;
		} else if (gameState == GAME_STATE_WIN) { 
			show_game_finished();
			gameState = GAME_STATE_INIT;
		} else if (gameState == GAME_STATE_PAUSE) {
			show_pause();
		} else if (gameState == GAME_STATE_RUNNING) {
			do_magnet_movement();
			do_sheep_movement();
			do_sprite_collision();
			update_sprites();

			// Cheapest animation method ever.
			set_chr_bank_0(CHR_BANK_0 + ((FRAME_COUNTER>>5) & 0x01));
			set_chr_bank_1(CHR_BANK_0 + ((FRAME_COUNTER>>5) & 0x01));


			if (abs(sheepXVel) > 1 || abs(sheepYVel) > 1) {
				sheepRotation = ((FRAME_COUNTER >> 2) & 0xfe) % 16;
			}

			if (staticPadState & PAD_START) {
				gameState = GAME_STATE_PAUSE;
			}

			draw_sprites();
		}
		ppu_wait_nmi();
	}
}

void update_sprites() {
	set_prg_bank(PRG_SPRITES);
	banked_update_sprites();
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

void delay_or_button(unsigned char _delay) {
	for (i = 0; i < _delay; ++i) {
		if (i > 10 && pad_trigger(0) & (PAD_A | PAD_START)) {
			break;
		}
		++i;
		ppu_wait_nmi();
	}
}

void do_sprite_collision() {
	// tempX = sheepX >> 4;
	// tempY = playerY >> 4;
	// sheepXlo sheepYlo
	for (i = 0; i < 12; ++i) {
		if (extendedSpriteData[i<<2] == SPRITE_TYPE_LEVEL_START)
			continue;
		tempX = *(char*)(0x200 + FIRST_ENEMY_SPRITE_ID+3 + (i<<4));
		tempY = *(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4));

		// All sprites are enemies of some sort, so shrink all enemies a little bit... move it 3 over, and treat it as
		// 10 wide vs 16.
		tempX += SPRITE_OFFSET;
		tempY += SPRITE_OFFSET;

		if ((sheepX+SHEEP_LEFT_OFFSET) >> 4 < tempX + SPRITE_WIDTH_MINI && ((sheepX+SHEEP_LEFT_OFFSET+SHEEP_WIDTH) >> 4) > tempX && 
			(sheepY+SHEEP_TOP_OFFSET) >> 4 < tempY + SPRITE_WIDTH_MINI && ((sheepY+SHEEP_TOP_OFFSET+SHEEP_HEIGHT) >> 4) > tempY) {
			// WOMP WOMP
			gameState = GAME_STATE_LEVEL_FAILED;	
		}
	}
}