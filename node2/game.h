// AI USE: This file was written with heavy use of AI (Claude), and then reviewed,
// edited and validated on the hardware by the candidates.

#pragma once

/* Game messages between the nodes. Must match node1/game.h.
 *   CAN_ID_MISSES      Node 2 -> Node 1, DLC 2: misses since game start (ball lost into the
 *                                               IR beam), uint16_t, byte 0 = low byte.
 *                                               Node 1 turns this into lives for the game mode.
 *   CAN_ID_GAME_START  Node 1 -> Node 2, DLC 0: new game, misses back to 0
 */
#define CAN_ID_MISSES      0x20
#define CAN_ID_GAME_START  0x30
