#ifndef CLEAN_UI_H
#define CLEAN_UI_H

#include "../screen.h"
#include "../ui.h"

#include "main_ui.h"

typedef struct {
    AnimatedSprite animation;
    int counter;
    Vector2 position;
} CleanUI;

CleanUI cleanUI;


void start_clean_ui();

void draw_clean_callback_ui() {
    cleanUI.counter -= 1;
    if(cleanUI.counter <= 0) {
        cleanUI.animation.addr = 0;
        goto_main_ui();
    } else {
        start_clean_ui();
    }
}

void start_clean_ui() {
    init_animation(&cleanUI.animation, data_stars_chr, 2, 20, 0, &draw_clean_callback_ui);
    cleanUI.position.x = random(screen_width() / 2 - 16 - 16, screen_width() / 2 + 16 - 16);
    cleanUI.position.y = random(screen_height() / 2 - 16 - 16, screen_height() / 2 + 16 - 16);
}

void draw_clean_ui() {
    if(cleanUI.animation.addr == 0) {
        cleanUI.counter = 3;
        start_clean_ui();
    }
    draw_animation(&cleanUI.animation, cleanUI.position.x, cleanUI.position.y, 0x00);
}


void init_clean_ui() {
    cleanUI.animation.addr = 0;
    cleanUI.counter = 0;
    cleanUI.position.x = 0;
    cleanUI.position.y = 0;

    ui.updateUI = 0;
    ui.drawUI = (int)&draw_clean_ui;
    ui.drawPet = FALSE;
}


#endif 