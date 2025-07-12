#ifndef STATS_UI_H
#define STATS_UI_H

#include "../screen.h"
#include "../pet.h"
#include "../ui.h"


#define STATS_UI_HAPPY 0
#define STATS_UI_HUNGRY 1
#define STATS_UI_CARE 2
#define STATS_UI_WEIGHT 3

#define STATS_UI_STEPS 4

typedef struct {
    unsigned char step;
    void* retDraw;
    void* retUpdate;
    bool retDrawPet;
} StatsUI;

StatsUI statsUI;


void draw_level_ui(int stat, int x, int y) {
    int lvl = (stat * 4 / 10 + 5) / 10;
    paint(x, y + 18, 64, 16, 0x0, data_ui_levels_chr + 32 * lvl * 8) ;
}

void return_stats_ui() {
    ui.drawPet = statsUI.retDrawPet;
    ui.updateUI = statsUI.retUpdate;
    ui.drawUI = statsUI.retDraw;
}

void update_stats_ui() {
    if(controller_button() == ButtonA) {
        statsUI.step = (statsUI.step + 1);
        if(statsUI.step >= STATS_UI_STEPS) {
            return_stats_ui();
        }
    }
}

void draw_stats_ui() {
    int x = screen_width() / 2 - 64 / 2;
    int y = screen_height() / 2 - (16 + 16 + 2) / 2;
    if(statsUI.step == STATS_UI_HAPPY) {
        paint(x, y, 56, 16, 0x0, data_ui_text_chr + 28 * 8 * 0);
        draw_level_ui(pet.happy, x, y);
    } else if(statsUI.step == STATS_UI_HUNGRY) {
        paint(x, y, 56, 16, 0x0, data_ui_text_chr + 28 * 8 * 1);
        draw_level_ui(pet.hunger, x, y);
    } else if(statsUI.step == STATS_UI_CARE) {
        paint(x, y, 56, 16, 0x0, data_ui_text_chr + 28 * 8 * 2);
        for(int i = 0; i < pet.caremissCount; i++) {
            paint(x + (16 + 2) * i, y + 18, 16, 16, 0x0, data_skull_chr);
        }
    } else if(statsUI.step == STATS_UI_WEIGHT) {
        paint(x, y + 8, 16, 16, 0x0, data_ui_weight_chr);
        paintInt(pet.weight, x + 32, y + 8, 0x0);
        // TODO: actually write numbers
    }
}

void init_stats_ui() {
    statsUI.retDraw = ui.drawUI;
    statsUI.retUpdate = ui.updateUI;
    statsUI.retDrawPet = ui.drawPet;
    statsUI.step = STATS_UI_HAPPY;

    ui.updateUI = (int)&update_stats_ui;
    ui.drawUI = (int)&draw_stats_ui;
    ui.drawPet = FALSE;
}

#endif 