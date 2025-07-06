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

#define FORM_BABY 0
const Form FormBaby = {
    .happy = 50,
    .hunger = 25,
    .poop = 100,
    .weight = 20,

    .weightMin = 15,
    .weightMax = 30,

    // this is usefull only after nap
    .sleepStartHour = 19,
    .sleepStartMinute = 0,
    .sleepEndHour = 7,
    .sleepEndMinute = 0,

    .hungerDecay = -75 / 30,
    .happyDecay = -75 / 40,
    .poopDecay = -5,
};


#define FORMS_COUNT 1
const Form FORMS[FORMS_COUNT] = { FormBaby };

#endif 