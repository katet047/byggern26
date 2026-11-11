#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
#include "can.h"

/* Game messages between the nodes. Must match node2/game.h.
 *   CAN_ID_BALL        Node 2 -> Node 1, DLC 1: byte 0 = BALL_LOST (beam blocked, the player missed)
 *                                               or BALL_IN_PLAY (beam clear again, ball put back)
 *   CAN_ID_GAME_START  Node 1 -> Node 2, DLC 0: new game
 */
#define CAN_ID_BALL        0x21
#define CAN_ID_GAME_START  0x30

#define BALL_IN_PLAY       0
#define BALL_LOST          1

#define GAME_LIVES         3    /* misses allowed before game over */

void    game_start(void);   /* menu action: tell Node 2 a game started and show the game screen */
void    game_exit(void);    /* leave the game screen (the caller redraws the menu) */
uint8_t game_active(void);  /* 1 while the game screen is shown, also after game over */
void    game_draw(void);    /* draw the whole game screen on the OLED */

/* Call every 10 ms tick: sends a pending start command and counts the play time */
void    game_update(void);

/* Call for every received CAN message: picks up ball lost / ball in play from Node 2 */
void    game_handle_can(const can_message *msg);

#endif
