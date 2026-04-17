#include <stdio.h>
#include "game.h"
#include "utilities.h"

int main() {
	GameState game;
	Card drawn;
	int move, result;
	bool skip = false;

	init_game(&game);

	// Bucle principal de juego
	while (true) {
		
		HandSlot *currentHand;
		HandSlot *opponentHand;  

		if (!skip) {
			game.currentPlayer = (game.currentPlayer == 1) ? 2 : 1;
			currentHand = (game.currentPlayer == 1) ? game.player1 : game.player2;
			opponentHand = (game.currentPlayer == 1) ? game.player2 : game.player1;  
		}
		skip = false;
		
		// Prints del estado de juego
		printf("Cartas restantes en deck: %d\n\n", game.deck.size);

		printf(DARK_YELLOW "Carta en mesa: " YELLOW);
		print_card(game.topCard);
		printf(RESET "\n");
		printf(DARK_GRAY "\t Color en mesa: " RESET);
		switch (game.topCard.color) {
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
				printf("NADA\n");
				break;
		}
		printf(RESET);


		printf("Jugador %d, te toca jugar, estas son tus cartas:\n", game.currentPlayer);
		insertion_sort_hand(currentHand, MAX_HAND_SIZE);
		print_hand_turn(currentHand);

		printf("Ingrese el numero de la carta que quieres jugar, o 0 para sacar una carta: ");

		// Se ingresa la carta
		result = scanf("%d", &move);
		while (result != 1 || move <= 0 || move > count_valid_cards(currentHand)) {
			if (result != 1) {
				printf("Entrada invalida. Debes ingresar un numero.\n");
				while (getchar() != '\n');
			}
			else if (move == 0) {
				drawn = draw_card(&game, currentHand);
				printf("Sacaste la carta:  ");
				print_card(drawn);
				printf("\n");
				printf("Pierdes tu turno\n");
				break;
			}
			else {
				printf("El numero debe estar entre 1 y %d, o 0 para sacar una carta\n",
					count_valid_cards(currentHand));
			}

			printf("Ingrese el numero de la carta que quieres jugar: ");
			result = scanf("%d", &move);
		}

		if (move != 0) {
			if (!validate_move(&game, currentHand[move - 1].card)) {
				printf("Estás intentando jugar: ");
				print_card(currentHand[move - 1].card);
				printf("\n");
				printf(DARK_RED "\nCarta inválida. \n" RESET);
				continue;
			}
		}

		if (move != 0) {
			currentHand[move - 1].valid = false;
			stack_bin(&game);
			game.topCard = currentHand[move - 1].card;
			game.currentColor = game.topCard.color;
			insertion_sort_hand(currentHand, MAX_HAND_SIZE);
		
			switch (game.topCard.type) {
				case CARD_WILD:
					play_color_change(&game);
					break;
				case CARD_WILD_DRAW_FOUR:
					play_draw_four(&game);
					print_hand_turn(opponentHand);
					break;
				case CARD_DRAW_TWO:
					play_draw_two(&game);
					print_hand_turn(opponentHand);
					break;
				case CARD_SKIP:
				case CARD_REVERSE:
					skip = true;
					break;
				default:
					break;
			}
		}

		if (verify_win(&game) != 0) {
			printf(BG_GREEN "El jugador %d gano!!!" RESET "\n", verify_win(&game));
			break;
		}

	}




	printf(DARK_GREEN "\nJuego terminado exitosamente\n" RESET);
	return 0;
}
