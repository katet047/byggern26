#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
#include "can.h"

/* Game messages between the nodes. Must match node2/game.h.
 *   CAN_ID_LIVES       Node 2 -> Node 1, DLC 1: byte 0 = lives left, 0 = game over
 *   CAN_ID_GAME_START  Node 1 -> Node 2, DLC 0: new game, all lives back
 */
#define CAN_ID_LIVES       0x20
#define CAN_ID_GAME_START  0x30

#define GAME_LIVES         3    /* a blocked IR beam (ball lost) costs one life */

void    game_start(void);   /* menu action: give Node 2 all lives back and show the game screen */
void    game_exit(void);    /* leave the game screen (the caller redraws the menu) */
uint8_t game_active(void);  /* 1 while the game screen is shown, also after game over */
uint8_t game_playing(void); /* 1 while the game screen is shown and lives are left */
void    game_draw(void);    /* draw the whole game screen on the OLED */

/* Call every 10 ms tick: sends a pending start command when the TX buffer is free */
void    game_update(void);

/* Call for every received CAN message: picks up the lives left from Node 2 */
void    game_handle_can(const can_message *msg);

#endif
