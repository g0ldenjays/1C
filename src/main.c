#include <stdio.h>
#include "game.h"
#include "utilities.h"

int main() {
	GameState game;
	Card drawn;
	int move, result;

	init_game(&game);

	while (true) {
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

		printf("Te toca jugar, estas son tus cartas:\n");
		insertion_sort_hand(game.player1, MAX_HAND_SIZE);
		print_hand_turn(game.player1);

		printf("Ingrese el numero de la carta que quieres jugar, o 0 para sacar una carta: ");

		// Se ingresa la carta
		result = scanf("%d", &move);
		while (result != 1 || move <= 0 || move > count_valid_cards(game.player1)) {
			if (result != 1) {
				printf("Entrada invalida. Debes ingresar un numero.\n");
				while (getchar() != '\n');
			}
			else if (move == 0) {

				drawn = draw_card(&game, game.player1);
				printf("Sacaste la carta:  ");
				print_card(drawn);
				printf("\n");
				break;
			}
			else {
				printf("El numero debe estar entre 1 y %d, o 0 para sacar una carta\n",
					count_valid_cards(game.player1));
			}

			printf("Ingrese el numero de la carta que quieres jugar: ");
			result = scanf("%d", &move);
		}

		if (move != 0) {
			if (!validate_move(&game, game.player1[move - 1].card)) {
				printf("Estás intentando jugar: ");
				print_card(game.player1[move - 1].card);
				printf("\n");
				printf(DARK_RED "\nCarta inválida. \n" RESET);
				continue;
			}
		}

		if (move != 0) {
			game.player1[move - 1].valid = false;
			game.topCard = game.player1[move - 1].card;
			game.currentColor = game.topCard.color;
			insertion_sort_hand(game.player1, MAX_HAND_SIZE);
		
			if (game.topCard.type == CARD_WILD) {
				play_color_change(&game);
			}
		}

		if (verify_win(&game) != 0) {
			printf(BG_GREEN "El jugador %d gano!!!" RESET "\n", verify_win(&game));
		}

	}




	printf(DARK_GREEN "\nJuego terminado exitosamente\n" RESET);
	return 0;
}
