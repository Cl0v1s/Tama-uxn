#ifndef PET_H
#define PET_H

#include "lib/varvara.h"
#include "data/egg.chr.h"
#include "screen.h"
#include "date.h"
#include "utils.h"

#define BORN_STAGE 2

typedef struct {
    Date init;
    Date step;
    Date next_update;
    int stage;
    AnimatedSprite form;    

    unsigned char happy;
    unsigned char strain;
    unsigned char hunger;
} Pet;

Pet pet;

void init_pet(Pet* pet) {
    pet->stage = 0;
    pet->hunger = 100;
    pet->strain = 100;
    pet->happy = 100;

    init_date(&pet->init, TRUE);
    init_date(&pet->next_update, TRUE);

    Date add;
    init_date(&add, FALSE);
    add.second = 20;

    copy_date(&pet->init, &pet->step);
    add_date(&pet->step, &add);

    init_animation(&pet->form, data_egg_chr, 2, 50, 0); 
}

// Animation functions 

/**
 * The pet is hacthing out of its egg
 */
void hatch_pet() {
    init_date(&pet.next_update, TRUE);
    Date add;
    init_date(&add, FALSE);
    add.minute = 1;
    add_date(&pet.next_update, &add);
    pet.happy = 100;
    pet.hunger = 25;
    pet.strain = 19;
}

/**
 * Replace pet randomly on the screen
 */
void tp_pet() {

}

/**
 * Is called when pet is growing up
 */
void grow_pet(Pet* pet) {
    if(pet->stage == 0) {
        init_animation(&pet->form, data_egg_chr, 2, 30, 0);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.second = 10;
        add_date(&pet->step, &add);
    }
    if(pet->stage == 1) {
        init_animation(&pet->form, data_egg_chr, 2, 15, 0);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.second = 5;
        add_date(&pet->step, &add);
    }
    if(pet->stage == 2) {
        init_animation(&pet->form, data_egg_chr, 4, 15, &hatch_pet);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.year = 1;
        add_date(&pet->step, &add);
    }
    pet->stage += 1;
}

/**
 * Is called when pet stats need to be updated
 */
void stats_pet(Pet* pet) {
    // BABY stats 
    // TODO: set other stages
    pet->hunger += -75/30; // TODO: handle float
    pet->strain += -75/40; // TODO: handle float
    
    
    // GLOBAL stats 
    if(pet->hunger < 25 || pet->strain < 25) {
        pet->happy += -5;
    } 
    if(pet->hunger > 50 && pet->strain > 50) {
        pet->happy += 1;
    }

    init_date(&pet->next_update, TRUE);
    Date add;
    init_date(&add, FALSE);
    add.minute = 1;
    add_date(&pet->next_update, &add);
}

/**
 * Is called each frame to manage pet situation
 */
void update_pet(Pet* pet) {
    Date now;
    init_date(&now, TRUE);
    // handle stats 
    if(pet->stage > BORN_STAGE) {
        if(compare_date(&pet->next_update, &now) <= -1) { // next step if in the past
            stats_pet(pet);
        }
    }

    // handle growing up
    if(compare_date(&pet->step, &now) <= -1) { // next step if in the past
        grow_pet(pet);
    }
}

#endif