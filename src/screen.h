#ifndef SCREEN_H
#define SCREEN_H

#include "lib/varvara.h"
#include "utils.h"

typedef struct {
    unsigned char* addr;
    int length; // number of frames
    int frame; // frame counter 
    int counter; // sub-frame counter
    int speed; // animation speed (the higher the slowest)
    int play_count; // how many times we were repeated
    int repeat; // how many times we should repeat
    void* onAnimationEnd;
    int w; // animation width
    int h; // animation height
} AnimatedSprite;

void clear_screen() {
    set_screen_xy(0, 0);
    draw_pixel(0b10000010);
}

void paint(int x, int y, int w, int h, unsigned char bitcolor, unsigned char* addr) {
    int endy = y + h;
    int count = w / 8 - 1;
    set_screen_y(y);
    set_screen_addr(addr);
    set_screen_auto( (count << 4) | 0b0110);
    do {
        set_screen_x(x);
        draw_sprite(0x80 | (bitcolor & 0x0f));
    } while (screen_y() < endy);
} 

void init_animation(AnimatedSprite* animation, unsigned char* addr, int length, int speed, int repeat, void* onAnimationEnd) {
    animation->addr = addr;
    animation->length = length;
    animation->speed = speed;
    animation->repeat = repeat;
    animation->frame = 0;
    animation->counter = 0;
    animation->play_count = 0;
    animation->onAnimationEnd = onAnimationEnd;
    animation->w = 32;
    animation->h = 32;
}

void set_size_animation(AnimatedSprite* animation, int w, int h) {
    animation->h = h;
    animation->w = w;
} 

void manage_animation(AnimatedSprite* animation) {
    animation->counter = (animation->counter + 1) % (animation->speed + 1);
    animation->frame = (animation->frame + animation->counter / animation->speed) % animation->length;
    if(animation->counter == 0 && animation->frame == 0) {
        animation->play_count += 1;        
        if(animation->play_count >= animation->repeat && animation->onAnimationEnd != 0) callback(animation->onAnimationEnd);
    }
}

void draw_animation(AnimatedSprite* animation, int x, int y, unsigned char bitcolor) {
    paint(x, y, animation->w, animation->h, bitcolor, animation->addr + animation->frame * 8 * animation->w);
    manage_animation(animation);
}



#endif 