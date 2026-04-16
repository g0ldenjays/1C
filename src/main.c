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

		printf("Te toca jugar, estas son tus cartas:\n");
		print_hand_turn(game.player1);

		printf("Ingrese el numero de la carta que quieres jugar, o 0 para sacar una carta: ");

		// Se ingresa la carta
		result = scanf("%d", &move);
		while (result != 1 || move < 0 || move > count_valid_cards(game.player1)) {
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
				printf(DARK_RED "\nERROR\n" RESET);
				break;
			}
		}

		if (move != 0) {
			game.player1[move - 1].valid = false;
			game.topCard =  game.player1[move - 1].card;
			insertion_sort_hand(game.player1, MAX_HAND_SIZE);
		}
	}

	printf(DARK_GREEN "\nJuego terminado exitosamente\n" RESET);
	return 0;
}
