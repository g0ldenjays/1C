#include "utilities.h"
#include "cards.h"
#include "cards.h"

/**
 * @brief Setea una carta especifica
 * 
 * @param card Carta en especifico a modificar
 * @param type Tipo de la carta
 * @param color Color de la carta
 * @param number Numero de la carta
 */
void set_card(Card *card, CardType type, Color color, int number) 
{
	card->type = type;
	card->color = color;
	card->number = number;
}

/**
 * @brief Intercambia dos cartas
 * 
 * @param card1 Carta 1
 * @param card2 Carta 2
 */
static void swap_cards(Card *card1, Card *card2) 
{
	Card temp = *card1;
	*card1 = *card2;
	*card2 = temp;
}

static void swap_handslots(HandSlot *a, HandSlot *b) {
	HandSlot temp = *a;
	*a = *b;
	*b = temp;
}

/**
 * @brief Genera el mazo completo de cartas
 * 
 * @param deck Mazo a generar
 */
void generate_deck(Deck *deck) 
{
	int i = 0;

	for (int color = COLOR_RED; color <= COLOR_BLUE; color++) {
		
		// Cartas numericas
		set_card(&deck->cards[i], CARD_NUMBER, color, 0);
		i++;

		// x2 Cartas numericas
		for (int number = 1; number <= 9; number++) {
			for (int copy = 0; copy < 2; copy++) {
				set_card(&deck->cards[i], CARD_NUMBER, color, number);
				i++;
			}
		}

		// Dos acciones por color
		for (int copy = 0; copy < 2; copy++) {
			set_card(&deck->cards[i], CARD_SKIP, color, -1);
			i++;
		}

		for (int copy = 0; copy < 2; copy++) {
			set_card(&deck->cards[i], CARD_REVERSE, color, -1);
			i++;
		}

		for (int copy = 0; copy < 2; copy++) {
			set_card(&deck->cards[i], CARD_DRAW_TWO, color, -1);
			i++;
		}
	}

	// Comodines: Cambio color, +4 y comodin total
	for (int copy = 0; copy < 4; copy++) {
		set_card(&deck->cards[i], CARD_WILD, COLOR_NONE, -1);
		i++;
	}

	for (int copy = 0; copy < 4; copy++) {
		set_card(&deck->cards[i], CARD_WILD_DRAW_FOUR, COLOR_NONE, -1);
		i++;
	}

	set_card(&deck->cards[i], CARD_WILD_TOTAL, COLOR_NONE, -1);
	i++;

	deck->size = i;
}

/**
 * @brief Mezcla el mazo de cartas con Fisher Yates
 * 
 * @param deck Mazo a mezclar
 */
void shuffle_deck(Deck *deck) 
{
	int j;
	for (int i = deck->size - 1; i > 0; i--) {
		j = rand() % (i + 1);
		swap_cards(&deck->cards[i], &deck->cards[j]);
	}
}

/**
 * @brief Imprime el mazo completo de cartas
 * 
 * @param deck Mazo a imprimir
 */
void print_deck(Deck *deck) 
{
	for (int i = 0; i < deck->size; i++) {
		printf("%d ", deck->cards[i].number);
		printf(deck->cards[i].color == COLOR_RED ? "Red" : deck->cards[i].color == COLOR_YELLOW ? "Yellow" : deck->cards[i].color == COLOR_GREEN ? "Green" : deck->cards[i].color == COLOR_BLUE ? "Blue" : "None");
		if (deck->cards[i].type != CARD_NUMBER) {
			printf(" %s ", deck->cards[i].type == CARD_SKIP ? "Skip" : deck->cards[i].type == CARD_REVERSE ? "Reverse" : deck->cards[i].type == CARD_DRAW_TWO ? "Draw Two" : deck->cards[i].type == CARD_WILD ? "Wild" : deck->cards[i].type == CARD_WILD_DRAW_FOUR ? "Wild Draw Four" : "Wild Total");
		}
		printf("\n");
	}
}


/**
 * @brief Imprime las manos
 * 
 * @param hand Mano a imprimir
 * @param hand_size Tamanho de la mano
 */
void print_hand_turn(HandSlot hand[])
{
	for (int i = 0; i < MAX_HAND_SIZE; i++) {
		printf("   %d. ", i + 1);
		print_card(hand[i].card);
		printf("\n");
		if (!hand[i+1].valid) {
			break;
		}
	}
	printf("\n");
}

/**
 * @brief Imprime una carta
 * 
 * @param card Carta a imprimir
 */
void print_card(Card card)
{
	if (card.number != -1) {
		printf("%d ", card.number);
	}
	printf(card.color == COLOR_RED ? "Red" : card.color == COLOR_YELLOW ? "Yellow" : card.color == COLOR_GREEN ? "Green" : card.color == COLOR_BLUE ? "Blue" : "None");
	if (card.type != CARD_NUMBER) {
		printf(" %s ", card.type == CARD_SKIP ? "Skip" : card.type == CARD_REVERSE ? "Reverse" : card.type == CARD_DRAW_TWO ? "Draw Two" : card.type == CARD_WILD ? "Wild" : card.type == CARD_WILD_DRAW_FOUR ? "Wild Draw Four" : "Wild Total");
	}
}

//sorteamos manos por campo .valid == true
void insertion_sort_hand(HandSlot *hand, int size) {
	for (int i = 1; i < size; i++) {
		for (int j = i; j > 0; j--) {
			if (hand[j].valid > hand[j - 1].valid) {
				swap_handslots(&hand[j], &hand[j - 1]);
			}
		}
	}
}

