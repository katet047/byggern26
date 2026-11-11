#pragma once

/* Game messages between the nodes. Must match node1/game.h.
 *   CAN_ID_LIVES       Node 2 -> Node 1, DLC 1: byte 0 = lives left, 0 = game over
 *   CAN_ID_GAME_START  Node 1 -> Node 2, DLC 0: new game, all lives back
 */
#define CAN_ID_LIVES       0x20
#define CAN_ID_GAME_START  0x30

#define GAME_LIVES         3    // a blocked IR beam (ball lost) costs one life
