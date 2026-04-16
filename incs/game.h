#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdbool.h>
#include "cards.h"

typedef struct {
	Deck deck;
	Card player1[7];
	Card player2[7];
	Card topCard;
	Color currentColor;
	int currentPlayer;
} GameState;

void init_game(GameState *game);
bool validate_move(GameState *game, Card move);

#endif