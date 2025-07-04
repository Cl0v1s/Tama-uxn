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

#define PET_STAGE_INIT 0 // 0s
#define PET_STAGE_EGG_SLOW 1  // 2min30
#define PET_STAGE_EGG_FAST 2  // 2min30
#define PET_STAGE_BABY_BEFORE_NAP 3 // 61min
#define PET_STAGE_BABY_AFTER_NAP 4 // will stop there for now
#define PET_STAGE_CHILD 5
#define PET_STAGE_ADULT 6
#define PET_STAGE_DEAD 100

#define PET_ALERT_HAPPY 1
#define PET_ALERT_HUNGRY 2
#define PET_ALERT_POOP 3
#define PET_ALERT_WEIGHT 4

#define PET_MAX_CAREMISS 3

typedef struct
{
    Date init;
    Date step;
    Date next_update;
    /**
     * Pet "age"
     */
    int stage;
    AnimatedSprite form;

    int weightMin;
    int weightMax;
    int weight;
    int happy;
    int hunger;
    unsigned char caremissCount;
    unsigned char caremissReaons[PET_MAX_CAREMISS];
    int poop;
    Date lastAlertDate;
    unsigned char lastAlertReason;

    unsigned char poopCount;

    int sleepStartHour;
    int sleepStartMinute;
    int sleepEndHour;
    int sleepEndMinute;
    bool sleeping;

    Vector2 position;
    bool can_tp;

} Pet;

Pet pet;
void* onPetStatsChanged;

AnimatedSprite sfx1;
Vector2 sfx1Position;
AnimatedSprite sfx2;
Vector2 sfx2Position;


void init_sfx_pet(AnimatedSprite* sfx, Vector2* position, int x, int y, int w, int h, int addr, int length, int speed, int repeat, void* onAnimationEnd) {
    position->x = x;
    position->y = y;
    init_animation(sfx, addr, length, speed, repeat, onAnimationEnd);
    sfx->w = w;
    sfx->h = h;
}

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
void tp_pet(Pet* pet)
{
    int min = pet->poopCount > 0 ? POOP_MAX_POOPS * 16 : 0;
    pet->position.x = random(min, screen_width() - 32);
    pet->position.y = GROUND - 32 - random(0, 3);
    // TODO: maybe try to play an animation
    animate_pet(pet, data_baby_jump_chr, 3, 30, 0, &set_idle_pet);
}

bool load_pet(Pet* pet) {
    // this load raw data into pet
    bool loaded = load(pet, sizeof(Pet));
    if(loaded) {
        /*
            since current form may require an onAnimationEnd callback 
            that can change from build to build, we need to ensure the form
            is correctly set
        */
       tp_pet(pet);
       if(onPetStatsChanged != 0) {
        callback(onPetStatsChanged);
       }
    }
    return loaded;
}

void init_pet(Pet *pet)
{
    pet->can_tp = TRUE;
    pet->stage = PET_STAGE_INIT;
    pet->hunger = 100;
    pet->caremissCount = 0;
    pet->happy = 100;
    pet->poop = 100;
    pet->poopCount = 0;
    pet->sleepStartHour = 0;
    pet->sleepStartMinute = 0;
    pet->sleepEndHour = 0;
    pet->sleepEndMinute = 0;
    pet->lastAlertReason = 0;
    pet->weight = 0;
    pet->weightMin = 0;
    pet->weightMax = 0;

    pet->position.x = screen_width() / 2 - 32 / 2;
    pet->position.y = GROUND - 32;

    init_date(&pet->lastAlertDate, FALSE);
    init_date(&pet->init, TRUE);
    init_date(&pet->next_update, TRUE);

    Date add;
    init_date(&add, FALSE);
    add.second = 0;

    copy_date(&pet->init, &pet->step);
    add_date(&pet->step, &add);

    animate_pet(pet, data_egg_chr, 2, 50, 0, 0);
}

void set_on_state_changed_pet(void* callback) {
    onPetStatsChanged = callback;
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
            sfx1.addr = 0;
            init_animation(&sfx2, data_zzz_chr, 3,50, 0, 0);
            pet->position.x = screen_width() / 2 - 32 / 2;
            pet->position.y = GROUND - 32;
        } else {
            set_idle_pet();
            sfx1.addr = 0;
            sfx2.addr = 0;
        }
        if(onPetStatsChanged != 0) callback(onPetStatsChanged);
    }
}

