#include <stdio.h>
#include "game.h"
#include "utilities.h"

int main() {

    GameState game;
    int move;

    init_game(&game);

    printf("Cartas restantes en deck: %d\n\n", game.deck.size);

    printf(DARK_YELLOW "Carta en mesa: " YELLOW);
    print_card(game.topCard);
    printf(RESET "\n");

    printf("Te toca jugar, estas son tus cartas:\n");
    print_hand_turn(game.player1, 7);

    printf("Ingrese el numero de la carta que quieres jugar: ");
    scanf("%d", &move);

    if (validate_move(&game, game.player1[move - 1])) {
        printf(DARK_GREEN "\nJuego terminado exitosamente\n" RESET);
        return 0;
    }

    printf(DARK_RED "\nNo valido\n" RESET);

    return 0;

}
