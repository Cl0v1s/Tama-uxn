#ifndef FORM_H
#define FORM_H

#include "data/baby/idle1.chr.h"
#include "data/baby/idle2.chr.h"
#include "data/baby/yes.chr.h"
#include "data/baby/no.chr.h"
#include "data/baby/eat.chr.h"
#include "data/baby/attention.chr.h"

typedef struct {
    // init stats 
    int happy;
    int hunger;
    int poop;
    int weight;

    int weightMin;
    int weightMax;

    int sleepStartHour;
    int sleepStartMinute;
    int sleepEndHour;
    int sleepEndMinute;

    int hungerDecay;
    int happyDecay;
    int poopDecay;

    // animations 
    unsigned char* idle1;
    unsigned char* idle2;
    unsigned char* yes;
    unsigned char* no;
    unsigned char* eat;
    unsigned char* attention;
} Form;

#define FORMS_COUNT 1
#define FORM_BABY 0

Form FORMS[FORMS_COUNT] = {
    // FORM_BABY
    {
        50,   /* happy */
        25,   /* hunger */
        100,  /* poop */
        20,   /* weight */

        15,   /* weightMin */
        30,   /* weightMax */

        19,   /* sleepStartHour */
        0,    /* sleepStartMinute */
        7,    /* sleepEndHour */
        0,    /* sleepEndMinute */

        -75 / 30,  /* hungerDecay */
        -75 / 40,  /* happyDecay */
        -5,         /* poopDecay */

        data_baby_idle1_chr,
        data_baby_idle2_chr,
        data_baby_yes_chr,
        data_baby_no_chr,
        data_baby_eat_chr,
        data_baby_attention_chr
    }
};

#endif 