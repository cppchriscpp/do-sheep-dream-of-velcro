#define SPRITE_TYPE_LEVEL_START 0
#define SPRITE_TYPE_SLIDER_UD 1
#define SPRITE_TYPE_SLIDER_LR 2
#define SPRITE_TYPE_MAGNET_SLIDER 3
#define SPRITE_TYPE_CHASE_SLIDER 4

#define SPRITE_PALETTE_0 0
#define SPRITE_PALETTE_1 1
#define SPRITE_PALETTE_2 2
#define SPRITE_PALETTE_3 3

#define SPRITE_DIRECTION_LEFT 0x24
#define SPRITE_DIRECTION_RIGHT 0x20
#define SPRITE_DIRECTION_UP 4
#define SPRITE_DIRECTION_DOWN 0
#define SPRITE_DIRECTION_UNDEF 0xff

#define SPRITE_SIZE_MASK 0xc0
#define SPRITE_PALETTE_MASK 0x03
#define SPRITE_ANIM_MASK 0x30

#define NESLIB_SPRITE_GONE 0xf0
// Slows things down a lil bit to sane speeds.. every other frame we move.
#define SPRITE_MOVEMENT_SPEED (FRAME_COUNTER & 0x01)
#define SPRITE_SIZE_NORMAL 0

// Eh, heck with it... all sprites are 16x16
#define SPRITE_WIDTH 15
#define SPRITE_HEIGHT 16
#define SPRITE_WIDTH_MINI 10
#define SPRITE_OFFSET 3

void banked_draw_sprites();
void banked_update_sprites();