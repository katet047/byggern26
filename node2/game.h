#pragma once

/* Game messages between the nodes. Must match node1/game.h.
 *   CAN_ID_SCORE       Node 2 -> Node 1, DLC 2: goals counted, uint16_t, byte 0 = low byte
 *   CAN_ID_GAME_START  Node 1 -> Node 2, DLC 0: new game, reset the score
 */
#define CAN_ID_SCORE       0x20
#define CAN_ID_GAME_START  0x30
