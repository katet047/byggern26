#include "game.h"
#include "OLED.h"

#include <stdio.h>

#define LIVES_LINE  2
#define STATUS_LINE 3

static uint8_t active = 0;
static uint8_t start_pending = 0;
static uint8_t lives = 0;

static void draw_lives(void) {
    oled_clear_line(LIVES_LINE);
    oled_pos(LIVES_LINE, 0);
    fprintf(&mystdout, "Lives: %u", lives);

    oled_clear_line(STATUS_LINE);
    if (lives == 0) {
        oled_pos(STATUS_LINE, 0);
        fprintf(&mystdout, "GAME OVER");
    }
}

void game_start(void) {
    active = 1;
    lives = GAME_LIVES;
    start_pending = 1;   /* sent from game_update(), the TX buffer may be busy right now */
    game_draw();
    printf("Starting game\n");
}

void game_exit(void) {
    active = 0;
}

uint8_t game_active(void) {
    return active;
}

uint8_t game_playing(void) {
    return active && lives > 0;
}

void game_draw(void) {
    oled_clear_screen();
    oled_home();
    fprintf(&mystdout, "PING PONG\n");
    draw_lives();
    oled_pos(5, 0);
    fprintf(&mystdout, "NB: shoot\n");
    fprintf(&mystdout, "NR: menu\n");
}

void game_update(void) {
    if (start_pending) {
        can_message msg = {.id = CAN_ID_GAME_START, .length = 0};
        if (can_send(&msg)) {
            start_pending = 0;
        }
    }
}

void game_handle_can(const can_message *msg) {
    if (msg->id != CAN_ID_LIVES || msg->length < 1) {
        return;
    }

    /* Lives only go down during a game, so the "all lives back" that Node 2
     * sends after our start command is ignored (we already reset to GAME_LIVES). */
    if (!active || msg->data[0] >= lives) {
        return;
    }

    lives = msg->data[0];
    draw_lives();
    if (lives == 0) {
        printf("Game over\n");
    } else {
        printf("Ball lost, %u lives left\n", lives);
    }
}
