#ifndef CARDS_H
#define CARDS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
	COLOR_RED,
	COLOR_YELLOW,
	COLOR_GREEN,
	COLOR_BLUE,
	COLOR_NONE
} Color;

typedef enum {
	CARD_NUMBER,
	CARD_SKIP,
	CARD_REVERSE,
	CARD_DRAW_TWO,
	CARD_WILD,
	CARD_WILD_DRAW_FOUR
} CardType;

typedef struct {
	CardType type;
	Color color;
	int number;
} Card;

typedef struct {
    Card *cards;
    int count;
    int capacity;
} Hand;

#endif