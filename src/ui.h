#ifndef UI_H
#define UI_H


#include "data/ui/hungry.chr.h"
#include "data/ui/stats.chr.h"
#include "data/ui/light.chr.h"
#include "data/ui/text.chr.h"
#include "data/ui/levels.chr.h"
#include "data/ui/clean.chr.h"
#include "data/ui/alert.chr.h"


typedef struct {
    bool drawPet;
    // this is actually a void*
    int updateUI;
    // this too
    int drawUI;
} UI;

UI ui;


#endif 