void dead_pet(Pet* pet) {
    pet->stage = PET_STAGE_DEAD;
    init_animation(&pet->form, data_angel_chr, 2, 50, 0, 0);
    init_sfx_pet(&sfx1, &sfx1Position, pet->position.x - 32, pet->position.y, 32, 32, data_stars_chr, 2, 50, 0, 0);
    init_sfx_pet(&sfx2, &sfx2Position, pet->position.x + 32, pet->position.y, 32, 32, data_stars_chr, 2, 50, 0, 0);
    if(onPetStatsChanged != 0) {
        callback(onPetStatsChanged);
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

    if(pet->stage == PET_STAGE_DEAD) return;

    pet->stage += 1;
    if (pet->stage == PET_STAGE_EGG_SLOW) // 0 => 1
    {
        animate_pet(pet, data_egg_chr, 2, 50, 0, 0);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.minute = 2;
        add.second = 30;
        add_date(&pet->step, &add);
    }
    else if (pet->stage == PET_STAGE_EGG_FAST) // 1 => 2
    {
        animate_pet(pet, data_egg_chr, 2, 30, 0, 0);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.minute = 2;
        add.second = 30; 
        add_date(&pet->step, &add);
    }
    else if (pet->stage == PET_STAGE_BABY_BEFORE_NAP) // 2 => 3
    {
        animate_pet(pet, data_egg_chr, 4, 15, 0, &set_idle_pet); // hatching animation

        // set next update
        init_date(&pet->next_update, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.minute = 1; // updated every 1 minute
        add_date(&pet->next_update, &add);

        // set init stats
        pet->happy = 50;
        pet->hunger = 25;
        pet->poop = 100;
        pet->weight = 20;
        pet->weightMin = 15;
        pet->weightMax = 30;

        // init sleeping state for nap
        Date now;
        init_date(&now, TRUE);
        init_date(&add, FALSE);

        // start
        add.minute = 30; // will nap after 30min
        add_date(&now, &add);
        pet->sleepStartHour = now.hour;
        pet->sleepStartMinute = now.minute;

        // end
        add.minute = 30; //  will wake up after 1h
        add_date(&now, &add);
        pet->sleepEndHour = now.hour;
        pet->sleepEndMinute = now.minute;

        add.minute = 1;
        add_date(&now, &add);

        // set next grow up after 1 min post wakeup
        copy_date(&now, &pet->step);
    }
    else if(pet->stage == PET_STAGE_BABY_AFTER_NAP) { // 3 => 4
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.year = 1; // TODO: change
        add_date(&pet->step, &add);

        //TODO: make it better to handle other stages than baby
        pet->sleepStartHour = 19;
        pet->sleepStartMinute = 0;
        pet->sleepEndHour = 7;
        pet->sleepEndMinute = 0;
    }

    if(onPetStatsChanged != 0) {
        callback(onPetStatsChanged);
    }
}

void set_alert_pet(Pet* pet, unsigned char reason) {
    // TODO: add a bip or something
    init_date(&pet->lastAlertDate, TRUE);
    Date add;
    init_date(&add, FALSE);
    add.minute = 15;
    add_date(&pet->lastAlertDate, &add);
    pet->lastAlertReason = reason;
}

void manage_poop_pet(Pet* pet) {
    if(pet->poop <= 0) {
        if(pet->poopCount >= POOP_MAX_POOPS) {
            set_alert_pet(pet, PET_ALERT_POOP);
        } else {
            pet->poopCount += 1;
            tp_pet(pet);
            init_poop();
        }
        pet->poop = 100;
    }
}


void reset_alert_pet(Pet* pet) {
    init_date(&pet->lastAlertDate, FALSE);
    pet->lastAlertReason = 0;
}

void manage_alert_pet(Pet* pet) {
    if(pet->happy <= 0) {
        set_alert_pet(pet, PET_ALERT_HAPPY);
        pet->happy = 0;
    } else if(pet->lastAlertDate.year != 0 && pet->lastAlertReason == PET_ALERT_HAPPY) {
        reset_alert_pet(pet);
    }

    if(pet->hunger <= 0) {
        set_alert_pet(pet, PET_ALERT_HUNGRY);
        pet->hunger = 0;
    } else if(pet->lastAlertDate.year != 0 && pet->lastAlertReason == PET_ALERT_HUNGRY) {
        reset_alert_pet(pet);
    }

    if(pet->weight < pet->weightMin || pet->weight > pet->weightMax) {
        set_alert_pet(pet, PET_ALERT_WEIGHT);
    } else if (pet->lastAlertDate.year != 0 && pet->lastAlertReason == PET_ALERT_WEIGHT) {
        reset_alert_pet(pet);
    }
    if(pet->weight <= 0) {
        dead_pet(pet);
    }

    if(pet->lastAlertDate.year != 0 && pet->lastAlertReason == PET_ALERT_POOP && pet->poopCount < POOP_MAX_POOPS) {
        reset_alert_pet(pet);
    }
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
    pet->happy += -75 / 40; // TODO: handle float
    pet->poop += -5; // TODO: handle float

    manage_poop_pet(pet);

    if(pet->poopCount > 0) {
        pet->happy -= (2 * pet->happy / 100) * pet->poopCount;
    }

    manage_alert_pet(pet);

    Date now;
    init_date(&now, TRUE);
    // manage alerts
    if (pet->lastAlertDate.year != 0 && compare_date(&pet->lastAlertDate, &now) <= 0)
    {
        // reset related stats
        if(pet->lastAlertReason == PET_ALERT_HAPPY) pet->happy = 100;
        else if(pet->lastAlertReason == PET_ALERT_HUNGRY) pet->hunger = 100;
        // no change for poop until next time pet is pooping, as this will trigger a new alert

        pet->caremissReaons[pet->caremissCount] = pet->lastAlertReason;
        pet->caremissCount += 1;
        reset_alert_pet(pet);
    }

    if(onPetStatsChanged != 0) {
        callback(onPetStatsChanged);
    }

    if(pet->happy <= 0) {
        dead_pet(pet);
    }

    copy_date(&now, &pet->next_update);
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
    if (compare_date(&pet->step, &now) <= 0)
    { 
        // next step if in the past
        grow_pet(pet);
    }

    if (pet->stage >= PET_STAGE_BABY_BEFORE_NAP && pet->stage < PET_STAGE_DEAD)
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

void clean_poop_pet(Pet* pet) {
    pet->poopCount = 0;
    clean_poop();
    manage_alert_pet(pet);
    if(onPetStatsChanged != 0) {
        callback(onPetStatsChanged);
    }
}

void clean_sfx_pet() {
    sfx1.addr = 0;
    sfx2.addr = 0;
}

void eat_after_pet(Pet* pet) {
    pet->position.x = screen_width() / 2 - 32 / 2;
    pet->position.y = GROUND - 32;
    if(pet->happy > 100) pet->happy =  100;
    if(pet->hunger > 100) pet->hunger = 100;
    animate_pet(pet, data_baby_eat_chr, 2, 20, 4, &set_idle_pet);
    manage_alert_pet(pet);
    if(onPetStatsChanged != 0) {
        callback(onPetStatsChanged);
    }
}

void eat_meal_pet(Pet* pet) {
    if(pet->hunger == 100) {
        animate_pet(pet, data_baby_no_chr, 2, 20, 4, &set_idle_pet);
        return;
    }
    pet->hunger += 25;
    init_sfx_pet(&sfx1, &sfx1Position, pet->position.x - 16, pet->position.y + 16, 16, 16, data_meal_chr, 3, 50, 0, &clean_sfx_pet);
    eat_after_pet(pet);
}

void eat_treat_pet(Pet* pet) {
    if(pet->hunger == 100) {
        animate_pet(pet, data_baby_no_chr, 2, 20, 4, &set_idle_pet);
        return;
    }
    pet->hunger += 5;
    pet->happy += 25;
    pet->weight += 2;
    init_sfx_pet(&sfx1, &sfx1Position, pet->position.x - 16, pet->position.y + 16, 16, 16, data_treat_chr, 3, 50, 0, &clean_sfx_pet);
    eat_after_pet(pet);
}

void draw_pet(Pet* pet, bool lightOn) {
    if(pet->form.addr != 0 && (pet->sleeping != TRUE || lightOn)) {
        draw_animation(&pet->form, pet->position.x, pet->position.y, 0x0);
    }
    if(sfx1.addr != 0) {
        draw_animation(&sfx1, sfx1Position.x, sfx1Position.y, 0x0);
    }
    if(sfx2.addr != 0) {
        draw_animation(&sfx2, sfx2Position.x, sfx2Position.y, 0x0);
    }
}

#endif