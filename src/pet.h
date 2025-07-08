#ifndef PET_H
#define PET_H

#include "lib/varvara.h"
#include "screen.h"
#include "date.h"
#include "utils.h"
#include "poop.h"
#include "form.h"

#include "data/zzz.chr.h"

// dead
#include "data/stars.chr.h"
#include "data/angel.chr.h"

// egg
#include "data/egg.chr.h"

// baby

#define PET_STAGE_INIT 0            // 0s
#define PET_STAGE_EGG_SLOW 1        // 2min30
#define PET_STAGE_EGG_FAST 2        // 2min30
#define PET_STAGE_BABY_BEFORE_NAP 3 // 61min
#define PET_STAGE_BABY_AFTER_NAP 4  // will stop there for now
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
    AnimatedSprite sprite;
    int formId;

    int weight;
    int happy;
    int hunger;
    unsigned char caremissCount;
    unsigned char caremissReaons[PET_MAX_CAREMISS];
    int poop;
    Date lastAlertDate;
    unsigned char lastAlertReason;

    unsigned char poopCount;

    /// only used in baby before nap
    int napStartHour;
    int napStartMinute;
    int napEndHour;
    int napEndMinute;
    bool sleeping;

    Vector2 position;
    bool can_tp;

} Pet;

Pet pet;
void *onPetStatsChanged;

AnimatedSprite sfx1;
Vector2 sfx1Position;
AnimatedSprite sfx2;
Vector2 sfx2Position;

void init_sfx_pet(AnimatedSprite *sfx, Vector2 *position, int x, int y, int w, int h, unsigned char *addr, int length, int speed, int repeat, void *onAnimationEnd)
{
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
    init_animation(&pet->sprite, addr, length, speed, repeat, onAnimationEnd);
}

/**
 * CALLBACK
 */
void set_idle_pet()
{
    animate_pet(&pet, FORMS[pet.formId].idle1, 2, 50, 0, 0);
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
    animate_pet(pet, FORMS[pet->formId].idle2, 3, 30, 0, &set_idle_pet);
}

bool load_pet(Pet *pet)
{
    // this load raw data into pet
    bool loaded = load(pet, sizeof(Pet));
    if (loaded)
    {
        /*
            since current form may require an onAnimationEnd callback
            that can change from build to build, we need to ensure the form
            is correctly set
        */
        int x = pet->position.x;
        int y = pet->position.y;
        tp_pet(pet);
        pet->position.x = x;
        pet->position.y = y;

        // fast forward until now
        Date now, add;
        init_date(&add, FALSE);
        add.minute = 1;
        init_date(&now, TRUE);
        int mins = diff_min_date(&pet->next_update, &now);
        copy_date(&pet->next_update, &now);

        for (int i = 0; i < mins; i++)
        {
            add_date(&now, &add);
            stats_pet(pet, &now);
        }

        if (onPetStatsChanged != 0)
        {
            callback(onPetStatsChanged);
        }
    }
    return loaded;
}

