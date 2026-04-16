#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdbool.h>
#include "cards.h"

typedef struct {
	Deck deck;
	HandSlot player1[MAX_HAND_SIZE];
	HandSlot player2[MAX_HAND_SIZE];
	Card topCard;
	Color currentColor;
	int currentPlayer;
} GameState;

void init_game(GameState *game);
bool validate_move(GameState *game, Card move);
int count_valid_cards(HandSlot hand[]);
Card draw_card(GameState *game, HandSlot hand[]);
void deal_cards(Deck *deck, HandSlot hand1[], HandSlot hand2[], int hand_size);
void play_color_change(GameState *game);
int verify_win(GameState *game);

#endif