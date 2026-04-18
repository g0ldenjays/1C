#include "client_ui.h"

static char *ansi_from_color(Color color)
{
	switch (color) {
		case COLOR_RED: return LIGHT_RED;
		case COLOR_YELLOW: return YELLOW;
		case COLOR_GREEN: return LIGHT_GREEN;
		case COLOR_BLUE: return LIGHT_BLUE;
		default: return RESET;
	}
}

static char *label_from_card(Card card)
{
	switch (card.type) {
		case CARD_NUMBER: return "\b";
		case CARD_SKIP: return "SKIP";
		case CARD_REVERSE: return "REVERSE";
		case CARD_DRAW_TWO: return "DRAW TWO";
		case CARD_WILD: return "COLOR CHANGE";
		case CARD_WILD_DRAW_FOUR: return "DRAW FOUR";
		case CARD_WILD_TOTAL: return "WILD TOTAL";
		default: return "UNKNOWN";
	}
}

void print_remote_card(Card card, int index)
{
	char *ansi = ansi_from_color(card.color);

	printf("         %d) %s", index, ansi);

	if (card.type == CARD_NUMBER) {
		switch (card.color) {
			case COLOR_RED: printf("RED "); break;
			case COLOR_YELLOW: printf("YELLOW "); break;
			case COLOR_GREEN: printf("GREEN "); break;
			case COLOR_BLUE: printf("BLUE "); break;
			default: printf("NONE "); break;
		}
		printf("%d", card.number);
	} else {
		switch (card.color) {
			case COLOR_RED: printf("RED "); break;
			case COLOR_YELLOW: printf("YELLOW "); break;
			case COLOR_GREEN: printf("GREEN "); break;
			case COLOR_BLUE: printf("BLUE "); break;
			default: printf("NONE "); break;
		}
		printf("%s", label_from_card(card));
	}

	printf(RESET "\n");
}