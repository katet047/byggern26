#include "game.h"
#include "OLED.h"

#include <stdio.h>

#define MODE_LINE   1
#define LIVES_LINE  2
#define STATUS_LINE 3

static uint8_t   active = 0;
static uint8_t   start_pending = 0;
static uint8_t   synced = 0;      /* Node 2 has confirmed the reset (sent misses = 0) */
static game_mode mode = GAME_NORMAL;
static uint16_t  misses = 0;

/* Lives can go below 0 in endless mode, so this is signed */
static int16_t lives(void) {
    return (int16_t)GAME_LIVES - (int16_t)misses;
}

static uint8_t over(void) {
    return mode == GAME_NORMAL && lives() <= 0;
}

static void draw_lives(void) {
    oled_clear_line(LIVES_LINE);
    oled_pos(LIVES_LINE, 0);
    fprintf(&mystdout, "Lives: %d", over() ? 0 : lives());

    oled_clear_line(STATUS_LINE);
    if (over()) {
        oled_pos(STATUS_LINE, 0);
        fprintf(&mystdout, "GAME OVER");
    }
}

void game_start(game_mode new_mode) {
    active = 1;
    mode = new_mode;
    misses = 0;
    synced = 0;
    start_pending = 1;   /* sent from game_update(), the TX buffer may be busy right now */
    game_draw();
    printf(mode == GAME_ENDLESS ? "Starting endless game\n" : "Starting game\n");
}

void game_exit(void) {
    active = 0;
}

uint8_t game_active(void) {
    return active;
}

uint8_t game_playing(void) {
    return active && !over();
}

void game_draw(void) {
    oled_clear_screen();
    oled_home();
    fprintf(&mystdout, "PING PONG\n");
    oled_pos(MODE_LINE, 0);
    fprintf(&mystdout, mode == GAME_ENDLESS ? "Endless" : "Normal");
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
    if (msg->id != CAN_ID_MISSES || msg->length < 2 || !active || over()) {
        return;
    }

    uint16_t value = msg->data[0] | ((uint16_t)msg->data[1] << 8);

    /* Ignore everything until Node 2 confirms the reset with misses = 0, so a miss
     * counted just before Node 2 received our start command cannot cost a life */
    if (!synced) {
        synced = (value == 0);
        return;
    }

    if (value == misses) {
        return;
    }

    misses = value;
    draw_lives();
    if (over()) {
        printf("Game over\n");
    } else {
        printf("Ball lost, lives: %d\n", lives());
    }
}
