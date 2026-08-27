#pragma once

typedef enum {
    D4 = 4,
    D6 = 6,
    D8 = 8,
    D10 = 10,
    D12 = 12,
    D20 = 20,
    D100 = 100
} DiceType;


int roll(DiceType dice, int count, int bonus) {
    static unsigned int state = 0xD20D20; 

    int total = bonus;
    for (int i = 0; i < count; ++i) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;

        total += 1 + (state % (unsigned int) dice);
    }
    return total;
}


