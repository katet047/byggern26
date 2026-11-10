#include "game.h"
#include "OLED.h"

#include <stdio.h>

#define SCORE_LINE 2

static uint8_t  active = 0;
static uint8_t  start_pending = 0;
static uint16_t score = 0;

static void draw_score(void) {
    oled_clear_line(SCORE_LINE);
    oled_pos(SCORE_LINE, 0);
    fprintf(&mystdout, "Goals: %u", score);
}

void game_start(void) {
    active = 1;
    score = 0;
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

void game_draw(void) {
    oled_clear_screen();
    oled_home();
    fprintf(&mystdout, "PING PONG\n");
    draw_score();
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
    if (msg->id != CAN_ID_SCORE || msg->length < 2) {
        return;
    }

    score = msg->data[0] | ((uint16_t)msg->data[1] << 8);
    if (active) {
        draw_score();
    }
}
