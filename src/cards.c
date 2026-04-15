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
static void set_card(Card *card, CardType type, Color color, int number) 
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

Card draw_card(Deck *deck) {
    Card empty_card;

    if (deck->size <= 0) {
        set_card(&empty_card, CARD_WILD_TOTAL, COLOR_NONE, -1);
        return empty_card;
    }

    deck->size--;
    return deck->cards[deck->size];
}

void deal_cards(Deck *deck, Card hand1[], Card hand2[], int hand_size)
{
    for (int i = 0; i < hand_size; i++) {
        hand1[i] = draw_card(deck);
        hand2[i] = draw_card(deck);
    }
}

void print_hand(Card hand[], int hand_size)
{
    for (int i = 0; i < hand_size; i++) {
        printf("%d ", hand[i].number);
        printf(hand[i].color == COLOR_RED ? "Red" : hand[i].color == COLOR_YELLOW ? "Yellow" : hand[i].color == COLOR_GREEN ? "Green" : hand[i].color == COLOR_BLUE ? "Blue" : "None");
        if (hand[i].type != CARD_NUMBER) {
            printf(" %s ", hand[i].type == CARD_SKIP ? "Skip" : hand[i].type == CARD_REVERSE ? "Reverse" : hand[i].type == CARD_DRAW_TWO ? "Draw Two" : hand[i].type == CARD_WILD ? "Wild" : hand[i].type == CARD_WILD_DRAW_FOUR ? "Wild Draw Four" : "Wild Total");
        }
        printf("\n");
    }
}