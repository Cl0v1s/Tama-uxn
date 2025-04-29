#ifndef PET_H
#define PET_H

#include "lib/varvara.h"
#include "screen.h"
#include "date.h"
#include "utils.h"

#include "data/egg.chr.h"

#include "data/baby/idle.chr.h"
#include "data/baby/yes.chr.h"
#include "data/baby/no.chr.h"
#include "data/baby/eat.chr.h"
#include "data/baby/jump.chr.h"

#define BORN_STAGE 2

typedef struct
{
    Date init;
    Date step;
    Date next_update;
    int stage;
    AnimatedSprite form;

    unsigned char happy;
    unsigned char strain;
    unsigned char hunger;

    unsigned char x;
    unsigned char y;
    bool can_tp;
} Pet;

Pet pet;

/**
 * CALLBACK
 */
void set_idle_pet() {
    animate_pet(&pet, data_baby_idle_chr, 2, 50, 0);
    pet.can_tp = TRUE;
}

/**
 * Replace pet randomly on the screen
 */
void tp_pet(Pet *pet)
{
    pet->x = random(0, screen_width() - 32);
    pet->y = GROUND - 32 - random(0, 10);
    // TODO: maybe try to play an animation
    animate_pet(pet, data_baby_jump_chr, 3, 30, &set_idle_pet);
}

/**
 * CALLBACK when an animation is ended the pet can tp
 */
void set_can_tp_pet()
{
    pet.can_tp = TRUE;
}

void animate_pet(Pet *pet, unsigned char *addr, int length, int speed, void *onAnimationEnd)
{
    if (onAnimationEnd == 0)
    {
        onAnimationEnd = &set_can_tp_pet;
    }
    pet->can_tp = FALSE;
    init_animation(&pet->form, addr, length, speed, onAnimationEnd);
}

void init_pet(Pet *pet)
{
    pet->can_tp = TRUE;
    pet->stage = 0;
    pet->hunger = 100;
    pet->strain = 100;
    pet->happy = 100;

    pet->x = screen_width() / 2 - 32 / 2;
    pet->y = GROUND - 32;

    init_date(&pet->init, TRUE);
    init_date(&pet->next_update, TRUE);

    Date add;
    init_date(&add, FALSE);
    add.second = 1; // TODO: change

    copy_date(&pet->init, &pet->step);
    add_date(&pet->step, &add);

    animate_pet(pet, data_egg_chr, 2, 50, 0);
}

/**
 * CALLBACK when a pet hatched
 */
void hatch_pet()
{
    init_date(&pet.next_update, TRUE);
    Date add;
    init_date(&add, FALSE);
    add.minute = 1;
    add_date(&pet.next_update, &add);
    pet.happy = 100;
    pet.hunger = 25;
    pet.strain = 19;

    animate_pet(&pet, data_baby_idle_chr, 2, 50, 0);
}

/**
 * Is called when pet is growing up
 */
void grow_pet(Pet *pet)
{
    if (pet->stage == 0)
    {
        animate_pet(pet, data_egg_chr, 2, 30, 0);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.second = 1; // TODO: change
        add_date(&pet->step, &add);
    }
    if (pet->stage == 1)
    {
        animate_pet(pet, data_egg_chr, 2, 15, 0);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.second = 1; // TODO: change
        add_date(&pet->step, &add);
    }
    if (pet->stage == 2)
    {
        animate_pet(pet, data_egg_chr, 4, 15, &hatch_pet);
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
void stats_pet(Pet *pet)
{
    // BABY stats
    // TODO: set other stages
    pet->hunger += -75 / 30; // TODO: handle float
    pet->strain += -75 / 40; // TODO: handle float

    // GLOBAL stats
    if (pet->hunger < 25 || pet->strain < 25)
    {
        pet->happy += -5;
    }
    if (pet->hunger > 50 && pet->strain > 50)
    {
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
void update_pet(Pet *pet)
{
    Date now;
    init_date(&now, TRUE);

    // handle growing up
    if (compare_date(&pet->step, &now) <= -1)
    { // next step if in the past
        grow_pet(pet);
    }

    if (pet->stage > BORN_STAGE)
    {
        // handle stats
        if (compare_date(&pet->next_update, &now) <= -1)
        { // next step if in the past
            stats_pet(pet);
        }
        // 10% chance to tp pet somewhere on the screen if he can tp
        if (random(0, 100) < 10 && pet->can_tp)
        {
            console_write('g');
            tp_pet(pet);
        }
    }
}

#endif