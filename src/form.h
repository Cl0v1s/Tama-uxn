#ifndef FORM_H
#define FORM_H

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
        -5         /* poopDecay */
    }
};

#endif 