#include "game.h"
#include "utilities.h"


/**
 * @brief Obtiene una carta del mazo, util al inicio del juego, para un robo mas completo usar draw_card()
 * 
 * @param deck Mazo
 * @return Card Ultima carta del mazo
 */
static Card draw_card_start(Deck *deck)
{
    Card empty_card;

    if (deck->size <= 0) {
        set_card(&empty_card, CARD_WILD_TOTAL, COLOR_NONE, -1);
        return empty_card;
    }

    deck->size--;
    return deck->cards[deck->size];
}

Card draw_card(GameState *game, HandSlot hand[])
{
    //insertion_sort_hand(hand, MAX_HAND_SIZE);

    for (int i = 0; i < MAX_HAND_SIZE; i++) {
        if (!hand[i].valid) {
            hand[i].card = draw_card_start(&game->deck);
            hand[i].valid = true;
            return hand[i].card;
        }
    }

	printf("No hay cartas disponibles\n");
	exit(1);
}

/**
 * @brief Reparte las cartas del mazo en dos manos
 * 
 * @param deck Mazo
 * @param hand1 Mano 1
 * @param hand2 Mano 2
 * @param hand_size Tamanho de mano //TODO: No se debe dejar fijo el tamaño de las manos
 */
void deal_cards(Deck *deck, HandSlot hand1[], HandSlot hand2[], int hand_size)
{
	for (int i = 0; i < hand_size; i++) {
		hand1[i].card = draw_card_start(deck);
		hand2[i].card = draw_card_start(deck);

		hand1[i].valid = true;
		hand2[i].valid = true;
	}
}

/**
 * @brief Inicializa el estado del juego
 * 
 * @param game Variable de estado del juego
 * @param player1 mano de jugador 1
 * @param player2 mano de jugador 2
 */
void init_game(GameState *game) {
	
	srand(time(NULL));
	generate_deck(&game->deck);
	shuffle_deck(&game->deck);

	//inicializamos las manos
	for (int i = 0; i < MAX_HAND_SIZE; i++) {
		game->player1[i].valid = false;
		game->player2[i].valid = false;
	}

	deal_cards(&game->deck, game->player1, game->player2, 7);
	game->topCard = draw_card_start(&game->deck);
	game->currentColor = game->topCard.color;
	game->currentPlayer = 0;

	return;
}

int count_valid_cards(HandSlot hand[]) {

	int count = 0;
	for (int i = 0; i < MAX_HAND_SIZE; i++) {
		if (hand[i].valid) {
			count++;
		}
	}

	return count;	
}

bool validate_move(GameState *game, Card move) {

	if ((move.type == CARD_WILD) || (move.type == CARD_WILD_DRAW_FOUR) || (move.type == CARD_WILD_TOTAL)) {
		return true;
	}

	if (move.color == game->topCard.color) {
		return true;
	}

	if (((move.type == CARD_NUMBER) && (game->topCard.type == CARD_NUMBER)) && (game->topCard.number == move.number)) {
		return true;
	}

	if (((move.type != CARD_NUMBER) && (game->topCard.type != CARD_NUMBER)) && (move.type == game->topCard.type)) {
		return true;
	}

	if ((move.type == CARD_DRAW_TWO) && (move.color == game->topCard.color)) {
		return true;
	}

	if ((move.type == CARD_SKIP) && (move.color == game->topCard.color)) {
		return true;
	}

	if ((move.type == CARD_REVERSE) && (move.color == game->topCard.color)) {
		return true;
	}

	return false;

}

void play_color_change(GameState *game) {
	int move, result;
	printf("Elige un color:\n");
	printf(LIGHT_RED "1) ROJO\n");
	printf(YELLOW "2) AMARILLO\n");
	printf(LIGHT_GREEN "3) VERDE\n");
	printf(LIGHT_BLUE "4) AZUL\n");

	while (1) {
		result = scanf("%d", &move);
		if (result != 1) {
			printf("Entrada invalida. Reintenta:\n");
			while (getchar() != '\n');
			continue;
		}

		if (move < 1 || move > 4) {
			printf("Numero invalido. Debe ser 1-4. Reintenta:\n");
			continue;
		}

		game->currentColor = move - 1;
		game->topCard.color = move - 1;
		printf("Color cambiado a: ");
		switch (game->currentColor) {
			case COLOR_RED:
				printf(LIGHT_RED "ROJO\n");	
				break;
			case COLOR_YELLOW:
				printf(YELLOW "AMARILLO\n");
				break;
			case COLOR_GREEN:
				printf(LIGHT_GREEN "VERDE\n");
				break;
			case COLOR_BLUE:
				printf(LIGHT_BLUE "AZUL\n");
				break;
			default:
				printf("%d\n", game->currentColor);
				break;
		}
		break;
	}
}

int verify_win(GameState *game) {
	int count1 = count_valid_cards(game->player1);
	int count2 = count_valid_cards(game->player2);

	if (count1 == 0) {
		return 1;
	}
	
	if (count2 == 0) {
		return 2;
	}

	return 0;
}