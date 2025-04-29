#include "lib/varvara.h"
#include "./screen.h"
#include "pet.h"

#define UI 0
#define GROUND 64-16

unsigned char counter = 0;
void on_screen() {
    counter += 1;
    counter = counter % 60;
    if(counter == 0) {
        update_pet(&pet);
    }
    clear_screen();
    draw_animation(&pet.form, 96 /2 - 32 / 2, GROUND - 32, 0x0);
}

void main() {
    set_palette(
        0x0f7f, 
        0x0fd6, 
        0x0fb2
    );
    set_screen_size(96, 64);

    init_pet(&pet);
}