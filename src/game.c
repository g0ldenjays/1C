#include "game.h"

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

	if (game->deck.size == 0) {
		bin_to_deck(game);
	}

    for (int i = 0; i < MAX_HAND_SIZE; i++) {
        if (!hand[i].valid) {
            hand[i].card = draw_card_start(&game->deck);
            hand[i].valid = true;
            return hand[i].card;
        }
    }

	return hand[0].card;
}

/**
 * @brief Reparte las cartas del mazo en dos manos
 * 
 * @param deck Mazo
 * @param hand1 Mano 1
 * @param hand2 Mano 2
 * @param hand_size Tamanho de mano
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
 * @brief Guarda las cartas pasadas en el mazo de la bin para poder volverlas
 * 
 * @param game Juego
 * @param bin Mazo de reserva
 */
void stack_bin(GameState *game)
{
	game->bin.cards[game->bin.size] = game->topCard;
	game->bin.size++;
}

void shuffle_bin(GameState *game)
{
	for (int i = game->bin.size - 1; i > 0; i--) {
		swap_cards(&game->bin.cards[i], &game->bin.cards[rand() % (i + 1)]);
	}
}

void bin_to_deck(GameState *game)
{
	if (game->bin.size == 0) {
		return;
	}

	shuffle_bin(game);
	game->deck = game->bin;
	game->bin.size = 0;
}

/**
 * @brief Inicializa el estado del juego
 * 
 * @param game Variable de estado del juego
 * @param player1 mano de jugador 1
 * @param player2 mano de jugador 2
 */
void init_game(GameState *game)
{
	
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

	game->bin.size = 0;

	return;
}

/**
 * @brief Cuenta el numero de cartas validos en una mano
 * 
 * @param hand Mano de jugador
 * @return int Cantidad de cartas validas
 */
int count_valid_cards(HandSlot hand[])
{

	int count = 0;
	for (int i = 0; i < MAX_HAND_SIZE; i++) {
		if (hand[i].valid) {
			count++;
		}
	}

	return count;	
}

/**
 * @brief Valida si un movimiento es valido
 * 
 * @param game Juego
 * @param move carta que se va a jugar
 * @return true Valido
 * @return false No valido
 */
bool validate_move(GameState *game, Card move)
{

	// Comodin
	if ((move.type == CARD_WILD) || (move.type == CARD_WILD_DRAW_FOUR) || (move.type == CARD_WILD_TOTAL)) return true;

	// Mismo color
	if (move.color == game->currentColor) return true;

	// Mismo numero
	if (((move.type == CARD_NUMBER) && (game->topCard.type == CARD_NUMBER)) && (game->topCard.number == move.number)) return true;

	// Mismo tipo
	if (((move.type != CARD_NUMBER) && (game->topCard.type != CARD_NUMBER)) && (move.type == game->topCard.type)) return true;

	// Encadenar +2
	if ((move.type == CARD_DRAW_TWO) && ((move.color == game->currentColor) || (game->topCard.type == CARD_DRAW_TWO))) return true;

	// Saltar
	if ((move.type == CARD_SKIP) && (move.color == game->currentColor)) return true;

	// Invertir
	if ((move.type == CARD_REVERSE) && (move.color == game->currentColor)) return true;

	return false;

}

/**
 * @brief Cambia el color en mesa
 * 
 * @param game Juego
 */
void play_color_change(GameState *game)
{
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

//esta funcion maldita
/**
 * @brief Le da +4 cartas al oponente y cambia de color
 * 
 * @param game Juego
 */
void play_draw_four(GameState *game)
{
	HandSlot *targetHand;

	// Determina jugador afectado
	if (game->currentPlayer == 1) {
		targetHand = game->player2;
	} else {
		targetHand = game->player1;
	}

	for (int i = 0; i < 4; i++) {
		draw_card(game, targetHand);
	}

	//Cambia color
	play_color_change(game);
}

/**
 * @brief Le da +2 cartas al oponente
 * 
 * @param game Juego
 */
void play_draw_two(GameState *game)
{

	HandSlot *targetHand;

	// Determina jugador afectado
	if (game->currentPlayer == 1) {
		targetHand = game->player2;
	} else {
		targetHand = game->player1;
	}

	for (int i = 0; i < 2; i++) {
		draw_card(game, targetHand);
	}

}

/**
 * @brief Cambia el flujo de turnos
 * 
 * @param game Juego
 */
void play_reverse()
{
	// en futura update cuando se añadan mas jugadores xD
}

/**
 * @brief Cambia el orden de los turnos
 * 
 * @param game Juego
 */
void play_change_direction()
{
	// en futura update cuando se añadan mas jugadores xD
}

/**
 * @brief Verifica si el juego ha terminado
 * 
 * @param game Juego
 * @return int Cual jugador gano
 */
int verify_win(GameState *game)
{
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

bool play_card_logic(GameState *game, int move, HandSlot *currentHand, bool skip)
{
	if (move != 0) {
		currentHand[move - 1].valid = false;
		stack_bin(game);
		game->topCard = currentHand[move - 1].card;
		game->currentColor = game->topCard.color;
		insertion_sort_hand(currentHand);
		

		switch (game->topCard.type) {
			case CARD_WILD:
				play_color_change(game);
				break;
			case CARD_WILD_DRAW_FOUR:
				play_draw_four(game);
				return true;
			case CARD_DRAW_TWO:
				play_draw_two(game);
				return true;
			case CARD_SKIP:
			case CARD_REVERSE:
				return true;
			default:
				break;
		}
	}

	return skip;
}