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


// This file defines globals that can be used all over. You'll want common things in here, as the NES has very, very
// limited ram. 
extern unsigned char currentPadState, lastPadState, staticPadState;
extern unsigned char i;
extern char currentMessage[16];
extern unsigned char gameState;

extern unsigned char magnetX, magnetY, sheepXVel, sheepYVel;
extern unsigned int sheepX, sheepY;

extern unsigned char game_palette[16];