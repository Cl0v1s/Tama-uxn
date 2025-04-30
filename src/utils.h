#ifndef UTILS_H
#define UTILS_H


typedef int bool;
#define TRUE  1
#define FALSE 0

unsigned char random_counter;
int random(int min, int max) { // TODO: rework by adding a table 
    int result = min + ((random_counter) % (max - min));
    random_counter /= 2;
    return result;
}

void callback(void* func) {
    asm(func, "JSR2");
}

/**
 * Print 
 */
void printInt(int val) {
    int isNegative = 0;
    if (val < 0) {
        isNegative = 1;
        val = -val;
    }

    char buffer[12];
    int index = sizeof(buffer) - 1;
    buffer[index] = '\0';

    do {
        buffer[--index] = val % 10 + '0';
        val /= 10;
    } while (val > 0);

    if (isNegative) {
        buffer[--index] = '-';
    }

    while (buffer[index] != '\0') {
        console_write(buffer[index++]);
    }
}

#define UI_POS 64-16
#define GROUND 64-20



#endif