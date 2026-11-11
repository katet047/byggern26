#include "game.h"
#include "OLED.h"

#include <stdio.h>

#define LIVES_LINE   2
#define TIME_LINE    3
#define STATUS_LINE  4
#define TICKS_PER_S  100        /* game_update() runs on the 100 Hz tick */

static uint8_t  active = 0;
static uint8_t  over = 0;
static uint8_t  in_play = 0;    /* ball on the board; the clock only runs while this is 1 */
static uint8_t  lives = 0;
static uint8_t  start_pending = 0;
static uint16_t ticks = 0;      /* 10 ms ticks inside the current second */
static uint16_t seconds = 0;    /* score: total time the ball has been in play */

static void draw_lives(void) {
    oled_clear_line(LIVES_LINE);
    oled_pos(LIVES_LINE, 0);
    fprintf(&mystdout, "Lives: %u", lives);
}

static void draw_time(void) {
    oled_clear_line(TIME_LINE);
    oled_pos(TIME_LINE, 0);
    fprintf(&mystdout, "Score: %u s", seconds);
}

static void draw_status(void) {
    oled_clear_line(STATUS_LINE);
    oled_pos(STATUS_LINE, 0);
    if (over) {
        fprintf(&mystdout, "GAME OVER");
    } else if (in_play) {
        fprintf(&mystdout, "Playing...");
    } else {
        fprintf(&mystdout, "Place ball");
    }
}

void game_start(void) {
    active = 1;
    over = 0;
    in_play = 0;          /* Node 2 sends BALL_IN_PLAY once the beam is clear */
    lives = GAME_LIVES;
    ticks = 0;
    seconds = 0;
    start_pending = 1;    /* sent from game_update(), the TX buffer may be busy right now */
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
    draw_lives();
    draw_time();
    draw_status();
    oled_pos(6, 0);
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

    if (active && in_play && !over) {
        if (++ticks >= TICKS_PER_S) {
            ticks = 0;
            seconds++;
            draw_time();
        }
    }
}

void game_handle_can(const can_message *msg) {
    if (msg->id != CAN_ID_BALL || msg->length < 1 || !active || over) {
        return;
    }

    if (msg->data[0] == BALL_LOST && in_play) {
        in_play = 0;
        lives--;
        if (lives == 0) {
            over = 1;
            printf("Game over, score %u s\n", seconds);
        } else {
            printf("Ball lost, %u lives left\n", lives);
        }
        draw_lives();
        draw_status();
    } else if (msg->data[0] == BALL_IN_PLAY && !in_play) {
        in_play = 1;
        draw_status();
    }
}
