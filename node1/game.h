#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
#include "can.h"

/* Game messages between the nodes. Must match node2/game.h.
 *   CAN_ID_SCORE       Node 2 -> Node 1, DLC 2: goals counted, uint16_t, byte 0 = low byte
 *   CAN_ID_GAME_START  Node 1 -> Node 2, DLC 0: new game, reset the score
 */
#define CAN_ID_SCORE       0x20
#define CAN_ID_GAME_START  0x30

void    game_start(void);   /* menu action: reset the score on Node 2 and show the game screen */
void    game_exit(void);    /* leave the game screen (the caller redraws the menu) */
uint8_t game_active(void);
void    game_draw(void);    /* draw the whole game screen on the OLED */

/* Call every 10 ms tick: sends a pending start command when the TX buffer is free */
void    game_update(void);

/* Call for every received CAN message: picks up score updates from Node 2 */
void    game_handle_can(const can_message *msg);

#endif
