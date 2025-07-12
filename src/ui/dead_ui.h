#ifndef DEAD_UI
#define DEAD_UI

#include "lib/varvara.h"
#include "../ui.h"
#include "../screen.h"



#define DEAD_UI_LENGTH 2
#define DEAD_UI_GAP 8
#define DEAD_HOVER 0x5


typedef struct {
    int index;
    // int is in fact a function pointer 
    // but no diff in uxn
    int callbacks[DEAD_UI_LENGTH];
} DeadUI;

DeadUI deadUI;


void update_dead_ui() {
    if(controller_button() == ButtonLeft) {
        int value = (deadUI.index - 1);
        if(value < 0) {
            value = DEAD_UI_LENGTH - 1;
        }
        deadUI.index = value;
    }
    else if(controller_button() == ButtonRight) {
        int value = (deadUI.index + 1) % DEAD_UI_LENGTH;
        deadUI.index = value;
    } else if(controller_button() == ButtonA) {
        callback((void*)deadUI.callbacks[deadUI.index]);
    }
}

void draw_dead_ui() {
    int x = screen_width() / 2 - DEAD_UI_LENGTH * 16 / 2;
    unsigned char* icons[DEAD_UI_LENGTH] = { data_ui_stats_chr, data_skull_chr };
    // draw main UI button
    for(int i = 0; i < DEAD_UI_LENGTH; i++) {
        paint(x + (16 + DEAD_UI_GAP) * i, UI_POS, 16, 16, deadUI.index == i ? DEAD_HOVER : 0x00, icons[i]);
    }
}

void init_dead_ui(void* onstats, void* onreset) {
    deadUI.callbacks[0] = (int)onstats;
    deadUI.callbacks[1] = (int)onreset;

    ui.updateUI = (int)&update_dead_ui;
    ui.drawUI = (int)&draw_dead_ui;
    ui.drawPet = TRUE;
}

#endif