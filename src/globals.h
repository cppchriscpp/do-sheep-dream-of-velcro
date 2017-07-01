#define GAME_STATE_RUNNING 0
#define GAME_STATE_REDRAW 10
#define GAME_STATE_LEVEL_COMPLETE 220
#define GAME_STATE_START_LEVEL 30
#define GAME_STATE_PAUSE 100
#define GAME_STATE_INIT 255
#define GAME_STATE_GAME_OVER 250
#define GAME_STATE_WIN 251

#define MAGNET_SPRITE_ID 0
#define SHEEP_SPRITE_ID 16
#define FIRST_ENEMY_SPRITE_ID 32

#define MAGNET_SPRITE_TILE 0xe6
#define SHEEP_SPRITE_TILE 0xc0

#define PLAYER_MAX_VELOCITY 32
#define SHEEP_WIDTH 256
#define SHEEP_HEIGHT 256


// This file defines globals that can be used all over. You'll want common things in here, as the NES has very, very
// limited ram. 
extern unsigned char currentPadState, lastPadState, staticPadState;
extern unsigned char i, j;
extern unsigned char gameState;
extern unsigned char currentLevelId, playerOverworldPosition;

extern unsigned char magnetX, magnetY;
extern int sheepXVel, sheepYVel;
extern unsigned char sheepXlo, sheepYlo, sheepXRlo, sheepYBlo;
extern int sheepX, sheepY, sheepXnext, sheepYnext, magnetXhi, magnetYhi;

extern unsigned char game_palette[16];

extern char screenBuffer[0x30];
extern unsigned char currentLevel[256];
extern const char lvl_details[4];
extern unsigned char touchingVelcro;

// Crappy macro to get absolute value in an absolutely disgusting way
// ps: Stolen from missing lands (again)
#define abs(x) (x > 0 ? x : 0-x)

// FIXME: Need a lot more of these
#pragma zpsym ("currentPadState")
#pragma zpsym ("staticPadState")
#pragma zpsym ("lastPadState")
#pragma zpsym ("i")
#pragma zpsym ("j")
#pragma zpsym ("currentLevelId")
#pragma zpsym ("playerOverworldPosition")
#pragma zpsym ("magnetX")
#pragma zpsym ("magnetY")
#pragma zpsym ("magnetXhi")
#pragma zpsym ("magnetYhi")
#pragma zpsym ("screenBuffer")
#pragma zpsym ("sheepX")
#pragma zpsym ("sheepY")
#pragma zpsym ("sheepXnext")
#pragma zpsym ("sheepYnext")
#pragma zpsym ("sheepXlo")
#pragma zpsym ("sheepYlo")
#pragma zpsym ("sheepXRlo")
#pragma zpsym ("sheepYBlo")
#pragma zpsym ("sheepXVel")
#pragma zpsym ("sheepYVel")
// #pragma zpsym ("FRAME_COUNTER")

unsigned char test_collision(unsigned char tileId);