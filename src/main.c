#include <stdio.h>
#include "cards.h"
#include "utilities.h"

int main(void) {
    Deck deck;
    Card player1[7];
    Card player2[7];

    srand(time(NULL));

    generate_deck(&deck);
    shuffle_deck(&deck);
    deal_cards(&deck, player1, player2, 7);

    printf("Cartas restantes en deck: %d\n\n", deck.size);

	printf("Cartas del jugador 1:\n");
	print_hand(player1, 7);
	printf("\n\n\nCartas del jugador 2:\n");
	print_hand(player2, 7);

    return 0;
}
