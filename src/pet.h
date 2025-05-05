#ifndef PET_H
#define PET_H

#include "lib/varvara.h"
#include "screen.h"
#include "date.h"
#include "utils.h"
#include "poop.h"

#include "data/zzz.chr.h"

// dead
#include "data/stars.chr.h"
#include "data/angel.chr.h"

// egg
#include "data/egg.chr.h"

// baby
#include "data/baby/idle.chr.h"
#include "data/baby/yes.chr.h"
#include "data/baby/no.chr.h"
#include "data/baby/eat.chr.h"
#include "data/baby/jump.chr.h"

#define PET_BORN_STAGE 2
#define PET_BABY_NAP_STAGE 3
#define PET_DEAD_STAGE 100

typedef struct
{
    Date init;
    Date step;
    Date next_update;
    int stage;
    AnimatedSprite form;

    unsigned char happy;
    unsigned char hygiene;
    unsigned char hunger;
    int poop;

    unsigned char poopCount;

    int sleepStartHour;
    int sleepStartMinute;
    int sleepEndHour;
    int sleepEndMinute;
    bool sleeping;

    Vector2 position;
    bool can_tp;

    // is actually a function pointer
    int onStatsChanged;
} Pet;

Pet pet;
AnimatedSprite sfx1;
AnimatedSprite sfx2;

/**
 * CALLBACK when an animation is ended the pet can tp
 */
void set_can_tp_pet()
{
    pet.can_tp = TRUE;
}

void animate_pet(Pet *pet, unsigned char *addr, int length, int speed, int repeat, void *onAnimationEnd)
{
    if (onAnimationEnd == 0)
    {
        onAnimationEnd = &set_can_tp_pet;
    }
    pet->can_tp = FALSE;
    init_animation(&pet->form, addr, length, speed, repeat, onAnimationEnd);
}

/**
 * CALLBACK
 */
void set_idle_pet() {
    animate_pet(&pet, data_baby_idle_chr, 2, 50, 0, 0);
    pet.can_tp = TRUE;
}

/**
 * Replace pet randomly on the screen
 */
void tp_pet(Pet *pet)
{
    int min = pet->poopCount > 0 ? POOP_MAX_POOPS * 16 : 0;
    pet->position.x = random(min, screen_width() - 32);
    pet->position.y = GROUND - 32 - random(0, 3);
    // TODO: maybe try to play an animation
    animate_pet(pet, data_baby_jump_chr, 3, 30, 0, &set_idle_pet);
}

void init_pet(Pet *pet, void* onStatsChanged)
{
    pet->can_tp = TRUE;
    pet->stage = 0;
    pet->hunger = 100;
    pet->hygiene = 100;
    pet->happy = 100;
    pet->poop = 100;
    pet->poopCount = 0;
    pet->sleepStartHour = 0;
    pet->sleepStartMinute = 0;
    pet->sleepEndHour = 0;
    pet->sleepEndMinute = 0;

    pet->position.x = screen_width() / 2 - 32 / 2;
    pet->position.y = GROUND - 32;

    pet->onStatsChanged = onStatsChanged;

    init_date(&pet->init, TRUE);
    init_date(&pet->next_update, TRUE);

    Date add;
    init_date(&add, FALSE);
    add.second = 1; // TODO: change

    copy_date(&pet->init, &pet->step);
    add_date(&pet->step, &add);

    animate_pet(pet, data_egg_chr, 2, 50, 0, 0);
}

void manage_sleep_pet(Pet* pet) {
    int currentTime = datetime_hour() * 60 + datetime_minute();
    int sleepStartTime = pet->sleepStartHour * 60 + pet->sleepStartMinute;
    int sleepEndTime = pet->sleepEndHour * 60 + pet->sleepEndMinute;

    bool before = pet->sleeping;
    // Vérifier si l'heure actuelle est dans la plage de sommeil
    if (sleepStartTime <= sleepEndTime) {
        // Cas où la plage de sommeil ne traverse pas minuit
        if (currentTime >= sleepStartTime && currentTime <= sleepEndTime) {
            pet->sleeping = TRUE;
        } else {
            pet->sleeping = FALSE;
        }
    } else {
        // Cas où la plage de sommeil traverse minuit
        if (currentTime >= sleepStartTime || currentTime <= sleepEndTime) {
            pet->sleeping = TRUE;
        } else {
            pet->sleeping = FALSE;
        }
    }

    if(before != pet->sleeping) {
        if(pet->sleeping) {
            pet->form.addr = 0;
            sfx1.addr = 0;
            init_animation(&sfx2, data_zzz_chr, 3,50, 0, 0);
            pet->position.x = screen_width() / 2 - 32 / 2;
            pet->position.y = GROUND - 32;
        } else {
            print("stop\n");
            sfx1.addr = 0;
            sfx2.addr = 0;
            set_idle_pet();
        }
        if(pet->onStatsChanged != 0) callback(pet->onStatsChanged);
    }
}

void dead_pet(Pet* pet) {
    pet->stage = PET_DEAD_STAGE;
    init_animation(&pet->form, data_angel_chr, 2, 50, 0, 0);
    init_animation(&sfx1, data_stars_chr, 2, 50, 0, 0);
    init_animation(&sfx2, data_stars_chr, 2, 50, 0, 0);
    if(pet->onStatsChanged != 0) {
        callback(pet->onStatsChanged);
    }
} 
 
