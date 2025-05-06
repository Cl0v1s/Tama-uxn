#include "lib/varvara.h"
#include "./screen.h"
#include "pet.h"
#include "utils.h"
#include "ui.h"
#include "poop.h"

#include "ui.h"
#include "ui/clean_ui.h"
#include "ui/main_ui.h"
#include "ui/stats_ui.h"

unsigned char counter = 0;
void on_screen() {
    random_counter = (random_counter + datetime_second()) % 255;

    counter += 1;
    counter = counter % 60;
    if(counter == 0) {
        update_pet(&pet);
    }
    clear_screen();
    callback(ui.drawUI);
    if(ui.drawPet) {
        draw_pet(&pet, mainUI.is_day);
        draw_poop();
    }
}

void on_controller() {
    if(ui.updateUI != 0) callback(ui.updateUI);
}

/**
 * UI callbacks
 */
void back_to_main() {
    init_main_ui(0, 0, 0, 0);
}

void on_stats() {
    init_stats_ui();
}

void on_eat() {
    eat_pet(&pet);
}

void on_clean() {
    clean_poop_pet(&pet);
    init_clean_ui();
}

void on_light() {
    toggle_day_main_ui();
}

bool previousSleeping = FALSE;
void on_stats_changed() {
    if(pet.stage >= PET_BORN_STAGE || pet.stage == PET_DEAD_STAGE) {
        set_disabled_main_ui(0x00, 0);
    }

    if(pet.sleeping) {
        set_disabled_main_ui(0b00000111, 2);
    } 

    if(pet.sleeping != previousSleeping) {
        toggle_day_main_ui();
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
    init_main_ui(&on_stats, &on_eat, &on_clean, &on_light);
}