void init_pet(Pet *pet)
{
    pet->formId = FORM_BABY;
    pet->can_tp = TRUE;
    pet->stage = PET_STAGE_INIT;
    pet->hunger = 100;
    pet->caremissCount = 0;
    pet->happy = 100;
    pet->poop = 100;
    pet->poopCount = 0;
    pet->napStartHour = 0;
    pet->napStartMinute = 0;
    pet->napEndHour = 0;
    pet->napEndMinute = 0;
    pet->lastAlertReason = 0;
    pet->weight = 0;

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

void set_on_state_changed_pet(void *callback)
{
    onPetStatsChanged = callback;
}

void manage_sleep_pet(Pet *pet)
{
    int currentTime = datetime_hour() * 60 + datetime_minute();
    int sleepStartTime = pet->napStartHour * 60 + pet->napStartMinute;
    int sleepEndTime = pet->napEndHour * 60 + pet->napEndMinute;
    if (pet->stage >= PET_STAGE_BABY_AFTER_NAP)
    {
        sleepStartTime = FORMS[pet->formId].sleepStartHour * 60 + FORMS[pet->formId].sleepStartMinute;
        sleepEndTime = FORMS[pet->formId].sleepEndHour * 60 + FORMS[pet->formId].sleepEndMinute;
    }

    // Vérifier si l'heure actuelle est dans la plage de sommeil
    if (sleepStartTime <= sleepEndTime)
    {
        // Cas où la plage de sommeil ne traverse pas minuit
        if (currentTime >= sleepStartTime && currentTime <= sleepEndTime)
        {
            pet->sleeping = TRUE;
        }
        else
        {
            pet->sleeping = FALSE;
        }
    }
    else
    {
        // Cas où la plage de sommeil traverse minuit
        if (currentTime >= sleepStartTime || currentTime <= sleepEndTime)
        {
            pet->sleeping = TRUE;
        }
        else
        {
            pet->sleeping = FALSE;
        }
    }

    if (pet->sleeping)
    {
        sfx1.addr = 0;
        pet->position.x = screen_width() / 2 - 32;
        pet->position.y = GROUND - 32;
        init_sfx_pet(&sfx2, &sfx2Position, pet->position.x + 24, pet->position.y, 32, 32, data_zzz_chr, 3, 50, 0, 0);
    }
    else
    {
        set_idle_pet();
        print("up\n");
        sfx1.addr = 0;
        sfx2.addr = 0;
    }
}

void dead_pet(Pet *pet)
{
    pet->stage = PET_STAGE_DEAD;
    init_animation(&pet->sprite, data_angel_chr, 2, 50, 0, 0);
    init_sfx_pet(&sfx1, &sfx1Position, pet->position.x - 32, pet->position.y, 32, 32, data_stars_chr, 2, 50, 0, 0);
    init_sfx_pet(&sfx2, &sfx2Position, pet->position.x + 32, pet->position.y, 32, 32, data_stars_chr, 2, 50, 0, 0);
    if (onPetStatsChanged != 0)
    {
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
    printInt(pet->stage + 1);
    putchar('\n');

    if (pet->stage == PET_STAGE_DEAD)
        return;

    pet->stage += 1;
    if (pet->stage == PET_STAGE_EGG_SLOW) // 0 => 1
    {
        animate_pet(pet, data_egg_chr, 2, 50, 0, 0);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.minute = 0; // 2;
        add.second = 1; // 30;
        add_date(&pet->step, &add);
    }
    else if (pet->stage == PET_STAGE_EGG_FAST) // 1 => 2
    {
        animate_pet(pet, data_egg_chr, 2, 30, 0, 0);
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.minute = 0; // 2;
        add.second = 1; // 30;
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
        pet->happy = FORMS[pet->formId].happy;
        pet->hunger = FORMS[pet->formId].hunger;
        pet->poop = FORMS[pet->formId].poop;
        pet->weight = FORMS[pet->formId].weight;

        // init sleeping state for nap
        Date now;
        init_date(&now, TRUE);
        init_date(&add, FALSE);

        // start
        add.minute = 30; // will nap after 30min
        add_date(&now, &add);
        pet->napStartHour = now.hour;
        pet->napStartMinute = now.minute;

        // end
        add.minute = 30; //  will wake up after 1h
        add_date(&now, &add);
        pet->napEndHour = now.hour;
        pet->napEndMinute = now.minute;

        add.minute = 1;
        add_date(&now, &add);

        // set next grow up after 1 min post wakeup
        copy_date(&now, &pet->step);
    }
    else if (pet->stage == PET_STAGE_BABY_AFTER_NAP)
    { // 3 => 4
        init_date(&pet->step, TRUE);
        Date add;
        init_date(&add, FALSE);
        add.year = 1; // TODO: change
        add_date(&pet->step, &add);

        // this is not used anymore
        pet->napStartHour = 0;
        pet->napStartMinute = 0;
        pet->napEndHour = 0;
        pet->napEndMinute = 0;
    }

    if (onPetStatsChanged != 0)
    {
        callback(onPetStatsChanged);
    }
}

void set_alert_pet(Pet *pet, unsigned char reason)
{
    // TODO: add a bip or something
    init_date(&pet->lastAlertDate, TRUE);
    Date add;
    init_date(&add, FALSE);
    add.minute = 15;
    add_date(&pet->lastAlertDate, &add);
    pet->lastAlertReason = reason;
}

void manage_poop_pet(Pet *pet)
{
    if (pet->poop <= 0)
    {
        if (pet->poopCount >= POOP_MAX_POOPS)
        {
            set_alert_pet(pet, PET_ALERT_POOP);
        }
        else
        {
            pet->poopCount += 1;
            tp_pet(pet);
            init_poop();
        }
        pet->poop = 100;
    }
}

void reset_alert_pet(Pet *pet)
{
    init_date(&pet->lastAlertDate, FALSE);
    pet->lastAlertReason = 0;
}

void manage_alert_pet(Pet *pet)
{
    if (pet->happy <= 0)
    {
        set_alert_pet(pet, PET_ALERT_HAPPY);
        pet->happy = 0;
    }
    else if (pet->lastAlertDate.year != 0 && pet->lastAlertReason == PET_ALERT_HAPPY)
    {
        reset_alert_pet(pet);
    }

    if (pet->hunger <= 0)
    {
        set_alert_pet(pet, PET_ALERT_HUNGRY);
        pet->hunger = 0;
    }
    else if (pet->lastAlertDate.year != 0 && pet->lastAlertReason == PET_ALERT_HUNGRY)
    {
        reset_alert_pet(pet);
    }

    if (pet->weight < FORMS[pet->formId].weightMin || pet->weight > FORMS[pet->formId].weightMax)
    {
        set_alert_pet(pet, PET_ALERT_WEIGHT);
    }
    else if (pet->lastAlertDate.year != 0 && pet->lastAlertReason == PET_ALERT_WEIGHT)
    {
        reset_alert_pet(pet);
    }
    if (pet->weight <= 0)
    {
        dead_pet(pet);
    }

    if (pet->lastAlertDate.year != 0 && pet->lastAlertReason == PET_ALERT_POOP && pet->poopCount < POOP_MAX_POOPS)
    {
        reset_alert_pet(pet);
    }
}

/**
 * Is called when pet stats need to be updated
 */
void stats_pet(Pet *pet, Date *now)
{
    print("Stats ");
    printInt(now->day);
    print("/");
    printInt(now->month);
    print("/");
    printInt(now->year);
    print(" ");
    printInt(now->hour);
    print(":");
    printInt(now->minute);
    print("\n");
    manage_sleep_pet(pet);

    pet->hunger += FORMS[pet->formId].hungerDecay; // TODO: handle float
    pet->happy += FORMS[pet->formId].happyDecay;   // TODO: handle float
    pet->poop += FORMS[pet->formId].poopDecay;     // TODO: handle float

    manage_poop_pet(pet);

    if (pet->poopCount > 0)
    {
        pet->happy -= (2 * pet->happy / 100) * pet->poopCount;
    }

    manage_alert_pet(pet);

    // manage alerts
    if (pet->lastAlertDate.year != 0 && compare_date(&pet->lastAlertDate, now) <= 0)
    {
        // reset related stats
        if (pet->lastAlertReason == PET_ALERT_HAPPY)
            pet->happy = 100;
        else if (pet->lastAlertReason == PET_ALERT_HUNGRY)
            pet->hunger = 100;
        // no change for poop until next time pet is pooping, as this will trigger a new alert

        pet->caremissReaons[pet->caremissCount] = pet->lastAlertReason;
        pet->caremissCount += 1;
        reset_alert_pet(pet);
    }

    if (onPetStatsChanged != 0)
    {
        callback(onPetStatsChanged);
    }

    if (pet->happy <= 0)
    {
        dead_pet(pet);
    }

    copy_date(now, &pet->next_update);
    Date add;
    init_date(&add, FALSE);
    add.minute = 1;
    add_date(&pet->next_update, &add);

    print("Sleeping: ");
    printInt(pet->sleeping);
    print(" Hu: ");
    printInt(pet->hunger);
    print(" Ha: ");
    printInt(pet->happy);
    print(" Ca: ");
    printInt(pet->caremissCount);
    print("\n");
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
        {
            // next step is in the past
            stats_pet(pet, &now);
        }
        // 10% chance to tp pet somewhere on the screen if he can tp
        if (random(0, 100) < 10 && pet->can_tp && pet->sleeping == FALSE)
        {
            tp_pet(pet);
        }
    }
}

void clean_poop_pet(Pet *pet)
{
    pet->poopCount = 0;
    clean_poop();
    manage_alert_pet(pet);
    if (onPetStatsChanged != 0)
    {
        callback(onPetStatsChanged);
    }
}

void clean_sfx_pet()
{
    sfx1.addr = 0;
    sfx2.addr = 0;
}

void eat_after_pet(Pet *pet)
{
    pet->position.x = screen_width() / 2;
    pet->position.y = GROUND - 32;
    if (pet->happy > 100)
        pet->happy = 100;
    if (pet->hunger > 100)
        pet->hunger = 100;
    animate_pet(pet, FORMS[pet->formId].eat, 2, 20, 4, &set_idle_pet);
    manage_alert_pet(pet);
    if (onPetStatsChanged != 0)
    {
        callback(onPetStatsChanged);
    }
}

void eat_meal_pet(Pet *pet)
{
    if (pet->hunger == 100)
    {
        animate_pet(pet, FORMS[pet->formId].no, 2, 20, 4, &set_idle_pet);
        return;
    }
    pet->hunger += 25;
    eat_after_pet(pet);
    init_sfx_pet(&sfx1, &sfx1Position, pet->position.x - 16, pet->position.y + 16, 16, 16, data_meal_chr, 3, 50, 0, &clean_sfx_pet);
}

void eat_treat_pet(Pet *pet)
{
    if (pet->hunger == 100)
    {
        animate_pet(pet, FORMS[pet->formId].no, 2, 20, 4, &set_idle_pet);
        return;
    }
    pet->hunger += 5;
    pet->happy += 25;
    pet->weight += 2;
    eat_after_pet(pet);
    init_sfx_pet(&sfx1, &sfx1Position, pet->position.x - 16, pet->position.y + 16, 16, 16, data_treat_chr, 3, 50, 0, &clean_sfx_pet);
}

void draw_pet(Pet *pet, bool lightOn)
{
    if (pet->sprite.addr != 0 && (pet->sleeping != TRUE || lightOn))
    {
        draw_animation(&pet->sprite, pet->position.x, pet->position.y, 0x0);
    }
    if (sfx1.addr != 0)
    {
        draw_animation(&sfx1, sfx1Position.x, sfx1Position.y, 0x0);
    }
    if (sfx2.addr != 0)
    {
        draw_animation(&sfx2, sfx2Position.x, sfx2Position.y, 0x0);
    }
}

#endif