#ifndef UI_H
#define UI_H

#include "lib/varvara.h"

#include "screen.h"
#include "utils.h"

#include "data/ui/hungry.chr.h"
#include "data/ui/stats.chr.h"
#include "data/ui/light.chr.h"
#include "data/ui/text.chr.h"

#define UI_SIZE 16
#define UI_GAP 2
#define UI_LENGTH 3
#define UI_COOLDOWN 10

#define UI_MODE_MAIN 1
#define UI_MODE_STATS 2

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
    // int is in fact a function pointer 
    // but no diff in uxn
    int callbacks[UI_LENGTH];
} UI;

UI ui;

void init_ui(UI* ui, void* on_stats, void* on_eat, void* on_light) {
    ui->index = UI_LENGTH / 2;
    ui->is_day = TRUE;
    ui->mode = UI_MODE_MAIN;

    ui->callbacks[0] = on_stats;
    ui->callbacks[1] = on_eat;
    ui->callbacks[2] = on_light;
}

void set_mode_ui(UI* ui, int mode) {
    ui->mode = mode;
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
        ui->index = (ui->index - 1);
        if(ui->index < 0) {
            ui->index = UI_LENGTH - 1;
        }
    }
    else if(controller_button() == ButtonRight) {
        ui->index = (ui->index + 1) % UI_LENGTH;
    } else if(controller_button() == ButtonA) {
        callback(ui->callbacks[ui->index]);
    }
}

void draw_ui(UI* ui) {
    int spacing = (UI_SIZE + UI_GAP * 2);
    int x = screen_width() / 2 - UI_LENGTH * spacing / 2;
    int hover = ui->is_day ? DAY_HOVER : NIGHT_HOVER;

    if(ui->mode == UI_MODE_STATS) {
        paint(0, screen_height() / 2 - 48 / 2, 56, 48, 0x0, data_ui_text_chr);
    } else {    
        draw_animation(&pet.form, pet.x, pet.y, 0x0);
        paint(x + 0, UI_POS, 16, 16, ui->index == 0 ? hover : 0x0, data_ui_stats_chr);
        paint(x + spacing * 1 + UI_GAP, UI_POS, 16, 16, ui->index == 1 ? hover : 0x0, data_ui_hungry_chr);
        paint(x + spacing * 2 + UI_GAP * 2, UI_POS - 2, 16, 16, ui->index == 2 ? hover : 0x0, data_ui_light_chr); // needs to be aligned
    }
}

#endif 