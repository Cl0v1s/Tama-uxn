#include "lib/varvara.h"
#include "./screen.h"
#include "pet.h"
#include "utils.h"
#include "ui.h"


unsigned char counter = 0;
void on_screen() {
    random_counter = (random_counter + datetime_second()) % 255;

    counter += 1;
    counter = counter % 60;
    if(counter == 0) {
        update_pet(&pet);
    }
    clear_screen();
    bool canDrawPet = draw_ui(&ui, &pet);
    if(canDrawPet) draw_pet(&pet);
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


void on_light() {
    back_to_main();
    toggle_day_ui(&ui);
}

void on_stats_changed() {
    if(pet.stage >= PET_BORN_STAGE || pet.stage == PET_DEAD_STAGE) {
        ui.disabled = FALSE;
    }
    if(pet.sleeping) ui.disabled = TRUE;
}

void main() {
    set_palette(
        DAY_1, 
        DAY_2, 
        DAY_3
    );
    set_screen_size(96, 64);

    init_pet(&pet, &on_stats_changed);
    init_ui(&ui, &on_stats, &on_eat, &on_light);
}
