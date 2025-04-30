#ifndef SCREEN_H
#define SCREEN_H

#include "lib/varvara.h"

typedef struct {
    unsigned char* addr;
    int length; // number of frames
    int frame; // frame counter 
    int counter; // sub-frame counter
    int speed; // animation speed (the higher the slowest)
    void* onAnimationEnd;
} AnimatedSprite;

void clear_screen() {
    set_screen_xy(0, 0);
    draw_pixel(0b10000010);
}

void paint_icon(int x, int y, unsigned char bitcolor, unsigned char* addr) {
    set_screen_xy(x, y);
    set_screen_addr(addr);
    set_screen_auto(0b00010110);
    draw_sprite(0x80 | (bitcolor & 0x0f));
    draw_sprite(0x80 | (bitcolor & 0x0f));
}

void paint_sprite(int x, int y, unsigned char bitcolor, unsigned char* addr, int frame) {
    set_screen_xy(x, y);
    set_screen_addr(addr + frame * 16 * 16);
    set_screen_auto(0b00110110);
    draw_sprite(0x80 | (bitcolor & 0x0f));
    draw_sprite(0x80 | (bitcolor & 0x0f));
    draw_sprite(0x80 | (bitcolor & 0x0f));
    draw_sprite(0x80 | (bitcolor & 0x0f));
}

void init_animation(AnimatedSprite* animation, unsigned char* addr, int length, int speed, void* onAnimationEnd) {
    animation->addr = addr;
    animation->length = length;
    animation->speed = speed;
    animation->frame = 0;
    animation->counter = 0;
    animation->onAnimationEnd = onAnimationEnd;
}

void draw_animation(AnimatedSprite* animation, int x, int y, unsigned char bitcolor) {
    paint_sprite(x, y, bitcolor, animation->addr, animation->frame);
    animation->counter = (animation->counter + 1) % (animation->speed + 1);
    animation->frame = (animation->frame + animation->counter / animation->speed) % animation->length;
    if(animation->counter == 0 && animation->frame == 0 && animation->onAnimationEnd != 0) {
        callback(animation->onAnimationEnd);
    }
}



#endif 