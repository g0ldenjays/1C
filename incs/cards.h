// Cartas: 109
// 4 Colores (Red, Yellow, Green, Blue)
// cada color tiene 19 cartas numericas ([0-9]x2 y un cero) y 6 de accion (25 por color)
// Cartas de accion: +2, skip y reverse, cada color tiene 2 de cada accion
// 9 comodines, 4 +4, 4 cambio color y un comodin total (user elije en que carta convertirla)


#ifndef CARDS_H
#define CARDS_H

#define DECK_SIZE 109

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
	CARD_WILD_DRAW_FOUR,
	CARD_WILD_TOTAL
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

typedef struct {
    Card cards[DECK_SIZE];
    int size;
} Deck;

void generate_deck(Deck *deck);
void print_deck(Deck *deck);
void shuffle_deck(Deck *deck);
Card draw_card(Deck *deck);
void deal_cards(Deck *deck, Card hand1[], Card hand2[], int hand_size);
void print_hand(Card hand[], int hand_size);
#endif