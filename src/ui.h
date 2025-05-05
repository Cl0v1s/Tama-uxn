#ifndef UI_H
#define UI_H

#include "lib/varvara.h"

#include "screen.h"
#include "utils.h"

#include "data/ui/hungry.chr.h"
#include "data/ui/stats.chr.h"
#include "data/ui/light.chr.h"
#include "data/ui/text.chr.h"
#include "data/ui/levels.chr.h"
#include "data/ui/clean.chr.h"

#define UI_SIZE 16
#define UI_GAP 2
#define UI_LENGTH 4

#define UI_MODE_MAIN 0
#define UI_MODE_HAPPY 1
#define UI_MODE_HUNGRY 2
#define UI_MODE_HYGIENE 3
#define UI_MODE_CLEAN 4

#define UI_MODE_COUNT 4

#define DAY_HOVER 0x5
#define NIGHT_HOVER 0xf

#define DAY_1 0x0f7f
#define DAY_2 0x0fd6
#define DAY_3 0x0fb2

#define NIGHT_1 0xf007
#define NIGHT_2 0xf00d
#define NIGHT_3 0xf00b

typedef struct {
    int index;
    bool is_day;
    int mode;
    char disabled;
    // int is in fact a function pointer 
    // but no diff in uxn
    int callbacks[UI_LENGTH];
} UI;

UI ui;

void init_ui(UI* ui, void* on_stats, void* on_eat, void* on_clean, void* on_light) {
    ui->index = UI_LENGTH / 2;
    ui->is_day = TRUE;
    ui->mode = UI_MODE_MAIN;
    ui->disabled = 0xff;

    ui->callbacks[0] = on_stats;
    ui->callbacks[1] = on_eat;
    ui->callbacks[2] = on_clean;
    ui->callbacks[3] = on_light;
}

void set_mode_ui(UI* ui, int mode) {
    ui->mode = mode;
}

void set_disabled_ui(UI* ui, int value, int newIndexIfDisabled) {
    ui->disabled = value;
    if(is_button_disabled_ui(ui, ui->index)) {
        ui->index = newIndexIfDisabled;
    }
}

void toggle_day_ui(UI* ui) {
    ui->is_day = !ui->is_day;
    if(ui->is_day) {
        set_palette(
            DAY_1, 
            DAY_2, 
            DAY_3
        );
    } else {
        set_palette(
            NIGHT_1, 
            NIGHT_2, 
            NIGHT_3
        );
    }
}

void update_ui(UI* ui) {
    if(controller_button() == ButtonLeft) {
        int value = (ui->index - 1);
        if(value < 0) {
            value = UI_LENGTH - 1;
        }
        if(is_button_disabled_ui(ui, value)) return;
        ui->index = value;
    }
    else if(controller_button() == ButtonRight) {
        int value = (ui->index + 1) % UI_LENGTH;
        if(is_button_disabled_ui(ui, value)) return;
        ui->index = value;
    } else if(controller_button() == ButtonA) {
        if(ui->mode != UI_MODE_MAIN) {
            set_mode_ui(ui, (ui->mode + 1) % UI_MODE_COUNT); 
        } else {
            if((ui->disabled >> ui->index) & 0b00000001) return;
            callback(ui->callbacks[ui->index]);
        }
    }
}

void draw_level_ui(int stat, int x, int y) {
    int lvl = (stat * 4 / 10 + 5) / 10;
    paint(x, y + 18, 64, 16, ui.is_day ? 0x0 : 0xa, data_ui_levels_chr + 32 * lvl * 8) ;
}

AnimatedSprite cleanAnimation;
int cleanAnimationCounter;
Vector2 cleanAnimationPosition;

void draw_clean_callback_ui() {
    cleanAnimationCounter -= 1;
    if(cleanAnimationCounter <= 0) {
        set_mode_ui(&ui, UI_MODE_MAIN);
        cleanAnimation.addr = 0;
    } else {
        init_clean_ui();
    }
}

void init_clean_ui() {
    init_animation(&cleanAnimation, data_stars_chr, 2, 20, 0, &draw_clean_callback_ui);
    cleanAnimationPosition.x = random(screen_width() / 2 - 16 - 16, screen_width() / 2 + 16 - 16);
    cleanAnimationPosition.y = random(screen_height() / 2 - 16 - 16, screen_height() / 2 + 16 - 16);
}

void draw_clean_ui() {
    if(cleanAnimation.addr == 0) {
        cleanAnimationCounter = 3;
        init_clean_ui();
    }
    draw_animation(&cleanAnimation, cleanAnimationPosition.x, cleanAnimationPosition.y, 0x00);
}


bool is_button_disabled_ui(UI* ui, int index) {
    return ((ui->disabled >> index) & 0b00000001);
}

void draw_button_ui(UI* ui, int x, int y, int index, int color, unsigned char* addr) {
    if(is_button_disabled_ui(ui, index)) return;
    paint(x, y, 16, 16, color, addr);
}

bool draw_ui(UI* ui, Pet* pet) {
    int spacing = (UI_SIZE + UI_GAP * 2);
    int x = screen_width() / 2 - UI_LENGTH * spacing / 2;
    int hover = ui->is_day ? DAY_HOVER : NIGHT_HOVER;

    if(ui->mode == UI_MODE_HAPPY) {
        int x = screen_width() / 2 - 64 / 2;
        int y = screen_height() / 2 - (16 + 16 + 2) / 2;
        paint(x, y, 56, 16, 0x0, data_ui_text_chr + 28 * 8 * 0);
        draw_level_ui(pet->happy, x, y);
    } else if(ui->mode == UI_MODE_HUNGRY) {
        int x = screen_width() / 2 - 64 / 2;
        int y = screen_height() / 2 - (16 + 16 + 2) / 2;
        paint(x, y, 56, 16, 0x0, data_ui_text_chr + 28 * 8 * 1);
        draw_level_ui(pet->hunger, x, y);
    } else if(ui->mode == UI_MODE_HYGIENE) {
        int x = screen_width() / 2 - 64 / 2;
        int y = screen_height() / 2 - (16 + 16 + 2) / 2;
        paint(x, y, 56, 16, 0x0, data_ui_text_chr + 28 * 8 * 2);
        draw_level_ui(pet->hygiene, x, y);
    } else if(ui->mode == UI_MODE_CLEAN) {
        draw_clean_ui();
    } else {    
        unsigned char* icons[UI_LENGTH] = { data_ui_stats_chr, data_ui_hungry_chr, data_ui_clean_chr, data_ui_light_chr };
        for(int i = 0; i < UI_LENGTH; i++) {
            draw_button_ui(ui, x + (spacing + UI_GAP) * i, i == 2 ? UI_POS - 2 : UI_POS, i, ui->index == i ? hover : 0x00, icons[i]);
        }
        return TRUE;
    }
    return FALSE;
}

#endif 