#pragma once

/* Game messages between the nodes. Must match node1/game.h.
 *   CAN_ID_BALL        Node 2 -> Node 1, DLC 1: byte 0 = BALL_LOST (beam blocked, the player missed)
 *                                               or BALL_IN_PLAY (beam clear again, ball put back)
 *   CAN_ID_GAME_START  Node 1 -> Node 2, DLC 0: new game
 */
#define CAN_ID_BALL        0x21
#define CAN_ID_GAME_START  0x30

#define BALL_IN_PLAY       0
#define BALL_LOST          1
