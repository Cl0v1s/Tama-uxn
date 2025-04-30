#ifndef UI_H
#define UI_H

#include "lib/varvara.h"

#include "screen.h"
#include "utils.h"

#include "data/ui/hungry.chr.h"
#include "data/ui/stats.chr.h"
#include "data/ui/light.chr.h"

#define UI_SIZE 16
#define UI_GAP 2
#define UI_LENGTH 3

#define UI_COOLDOWN 10

typedef struct {
    int index;
    // int is in fact a function pointer 
    // but no diff in uxn
    int callbacks[UI_LENGTH];
} UI;

UI ui;

void init_ui(UI* ui, void* on_stats, void* on_eat, void* on_light) {
    ui->index = UI_LENGTH / 2;

    ui->callbacks[0] = on_stats;
    ui->callbacks[1] = on_eat;
    ui->callbacks[2] = on_light;
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

    paint_icon(x + 0, UI_POS, ui->index == 0 ? 0x5 : 0x0, data_ui_stats_chr);
    paint_icon(x + spacing * 1 + UI_GAP, UI_POS, ui->index == 1 ? 0x5 : 0x0, data_ui_hungry_chr);
    paint_icon(x + spacing * 2 + UI_GAP * 2, UI_POS - 2, ui->index == 2 ? 0x5 : 0x0, data_ui_light_chr); // needs to be aligned
}

#endif 