#include "game.h"
#include "utilities.h"

/**
 * @brief Inicializa el estado del juego
 * 
 * @param game Variable de estado del juego
 * @param player1 mano de jugador 1
 * @param player2 mano de jugador 2
 */
void init_game(GameState *game) {
	
	Deck deck;

	srand(time(NULL));
	generate_deck(&deck);
	shuffle_deck(&deck);

	game->deck = deck;

	deal_cards(&game->deck, game->player1, game->player2, 7);

	game->topCard = draw_card(&game->deck);
	game->currentColor = game->topCard.color;
	game->currentPlayer = 0;

	return;
}

bool validate_move(GameState *game, Card move) {

	if ((move.type == CARD_WILD) || (move.type == CARD_WILD_DRAW_FOUR) || (move.type == CARD_WILD_TOTAL)) {
		return true;
	}

	if (move.color == game->currentColor) {
		return true;
	}

	if (((move.type == CARD_NUMBER) && (game->topCard.type == CARD_NUMBER)) && (game->topCard.number == move.number)) {
		return true;
	}

	if (((move.type != CARD_NUMBER) && (game->topCard.type != CARD_NUMBER)) && (move.type == game->topCard.type)) {
		return true;
	}

	return false;

}
