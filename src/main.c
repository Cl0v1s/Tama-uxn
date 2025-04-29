#include "lib/varvara.h"
#include "data/egg.chr.h"
#include "./screen.h"

#define UI 0
#define GROUND 64-16

AnimatedSprite egg;

void on_screen() {
    clear_screen();
    draw_animation(&egg, 96 /2 - 32 / 2, GROUND - 32, 0x0);
}

void main() {
    set_palette(
        0x0f7f, 
        0x0fd6, 
        0x0fb2
    );
    set_screen_size(96, 64);
    egg.addr = data_egg_chr;
    egg.frame = 0;
    egg.length = 2;
    egg.speed = 50;
    // sprite(0, 0, 0, data_egg_chr, 1);
}