#ifndef FOOD_UI_H
#define FOOD_UI_H

#include "lib/varvara.h"

#include "../ui.h"
#include "../utils.h"

#define FOOD_UI_LENGTH 3

typedef struct {
    int index;
    int callbacks[FOOD_UI_LENGTH];
    int labels[FOOD_UI_LENGTH];
} FoodUI;

FoodUI foodUI;

void update_food_ui() {
    if(controller_button() == ButtonLeft) {
        int value = (foodUI.index - 1);
        if(value < 0) {
            value = FOOD_UI_LENGTH - 1;
        }
        foodUI.index = value;
    }
    else if(controller_button() == ButtonRight) {
        int value = (foodUI.index + 1) % FOOD_UI_LENGTH;
        foodUI.index = value;
    } else if(controller_button() == ButtonA) {
        callback((void*)foodUI.callbacks[foodUI.index]);
    }
}

void draw_food_ui() {
    int x = screen_width() / 2 - 16 / 2;
    int y = screen_height() / 2 - FOOD_UI_LENGTH * 18 / 2;
    paint(x - 16, y + (18 * foodUI.index), 16, 16, 0x0, data_ui_cursor_chr);
    for(int i = 0; i < FOOD_UI_LENGTH; i++) {
        paint(x, y + (18 * i), 16, 16, 0x0, foodUI.labels[i]);
    }
}

void on_meal_food_ui() {

}

void on_treat_food_ui() {

}

void init_food_ui() {
    foodUI.index =  0;
    foodUI.callbacks[0] = (int)&on_meal_food_ui;
    foodUI.callbacks[1] = (int)&on_treat_food_ui;
    foodUI.callbacks[2] = (int)&goto_main_ui;

    foodUI.labels[0] = data_meal_chr;
    foodUI.labels[1] = data_treat_chr;
    foodUI.labels[2] = data_meal_chr;

    ui.updateUI = (int)&update_food_ui;
    ui.drawUI = (int)&draw_food_ui;
    ui.drawPet = FALSE;
}


#endif 