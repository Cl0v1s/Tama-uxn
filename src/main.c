#include "lib/varvara.h"
#include "./screen.h"
#include "pet.h"
#include "utils.h"
#include "ui.h"
#include "poop.h"

unsigned char counter = 0;
void on_screen() {
    random_counter = (random_counter + datetime_second()) % 255;

    counter += 1;
    counter = counter % 60;
    if(counter == 0) {
        update_pet(&pet);
    }
    clear_screen();
    bool canDraw = draw_ui(&ui, &pet);
    if(canDraw) {
        draw_pet(&pet);
        draw_poop();
    }
}

void on_controller() {
    update_ui(&ui);
}

/**
 * UI callbacks
 */
void back_to_main() {
    set_mode_ui(&ui, UI_MODE_MAIN);
}


void on_stats() {
    if(ui.mode == UI_MODE_MAIN) {
        set_mode_ui(&ui, UI_MODE_HAPPY);
    } else {
        back_to_main();
    }
}

void on_eat() {
    back_to_main();
    eat_pet(&pet);
}

void on_clean() {
    back_to_main();
    clean_poop_pet(&pet);
    set_mode_ui(&ui, UI_MODE_CLEAN);
}

void on_light() {
    back_to_main();
    toggle_day_ui(&ui);
}


bool previousSleeping = FALSE;
void on_stats_changed() {
    if(pet.stage >= PET_BORN_STAGE || pet.stage == PET_DEAD_STAGE) {
        set_disabled_ui(&ui, 0x00, 0);
    }

    if(pet.sleeping) {
        set_disabled_ui(&ui, 0b00000011, 2);
    } 

    if(pet.sleeping != previousSleeping) {
        toggle_day_ui(&ui);
    }
    previousSleeping = pet.sleeping;
}

void main() {
    set_palette(
        DAY_1, 
        DAY_2, 
        DAY_3
    );
    set_screen_size(96, 64);

    clean_poop();
    init_pet(&pet, &on_stats_changed);
    init_ui(&ui, &on_stats, &on_eat, &on_clean, &on_light);
}
