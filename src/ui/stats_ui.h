#ifndef STATS_UI_H
#define STATS_UI_H

#include "../screen.h"
#include "../pet.h"
#include "../ui.h"


#define STATS_UI_HAPPY 0
#define STATS_UI_HUNGRY 1
#define STATS_UI_HYGIENE 2

#define STATS_UI_STEPS 3

typedef struct {
    unsigned char step;
} StatsUI;

StatsUI statsUI;


void draw_level_ui(int stat, int x, int y) {
    int lvl = (stat * 4 / 10 + 5) / 10;
    paint(x, y + 18, 64, 16, 0x0, data_ui_levels_chr + 32 * lvl * 8) ;
}

void update_stats_ui() {
    if(controller_button() == ButtonA) {
        statsUI.step = (statsUI.step + 1);
        if(statsUI.step >= STATS_UI_STEPS) {
            goto_main_ui();
        }
    }
}

void draw_stats_ui() {
    if(statsUI.step == STATS_UI_HAPPY) {
        int x = screen_width() / 2 - 64 / 2;
        int y = screen_height() / 2 - (16 + 16 + 2) / 2;
        paint(x, y, 56, 16, 0x0, data_ui_text_chr + 28 * 8 * 0);
        draw_level_ui(pet.happy, x, y);
    } else if(statsUI.step == STATS_UI_HUNGRY) {
        int x = screen_width() / 2 - 64 / 2;
        int y = screen_height() / 2 - (16 + 16 + 2) / 2;
        paint(x, y, 56, 16, 0x0, data_ui_text_chr + 28 * 8 * 1);
        draw_level_ui(pet.hunger, x, y);
    } else if(statsUI.step == STATS_UI_HYGIENE) {
        int x = screen_width() / 2 - 64 / 2;
        int y = screen_height() / 2 - (16 + 16 + 2) / 2;
        paint(x, y, 56, 16, 0x0, data_ui_text_chr + 28 * 8 * 2);
        for(int i = 0; i < pet.caremissCount; i++) {
            paint(x + (16 + 2) * i, y + 18, 16, 16, 0x0, data_skull_chr);
        }
    }
}

void init_stats_ui() {
    statsUI.step = STATS_UI_HAPPY;

    ui.updateUI = (int)&update_stats_ui;
    ui.drawUI = (int)&draw_stats_ui;
    ui.drawPet = FALSE;
}

#endif 