#define GAME_STATE_RUNNING 0
#define GAME_STATE_REDRAW 10
#define GAME_STATE_LEVEL_COMPLETE 220
#define GAME_STATE_LEVEL_FAILED 120
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
#define SHEEP_WIDTH 176
#define SHEEP_HEIGHT 176
#define SHEEP_LEFT_OFFSET 56
#define SHEEP_TOP_OFFSET 56

#define MAP_TILE_SIZE 192
#define TILE_END_OF_LEVEL 56
#define TILE_HOLE 63
#define TILE_BOOST_R 40
#define TILE_BOOST_L 41
#define TILE_BOOST_U 42
#define TILE_BOOST_D 43
#define TILE_DIRECTION_SWAP 44
#define TILE_SPEED_1 45
#define TILE_SPEED_2 46
#define TILE_SPEED_3 47
#define BOOST_LOCK_TIME 40
#define BOOST_SPEED 60
#define TILE_ARROW 0x3c

#define NUMBER_OF_LEVELS 21
#define FIRST_LEVEL 0
#define MAGNET_DIRECTION_LOCK 60

#define MAGNET_CONTROL 0
#define MAGNET_SPEED_1 1
#define MAGNET_SPEED_2 2
#define MAGNET_SPEED_3 3


// This file defines globals that can be used all over. You'll want common things in here, as the NES has very, very
// limited ram. 
extern unsigned char currentPadState, lastPadState, staticPadState;
extern unsigned char i, j;
extern unsigned char gameState;
extern unsigned char currentLevelId, playerOverworldPosition;
extern unsigned char sheepVelocityLock;

extern unsigned char magnetX, magnetY;
extern int sheepXVel, sheepYVel;
extern unsigned char sheepXlo, sheepYlo, sheepXRlo, sheepYBlo;
extern int sheepX, sheepY, sheepXnext, sheepYnext, magnetXhi, magnetYhi;
extern unsigned char tempX2, tempY2;

extern unsigned char game_palette[16];
extern unsigned char sprite_palette[16];

extern char screenBuffer[0x30];
extern unsigned char currentLevel[256];
extern const char lvl_details[4];
extern unsigned char touchingVelcro, sheepRotation;
extern unsigned char FRAME_COUNTER;
extern unsigned char prettyLevel, prettyLives;
extern char extendedSpriteData[56];

extern unsigned char currentSpriteId;
extern unsigned char scratch, scratch2;
extern unsigned int scratchInt;
extern unsigned char tempX, tempY, tempPosition, tempSpriteId;
extern char magnetSpeed;
extern unsigned char magnetDirection, magnetDirectionLock;

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
#pragma zpsym ("touchingVelcro")
#pragma zpsym ("FRAME_COUNTER")
#pragma zpsym ("sheepRotation")
#pragma zpsym ("gameState")
#pragma zpsym ("scratch")
#pragma zpsym ("scratch2")
#pragma zpsym ("sheepVelocityLock")
#pragma zpsym ("prettyLevel")
#pragma zpsym ("prettyLives")
#pragma zpsym ("currentSpriteId")
#pragma zpsym ("tempX")
#pragma zpsym ("tempY")
#pragma zpsym ("tempX2")
#pragma zpsym ("tempY2")
#pragma zpsym ("tempPosition")
#pragma zpsym ("tempSpriteId")
#pragma zpsym ("scratchInt")
#pragma zpsym ("magnetDirection")
#pragma zpsym ("magnetSpeed")
#pragma zpsym ("magnetDirectionLock")

unsigned char test_collision(unsigned char tileId, unsigned char isPlayer);

void animate_fadeout(unsigned char _delay);
void animate_fadein(unsigned char _delay);
void delay_or_button(unsigned char _delay);
void write_screen_buffer(unsigned char, unsigned char, char *data);
void do_magnet_movement();
void do_sheep_movement();
void draw_sprites();
void put_str(unsigned int adr, const char *str);
void do_pause();
void update_sprites();
void do_sprite_collision();