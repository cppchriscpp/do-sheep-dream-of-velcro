#include "src/globals.h"
#include "lib/neslib.h"
#include "src/movement.h"
#pragma rodataseg ("ROM_01")
#pragma codeseg ("ROM_01")

void do_banked_movement() {
	// Magnet is lo-res by default... bring it on up. (Just makes calculations easier...)
	magnetXhi = magnetX << 4;
	magnetYhi = magnetY << 4; 
	
	// TODO: This should be velocity based, and be very slippery if the sheep isn't velcroed down
	// (I sound like a complete crazy person, don't I? No sheep were harmed in the making of this logic.)
	// NOTE: Velcro OVERRIDES the speed lock.
	if (touchingVelcro) {
		if (currentPadState & PAD_A) {
			if (sheepXVel > 2) {
				sheepXVel -= 2;
			} else if (sheepXVel > 0) {
				sheepXVel = 0;
			} else if (sheepXVel < -2) {
				sheepXVel += 2;
			} else {
				sheepXVel = 0;
			}

			if (sheepYVel > 2) {
				sheepYVel -= 2;
			} else if (sheepYVel > 0) {
				sheepYVel = 0;
			} else if (sheepYVel < -2) {
				sheepYVel += 2;
			} else {
				sheepYVel = 0;
			}
		} else {
			if (sheepXVel > 8) {
				sheepXVel -= 2;
			} else if (sheepXVel > 0) {
				// Do nothing...
			} else if (sheepXVel < -8) {
				sheepXVel += 2;
			}

			if (sheepYVel > 8) {
				sheepYVel -= 2;
			} else if (sheepYVel > 0) {
				// Do nothing...
			} else if (sheepYVel < -8) {
				sheepYVel += 2;
			}
		}

	}

	if (!sheepVelocityLock && !(touchingVelcro && currentPadState & PAD_A)) {
		if (sheepX > magnetXhi) {
			// TODO: Erm, defines maybe?
			sheepXVel -= 1;
		} else if (sheepX < magnetXhi) {
			sheepXVel += 1;
		} else {
			sheepXVel = 0;
		}

		if (sheepY > magnetYhi) {
			// TODO: Erm, defines maybe?
			sheepYVel -= 1;
		} else if (sheepY < magnetYhi) {
			sheepYVel += 1;
		}
	}

	if (!sheepVelocityLock) {
		if (sheepYVel > 0 && sheepYVel > PLAYER_MAX_VELOCITY) {
			sheepYVel = PLAYER_MAX_VELOCITY;
		} else if (sheepYVel < 0 && sheepYVel < 0-PLAYER_MAX_VELOCITY) {
			sheepYVel = 0-PLAYER_MAX_VELOCITY;
		}

		if (sheepXVel > 0 && sheepXVel > PLAYER_MAX_VELOCITY) {
			sheepXVel = PLAYER_MAX_VELOCITY;
		} else if (sheepXVel < 0 && sheepXVel < 0-PLAYER_MAX_VELOCITY) {
			sheepXVel = 0-PLAYER_MAX_VELOCITY;
		}
	}

	sheepXnext = sheepX + sheepXVel;
	sheepYnext = sheepY + sheepYVel;

	if (sheepXnext < 0) {
		sheepXnext = 2;
	}
	if (sheepXnext > (240<<4)) {
		sheepXnext = 240<<4;
	}
	if (sheepYnext < 0) {
		sheepYnext = 2;
	}
	if (sheepYnext > (220<<4)) {
		sheepYnext = 220 << 4;
	}

	if (sheepVelocityLock)
		--sheepVelocityLock;
	
	touchingVelcro = 0;
	// WEIRD STUFF NOTE, using sheepXlo&sheepYlo here to test, then setting it for real a little later
	sheepXlo = (sheepX+SHEEP_LEFT_OFFSET) >> 8;
	sheepYlo = (sheepYnext+SHEEP_TOP_OFFSET) >> 8;
	sheepXRlo = (sheepX+SHEEP_LEFT_OFFSET+SHEEP_WIDTH) >> 8;
	sheepYBlo = (sheepYnext+SHEEP_TOP_OFFSET+SHEEP_HEIGHT) >> 8;
	if (sheepYVel < 0) {
		// Going Y-, or up. So... test TL and TR
		if (test_collision(currentLevel[sheepXlo+(sheepYlo<<4)], 1) || test_collision(currentLevel[sheepXRlo+(sheepYlo<<4)], 1)) {
			sheepYVel = 0;
		}
		// TODO?: Could set direction here
	} else if (sheepYVel > 0) {
		// Oh, BL/BR
		if (test_collision(currentLevel[sheepXlo+(sheepYBlo<<4)], 1) || test_collision(currentLevel[sheepXRlo+(sheepYBlo<<4)], 1)) {
			sheepYVel = 0;
		}
	}

	sheepXlo = (sheepXnext+SHEEP_LEFT_OFFSET) >> 8;
	sheepYlo = (sheepY+SHEEP_TOP_OFFSET) >> 8;
	sheepXRlo = (sheepXnext+SHEEP_LEFT_OFFSET + SHEEP_WIDTH) >> 8;
	sheepYBlo = (sheepY + SHEEP_TOP_OFFSET + SHEEP_HEIGHT) >> 8;
	if (sheepXVel < 0) {
		// Going X-, or left, so... test TL and BL
		if (test_collision(currentLevel[sheepXlo+(sheepYlo<<4)], 1) || test_collision(currentLevel[sheepXlo+(sheepYBlo<<4)], 1)) {
			sheepXVel = 0;
		}
	} else if (sheepXVel > 0) {
		// Going X+, or right, so... test TR and BR
		if (test_collision(currentLevel[sheepXRlo+(sheepYlo<<4)], 1) || test_collision(currentLevel[sheepXRlo+(sheepYBlo<<4)], 1)) {
			sheepXVel = 0;
		}
	}


	sheepX += sheepXVel;
	sheepY += sheepYVel;

	// Test, are you within one block entirely?
	sheepXlo = (sheepX+SHEEP_LEFT_OFFSET) >> 8;
	sheepYlo = (sheepY+SHEEP_TOP_OFFSET) >> 8;
	sheepXRlo = (sheepX+SHEEP_LEFT_OFFSET + SHEEP_WIDTH) >> 8;
	sheepYBlo = (sheepY + SHEEP_TOP_OFFSET + SHEEP_HEIGHT) >> 8;
	scratch = currentLevel[sheepXlo+(sheepYlo<<4)];

	if (scratch == currentLevel[sheepXRlo+(sheepYlo<<4)] && scratch == currentLevel[sheepXlo+(sheepYBlo<<4)] && scratch == currentLevel[sheepXRlo+(sheepYBlo<<4)]) {
		scratch &= 0x3f;
		if (scratch == TILE_END_OF_LEVEL) {
			gameState = GAME_STATE_LEVEL_COMPLETE;
		} else if (scratch == TILE_HOLE) {
			gameState = GAME_STATE_LEVEL_FAILED;
		} else if (scratch == TILE_BOOST_R) {
			sheepXVel = BOOST_SPEED; // VROOM
			sheepVelocityLock = BOOST_LOCK_TIME;
		} else if (scratch == TILE_BOOST_L) {
			sheepXVel = -BOOST_SPEED; // VROOM
			sheepVelocityLock = BOOST_LOCK_TIME;
		} else if (scratch == TILE_BOOST_D) {
			sheepYVel = BOOST_SPEED; // VROOM
			sheepVelocityLock = BOOST_LOCK_TIME;
		} else if (scratch == TILE_BOOST_U) {
			sheepYVel = -BOOST_SPEED; // VROOM
			sheepVelocityLock = BOOST_LOCK_TIME;
		} else if (scratch == TILE_DIRECTION_SWAP && !magnetDirectionLock) {
			magnetDirection = !magnetDirection;
			magnetDirectionLock = MAGNET_DIRECTION_LOCK;
		} else if (scratch == TILE_SPEED_1) {
			magnetSpeed = (magnetSpeed > 0 ? MAGNET_SPEED_1 : -MAGNET_SPEED_1);
		} else if (scratch == TILE_SPEED_2) {
			magnetSpeed = (magnetSpeed > 0 ? MAGNET_SPEED_2 : -MAGNET_SPEED_2);
		} else if (scratch == TILE_SPEED_3) {
			magnetSpeed = (magnetSpeed > 0 ? MAGNET_SPEED_3 : -MAGNET_SPEED_3);
		}
		
	}

	if (magnetDirectionLock > 0)
		--magnetDirectionLock;


	sheepXlo = sheepX >> 4;
	sheepYlo = sheepY >> 4;

}