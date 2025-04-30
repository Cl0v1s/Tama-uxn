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
    update_ui(&ui);
    clear_screen();
    draw_animation(&pet.form, pet.x, pet.y, 0x0);
    draw_ui(&ui);
}


/**
 * UI callbacks
 */
void on_stats() {
    console_write('S');
}

void on_eat() {
    eat_pet(&pet);
}

void on_light() {

}

void main() {
    set_palette(
        0x0f7f, 
        0x0fd6, 
        0x0fb2
    );
    set_screen_size(96, 64);

    init_pet(&pet);
    init_ui(&ui, &on_stats, &on_eat, &on_light);
}
