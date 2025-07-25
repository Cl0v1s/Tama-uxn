#include "lib/varvara.h"

#include "data/meal.chr.h"
#include "data/treat.chr.h"
#include "data/ui/cursor.chr.h"
#include "data/ui/weight.chr.h"


#include "screen.h"
#include "pet.h"
#include "utils.h"
#include "ui.h"
#include "poop.h"

#include "ui.h"
#include "ui/clean_ui.h"
#include "ui/dead_ui.h"
#include "ui/main_ui.h"
#include "ui/stats_ui.h"
#include "ui/food_ui.h"

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
void on_stats() {
    init_stats_ui();
}

void on_eat() {
    init_food_ui();
}

void on_clean() {
    clean_poop_pet(&pet);
    init_clean_ui();
}

void on_light() {
    toggle_day_main_ui();
}

void on_reset() {
    init_game(FALSE);
}

bool previousSleeping = FALSE;
void on_stats_changed() {
    if(pet.stage == PET_STAGE_DEAD) {
        init_dead_ui(&on_stats, &on_reset);
    }

    if(pet.stage >= PET_STAGE_BABY_BEFORE_NAP) {
        set_disabled_main_ui(0x00, 0);
    }

    if(pet.sleeping) {
        set_disabled_main_ui(0b00000111, 2);
    } 

    if(pet.sleeping != previousSleeping) {
        toggle_day_main_ui();
    }
    previousSleeping = pet.sleeping;
    save(&pet, sizeof(Pet));
}

void init_game(bool load) {
    set_palette(
        DAY_1, 
        DAY_2, 
        DAY_3
    );
    set_screen_size(96, 64);

    clean_poop();
    
    init_main_ui(&on_stats, &on_eat, &on_clean, &on_light);
    init_pet(&pet);
    set_on_state_changed_pet(&on_stats_changed);
    if(load && load_pet(&pet)) {
        print("Loaded\n");
    }
}

void main() {
    init_game(TRUE);
}
