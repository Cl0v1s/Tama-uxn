#ifndef PET_H
#define PET_H

#include "lib/varvara.h"
#include "data/egg.chr.h"
#include "screen.h"
#include "date.h"
#include "utils.h"

typedef struct {
    Date init;
    Date step;
    int stage;
    AnimatedSprite form;    
} Pet;


void init_pet(Pet* pet) {
    pet->stage = 0;
    init_date(&pet->init, TRUE);

    Date add;
    add.second = 10;

    copy_date(&pet->init, &pet->step);
    add_date(&pet->step, &add);

    init_animation(&pet->form, data_egg_chr, 2, 50); 
}

void grow_pet(Pet* pet) {
    if(pet->stage == 0) {
        init_animation(&pet->form, data_egg_chr, 2, 20);
        init_date(&pet->step, TRUE);
        Date add;
        add.second = 5;
        add_date(&pet->step, &add);
    }
    pet->stage += 1;
}

void update_pet(Pet* pet) {
    Date now;
    init_date(&now, TRUE);
    if(compare_date(&pet->step, &now) <= -1) { // next step if in the past
        grow_pet(pet);
    }
}

#endif