#include "protocol.h"

/**
 * @brief Convierte un color a una cadena de caracteres
 * 
 * @param color Color a convertir
 * @return char* String de caracteres coloreados
 */
char *color_to_net(Color color)
{
	switch (color) {
		case COLOR_RED: return "RED";
		case COLOR_YELLOW: return "YELLOW";
		case COLOR_GREEN: return "GREEN";
		case COLOR_BLUE: return "BLUE";
		default: return "NONE";
	}
}

/**
 * @brief Convierte un tipo de carta a una cadena de caracteres
 * 
 * @param type Tipo de carta a convertir
 * @return char* String de caracteres
 */
char *type_to_net(CardType type)
{
	switch (type) {
		case CARD_NUMBER: return "NUMBER";
		case CARD_SKIP: return "SKIP";
		case CARD_REVERSE: return "REVERSE";
		case CARD_DRAW_TWO: return "DRAW_TWO";
		case CARD_WILD: return "WILD";
		case CARD_WILD_DRAW_FOUR: return "WILD_DRAW_FOUR";
		case CARD_WILD_TOTAL: return "WILD_TOTAL";
		default: return "UNKNOWN";
	}
}

/**
 * @brief Empaqueta y envía al cliente el estado visible del juego.
 * Envía: TOP, COLOR, cantidad de cartas en mano, los datos de cada carta válida, y marca el final con "END".
 * 
 * @param sockfd Socket de comunicación
 * @param game Estado del juego
 * @param hand Mano del jugador
 * @return int Cantidad de cartas enviadas, o -1 si falla
 */
int send_state_to_client(int sockfd, GameState *game, HandSlot hand[])
{
	char line[128];
	int sent_cards = 0;

	if (send_line(sockfd, "STATE") < 0)
		return -1;

	if (game->topCard.type == CARD_NUMBER) {
		snprintf(line, sizeof(line), "TOP %s %s %d",
			type_to_net(game->topCard.type),
			color_to_net(game->topCard.color),
			game->topCard.number);
	} else {
		snprintf(line, sizeof(line), "TOP %s %s",
			type_to_net(game->topCard.type),
			color_to_net(game->topCard.color));
	}

	if (send_line(sockfd, line) < 0)
		return -1;

	snprintf(line, sizeof(line), "COLOR %s", color_to_net(game->currentColor));
	if (send_line(sockfd, line) < 0)
		return -1;

	snprintf(line, sizeof(line), "HAND_COUNT %d", count_valid_cards(hand));
	if (send_line(sockfd, line) < 0)
		return -1;

	
	for (int i = 0; i < MAX_HAND_SIZE; i++) {
		if (!hand[i].valid)
			continue;

		if (hand[i].card.type == CARD_NUMBER) {
			snprintf(line, sizeof(line),"\t %d) %s %s %d", i + 1,
				color_to_net(hand[i].card.color),
				type_to_net(hand[i].card.type),
				hand[i].card.number);
		} else {
			snprintf(line, sizeof(line), "\t %d) %s %s", i + 1,
				color_to_net(hand[i].card.color),
				type_to_net(hand[i].card.type));
		}

		if (send_line(sockfd, line) < 0)
			return -1;

		sent_cards++;
	}

	if (send_line(sockfd, "END") < 0)
		return -1;

	return sent_cards;
}

/**
 * @brief Envía al cliente que es su turno.
 * 
 * @param sockfd Socket de comunicacion
 * @return int 0 si éxito, -1 si falla (lo delega a send_line)
 */
int send_your_turn(int sockfd)
{
	return send_line(sockfd, "YOUR_TURN");
}

/**
 * @brief Envía al cliente que espere.
 * 
 * @param sockfd Socket de comunicacion
 * @return int 0 si éxito, -1 si falla (lo delega a send_line)
 */
int send_wait(int sockfd)
{
	return send_line(sockfd, "WAIT");
}

/**
 * @brief Recibe un comando de texto del cliente.
 * Delega a recv_line() para leer hasta '\n' o max_len.
 * 
 * @param sockfd Socket de donde recibir
 * @param buffer Donde almacenar el comando
 * @param max_len Tamaño máximo del buffer
 * @return int 0 si éxito, -1 si falla o se corta la conexión
 */
int recv_command(int sockfd, char *buffer, size_t max_len)
{
	return recv_line(sockfd, buffer, max_len);
}

/**
 * @brief Convierte un string de caracteres a un color
 * 
 * @param text Texto a convertir
 * @return Color Texto convertido
 */
Color color_from_net(char *text)
{
	if (strcmp(text, "RED") == 0) return COLOR_RED;
	if (strcmp(text, "YELLOW") == 0) return COLOR_YELLOW;
	if (strcmp(text, "GREEN") == 0) return COLOR_GREEN;
	if (strcmp(text, "BLUE") == 0) return COLOR_BLUE;
	return COLOR_NONE;
}

/**
 * @brief Convierte un string de caracteres a un tipo de carta
 * 
 * @param text Texto a convertir
 * @return CardType Tipo de carta convertido
 */
CardType type_from_net(char *text)
{
	if (strcmp(text, "NUMBER") == 0) return CARD_NUMBER;
	if (strcmp(text, "SKIP") == 0) return CARD_SKIP;
	if (strcmp(text, "REVERSE") == 0) return CARD_REVERSE;
	if (strcmp(text, "DRAW_TWO") == 0) return CARD_DRAW_TWO;
	if (strcmp(text, "WILD") == 0) return CARD_WILD;
	if (strcmp(text, "WILD_DRAW_FOUR") == 0) return CARD_WILD_DRAW_FOUR;
	if (strcmp(text, "WILD_TOTAL") == 0) return CARD_WILD_TOTAL;
	return CARD_NUMBER;
}