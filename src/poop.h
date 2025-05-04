#ifndef POOP_H
#define POOP_H

#include "utils.h"
#include "screen.h"

#include "data/poop.chr.h"

#define POOP_MAX_POOPS 3

Vector2 poops[POOP_MAX_POOPS];
AnimatedSprite poop;

int find_slot_poop() {
    for(int i = 0; i < POOP_MAX_POOPS; i++) {
        if(poops[i].x == 0 && poops[i].y == 0) {
            return i;
        }
    }
    return -1;
}

void init_poop() {
    if(poop.addr == 0) {
        init_animation(&poop, data_poop_chr, 3, 50, 0, 0); // TODO: add poop addr
    }

    int slot = find_slot_poop();
    if(slot == -1) return;
    poops[slot].y = GROUND - 16;
    poops[slot].x = slot * 16;
}

void clean_poop() {
    for(int i = 0; i < POOP_MAX_POOPS; i++) {
        poops[i].x = 0;
        poops[i].y = 0;
    }
}

void draw_poop() {
    if(poop.addr == 0) return;
    manage_animation(&poop);
    for(int i = 0; i < POOP_MAX_POOPS; i++) {
        if(poops[i].x == 0 && poops[i].y == 0) continue;
        paint(poops[i].x, poops[i].y, 16, 16, 0x00, poop.addr + poop.frame * 64); // TODO: certainement pété 
    }
}

#endif 