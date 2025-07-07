#ifndef MAIN_UI_H
#define MAIN_UI_H

#include "lib/varvara.h"

#include "../screen.h"
#include "../pet.h"
#include "../ui.h"

#define MAIN_UI_SIZE 16
#define MAIN_UI_GAP 2
#define MAIN_UI_LENGTH 4

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
    char disabled;
    // int is in fact a function pointer 
    // but no diff in uxn
    int callbacks[MAIN_UI_LENGTH];
} MainUI;

MainUI mainUI;


bool is_button_disabled_main_ui(int index) {
    return ((mainUI.disabled >> index) & 0b00000001);
}


void update_main_ui() {
    if(controller_button() == ButtonLeft) {
        int value = (mainUI.index - 1);
        if(value < 0) {
            value = MAIN_UI_LENGTH - 1;
        }
        if(is_button_disabled_main_ui(value)) return;
        mainUI.index = value;
    }
    else if(controller_button() == ButtonRight) {
        int value = (mainUI.index + 1) % MAIN_UI_LENGTH;
        if(is_button_disabled_main_ui(value)) return;
        mainUI.index = value;
    } else if(controller_button() == ButtonA) {
        if((mainUI.disabled  >> mainUI.index) & 0b00000001) return;
        callback((void*)mainUI.callbacks[mainUI.index]);
    }
}

void set_disabled_main_ui(int value, int newIndexIfDisabled) {
    mainUI.disabled = value;
    if(is_button_disabled_main_ui(mainUI.index)) {
        mainUI.index = newIndexIfDisabled;
    }
}

void toggle_day_main_ui() {
    set_day_main_ui(!mainUI.is_day);
}

void set_day_main_ui(bool value) {
    mainUI.is_day = value;
    if(mainUI.is_day) {
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



void draw_button_main_ui(int x, int y, int index, int color, unsigned char* addr) {
    if(is_button_disabled_main_ui(index)) return;
    paint(x, y, 16, 16, color, addr);
}

void draw_main_ui() {
    int spacing = (MAIN_UI_SIZE + MAIN_UI_GAP * 2);
    int x = screen_width() / 2 - MAIN_UI_LENGTH * spacing / 2;
    int hover = mainUI.is_day ? DAY_HOVER : NIGHT_HOVER;
    unsigned char* icons[MAIN_UI_LENGTH] = { data_ui_stats_chr, data_ui_hungry_chr, data_ui_clean_chr, data_ui_light_chr };
    // draw main UI button
    for(int i = 0; i < MAIN_UI_LENGTH; i++) {
        draw_button_main_ui(x + (spacing + MAIN_UI_GAP) * i, i == 3 ? UI_POS - 2 : UI_POS, i, mainUI.index == i ? hover : 0x00, icons[i]);
    }
    // draw alert icon
    if(pet.lastAlertDate.year != 0) {
        paint(screen_width() - 18, 0, 16, 16, 0x0, data_ui_alert_chr);
    }
}


void goto_main_ui() {
    ui.updateUI = (int)&update_main_ui;
    ui.drawUI = (int)&draw_main_ui;
    ui.drawPet = TRUE;
}

void init_main_ui(void* on_stats, void* on_eat, void* on_clean, void* on_light) {
    mainUI.index = MAIN_UI_LENGTH / 2;
    mainUI.is_day = TRUE;
    mainUI.disabled = 0xff;

    mainUI.callbacks[0] = (int)on_stats;
    mainUI.callbacks[1] = (int)on_eat;
    mainUI.callbacks[2] = (int)on_clean;
    mainUI.callbacks[3] = (int)on_light;

    goto_main_ui();
}


#endif 