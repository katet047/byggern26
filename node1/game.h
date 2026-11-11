#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
#include "can.h"

/* Game messages between the nodes. Must match node2/game.h.
 *   CAN_ID_MISSES      Node 2 -> Node 1, DLC 2: misses since game start (ball lost into the
 *                                               IR beam), uint16_t, byte 0 = low byte
 *   CAN_ID_GAME_START  Node 1 -> Node 2, DLC 0: new game, misses back to 0
 */
#define CAN_ID_MISSES      0x20
#define CAN_ID_GAME_START  0x30

#define GAME_LIVES         3    /* lives at the start; each miss costs one */

typedef enum {
    GAME_NORMAL,    /* game over when the lives reach 0 */
    GAME_ENDLESS,   /* lives keep counting down below 0, never game over */
} game_mode;

void    game_start(game_mode mode); /* menu action: reset Node 2 and show the game screen */
void    game_exit(void);    /* leave the game screen (the caller redraws the menu) */
uint8_t game_active(void);  /* 1 while the game screen is shown, also after game over */
uint8_t game_playing(void); /* 1 while the game screen is shown and the game is not over */
void    game_draw(void);    /* draw the whole game screen on the OLED */

/* Call every 10 ms tick: sends a pending start command when the TX buffer is free */
void    game_update(void);

/* Call for every received CAN message: picks up the misses from Node 2 */
void    game_handle_can(const can_message *msg);

#endif