/**
 * Is called when pet is growing up
 */
void grow_pet(Pet *pet)
{
    print("GROW ");
    printInt(pet->stage);
    putchar('>');
    printInt(pet->stage+1);
    putchar('\n');
    if(pet->stage == PET_DEAD_STAGE) return;
    if (pet->stage == 0)
    {
        animate_pet(pet, data_egg_chr, 2, 30, 0, 0);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.second = 1; // TODO: change
        add_date(&pet->step, &add);
    }
    else if (pet->stage == 1)
    {
        animate_pet(pet, data_egg_chr, 2, 15, 0, 0);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.second = 1; // TODO: change
        add_date(&pet->step, &add);
    }
    else if (pet->stage == PET_BORN_STAGE)
    {
        animate_pet(pet, data_egg_chr, 4, 15, 0, &set_idle_pet);

        // set next update
        init_date(&pet->next_update, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.minute = 1;
        add_date(&pet->next_update, &add);

        // set init stats
        pet->happy = 100;
        pet->hunger = 25;
        pet->hygiene = 19;
        pet->poop = 100;

        // init sleeping state for nap
        Date now;
        init_date(&now, TRUE);
        init_date(&add, FALSE);

        // start
        add.minute = 1; // TODO: change
        add_date(&now, &add);
        pet->sleepStartHour = now.hour;
        pet->sleepStartMinute = now.minute;

        // end
        add.minute = 2; // TODO: change 
        add_date(&now, &add);
        pet->sleepEndHour = now.hour;
        pet->sleepEndMinute = now.minute;

        // set next grow up when wakeup
        copy_date(&now, &pet->step);
    }
    else if(pet->stage == PET_BABY_NAP_STAGE) {
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.year = 1; // TODO: change
        add_date(&pet->step, &add);

        //TODO: make it better to handle other stages than baby
        pet->sleeping =  FALSE;
        pet->sleepStartHour = 19;
        pet->sleepStartMinute = 0;
        pet->sleepEndHour = 7;
        pet->sleepEndMinute = 0;
    }

    if(pet->onStatsChanged != 0) {
        callback(pet->onStatsChanged);
    }
    pet->stage += 1;
}

void manage_poop_pet(Pet* pet) {
    if(pet->poop <= 0) {
        if(pet->poopCount >= POOP_MAX_POOPS) {
            pet->happy -= 25;
        } else {
            pet->poopCount += 1;
            tp_pet(pet);
            init_poop();
        }
        pet->poop = 100;
    }
}

void clean_poop_pet(Pet* pet) {
    pet->poopCount = 0;
    clean_poop();
}

/**
 * Is called when pet stats need to be updated
 */
void stats_pet(Pet *pet)
{
    print("Stats\n");
    manage_sleep_pet(pet);

    // BABY stats
    // TODO: set other stages
    pet->hunger += -75 / 30; // TODO: handle float
    pet->hygiene += -75 / 40; // TODO: handle float
    pet->poop += -5; // TODO: handle float

    manage_poop_pet(pet);

    // handle happy
    if(pet->poopCount > 0) {
        pet->happy -= (2 * pet->happy / 100) * pet->poopCount;
    }
    if (pet->hunger < 25 || pet->hygiene < 25)
    {
        pet->happy += -5;
    }
    if (pet->hunger > 50 && pet->hygiene > 50)
    {
        pet->happy += 1;
    }

    init_date(&pet->next_update, TRUE);
    Date add;
    init_date(&add, FALSE);
    add.minute = 1;
    add_date(&pet->next_update, &add);

    if(pet->happy <= 0) {
        dead_pet(pet);
    }
}

/**
 * Is called each frame to manage pet situation
 */
void update_pet(Pet *pet)
{
    Date now;
    init_date(&now, TRUE);

    // handle growing up
    if (compare_date(&pet->step, &now) <= 0)
    { 
        // next step if in the past
        grow_pet(pet);
    }

    if (pet->stage > PET_BORN_STAGE && pet->stage != PET_DEAD_STAGE)
    {
        // handle stats
        if (compare_date(&pet->next_update, &now) <= 0)
        { // next step if in the past
            stats_pet(pet);
        }
        // 10% chance to tp pet somewhere on the screen if he can tp
        if (random(0, 100) < 10 && pet->can_tp && pet->sleeping == FALSE)
        {
            tp_pet(pet);
        }
    }
}

void eat_pet(Pet* pet) {
    if(pet->hunger == 100) {
        animate_pet(pet, data_baby_no_chr, 2, 20, 4, &set_idle_pet);
        return;
    }
    pet->hunger += 50;
    if(pet->hunger > 100) pet->hunger = 100;
    animate_pet(pet, data_baby_eat_chr, 2, 20, 4, &set_idle_pet);
}


void draw_pet(Pet* pet) {
    if(pet->form.addr != 0) {
        draw_animation(&pet->form, pet->position.x, pet->position.y, 0x0);
    }
    if(sfx1.addr != 0) {
        draw_animation(&sfx1, pet->position.x - 32, pet->position.y, 0x0);
    }
    if(sfx2.addr != 0) {
        draw_animation(&sfx2, pet->position.x + 32, pet->position.y, 0x0);
    }
}

#endif