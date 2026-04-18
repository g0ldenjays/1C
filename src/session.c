#include "session.h"
#include "cards.h"
#include "client_ui.h"

/**
 * @brief Ejecuta el host.
 * Crea el servidor TCP, espera cliente, acepta conexion e inicia el juego.
 * Le manda el estado del juego al cliente, recibe el comando del cliente y ejecuta la accion.
 * 
 */
int run_host()
{
	int server_fd;
	int client_fd;
	GameState game;
	char buffer[256];
	int move;
	bool skip;
	Color chosen_color;
	char color_text[32];

	server_fd = create_server_socket(8080);
	if (server_fd < 0)
		return 1;

	printf(YELLOW "Esperando cliente...\n");
	printf(RESET"\n");
	client_fd = accept_client(server_fd);
	if (client_fd < 0)
		return 1;

	printf(LIGHT_GREEN "Cliente conectado.\n");
	printf(RESET"\n");

	init_game(&game);

	while (1) {
		if (send_state_to_client(client_fd, &game, game.player2) < 0)
			return 1;

		if (send_your_turn(client_fd) < 0)
			return 1;

		if (recv_command(client_fd, buffer, sizeof(buffer)) < 0)
			return 1;

		printf("Cliente envio: %s\n", buffer);

		if (strcmp(buffer, "DRAW") == 0) {
			draw_card(&game, game.player2);

			if (send_line(client_fd, "OK") < 0)
				return 1;
		}
		else if (sscanf(buffer, "PLAY %d %31s", &move, color_text) == 2 ||
				sscanf(buffer, "PLAY %d", &move) == 1) {

			chosen_color = COLOR_NONE;

			if (strstr(buffer, "PLAY") == buffer && sscanf(buffer, "PLAY %d %31s", &move, color_text) == 2) {
				chosen_color = color_from_net(color_text);
			}

			if (move < 1 || move > count_valid_cards(game.player2)) {
				if (send_line(client_fd, "INVALID") < 0)
					return 1;
				continue;
			}

			if (!validate_move(&game, game.player2[move - 1].card)) {
				if (send_line(client_fd, "INVALID") < 0)
					return 1;
				continue;
			}

			if ((game.player2[move - 1].card.type == CARD_WILD ||
				game.player2[move - 1].card.type == CARD_WILD_DRAW_FOUR ||
				game.player2[move - 1].card.type == CARD_WILD_TOTAL) &&
				chosen_color == COLOR_NONE) {
				if (send_line(client_fd, "INVALID") < 0)
					return 1;
				continue;
			}

			skip = play_card_logic(&game, move, game.player2, chosen_color);
			(void)skip;

			if (send_line(client_fd, "OK") < 0)
				return 1;

			if (verify_win(&game) == 2) {
				if (send_line(client_fd, "WIN 2") < 0)
					return 1;
				break;
			}
		}
		else {
			if (send_line(client_fd, DARK_RED "INVALID" RESET "\n") < 0)
				return 1;
		}
	}

	close_socket(client_fd);
	close_socket(server_fd);
	return 0;
}

/**
 * @brief Ejecuta el cliente.
 * Se conecta al servidor, recibe estado del juego, envia comandos por inputs y lo envia al servidor.
 * 
 * @param ip 
 * @return int 
 */
int run_client(char *ip)
{
	int sockfd;
	char buffer[256];
	int move;
	int result;
	char command[64];
	Card client_cards[MAX_HAND_SIZE];
	int client_card_count = 0;
	char type_text[32];
	char color_text[32];
	int number;

	sockfd = connect_to_server(ip, 8080);
	if (sockfd < 0) {
		return 1;
	}

	while (1) {
		if (recv_line(sockfd, buffer, sizeof(buffer)) < 0) {
			printf(DARK_RED "Conexion cerrada por el host.");
			printf(RESET"\n");
			close_socket(sockfd);
			return 1;
		}

		if (strcmp(buffer, "STATE") == 0) {
			client_card_count = 0;
			printf("=== ESTADO DEL JUEGO ===\n");

			while (1) {
				if (recv_line(sockfd, buffer, sizeof(buffer)) < 0) {
					printf(DARK_RED "Conexion cerrada por el host.");
					printf(RESET"\n");
					close_socket(sockfd);
					return 1;
				}

				if (strcmp(buffer, "END") == 0) {
					break;
				}

				if (strcmp(buffer, "OK") == 0) {
					printf(LIGHT_GREEN "Movimiento aceptado.");
					printf(RESET"\n");
					continue;
				}

				if (strcmp(buffer, "INVALID") == 0) {
					printf(LIGHT_RED "Movimiento invalido.");
					printf(RESET"\n");
					continue;
				}

				if (strncmp(buffer, "WIN", 3) == 0) {
					printf("%s\n", buffer);
					close_socket(sockfd);
					return 0;
				}

				// Parse TOP para mostrar como arte ASCII
				if (strncmp(buffer, "TOP", 3) == 0) {
					char top_type_str[32], top_color_str[32];
					int top_number = -1;
					if (sscanf(buffer, "TOP %31s %31s %d", top_type_str, top_color_str, &top_number) >= 2) {
						Card top;
						top.type = type_from_net(top_type_str);
						top.color = color_from_net(top_color_str);
						top.number = top_number;
						printf("\n--- CARTA EN MESA ---\n");
						print_card_ascii(top);
					}
					continue;
				}

				// Para cartas con número
				if (sscanf(buffer, " %*d) %31s %31s %d", color_text, type_text, &number) == 3) {
					client_cards[client_card_count].type = type_from_net(type_text);                                                      
					client_cards[client_card_count].color = color_from_net(color_text);
					client_cards[client_card_count].number = number;
					client_card_count++;
					print_remote_card(client_cards[client_card_count - 1], client_card_count);
				}
				// Para cartas sin número
				else if (sscanf(buffer," %*d) %31s %31s", color_text, type_text) == 2) {
					client_cards[client_card_count].type = type_from_net(type_text);
					client_cards[client_card_count].color = color_from_net(color_text);
					client_cards[client_card_count].number = -1;
					client_card_count++;
					print_remote_card(client_cards[client_card_count - 1], client_card_count);
				}

				//printf("%s\n", buffer);
			}

			continue;
		}

		if (strcmp(buffer, "YOUR_TURN") == 0) {
			printf("\nTe toca jugar.\n");
			printf("Ingresa el numero de carta a jugar, o 0 para robar: ");

			result = scanf("%d", &move);
			while (result != 1 || move < 0) {
				if (result != 1) {
					printf(DARK_RED "Entrada invalida. Debes ingresar un numero.");
					printf(RESET"\n");
					while (getchar() != '\n');
				} else {
					printf(DARK_RED "El numero debe ser 0 o mayor.");
					printf(RESET"\n");
				}

				printf("Ingresa el numero de carta a jugar, o 0 para robar: ");
				result = scanf("%d", &move);
			}

			if (move == 0) {
				if (send_line(sockfd, "DRAW") < 0) {
					printf(DARK_RED "No se pudo enviar DRAW.");
					printf(RESET"\n");
					close_socket(sockfd);
					return 1;
				}
			} else {
				if (move > client_card_count) {
					printf(DARK_RED "Indice de carta invalido.");
					printf(RESET "\n");
					continue;
				}

				if (client_cards[move - 1].type == CARD_WILD ||
					client_cards[move - 1].type == CARD_WILD_DRAW_FOUR ||
					client_cards[move - 1].type == CARD_WILD_TOTAL) {

					int color_choice;
					const char *chosen_color_text = "NONE";

					printf("Elige color: 1) RED  2) YELLOW  3) GREEN  4) BLUE\n");
					result = scanf("%d", &color_choice);

					while (result != 1 || color_choice < 1 || color_choice > 4) {
						if (result != 1) {
							printf("Entrada invalida. Debes ingresar un numero.\n");
							while (getchar() != '\n');
						} else {
							printf("El numero debe estar entre 1 y 4.\n");
						}

						printf("Elige color: 1) RED  2) YELLOW  3) GREEN  4) BLUE\n");
						result = scanf("%d", &color_choice);
					}

					switch (color_choice) {
						case 1: chosen_color_text = "RED"; break;
						case 2: chosen_color_text = "YELLOW"; break;
						case 3: chosen_color_text = "GREEN"; break;
						case 4: chosen_color_text = "BLUE"; break;
					}

					snprintf(command, sizeof(command), "PLAY %d %s", move, chosen_color_text);
				} else {
					snprintf(command, sizeof(command), "PLAY %d", move);
				}

				if (send_line(sockfd, command) < 0) {
					printf(DARK_RED "No se pudo enviar PLAY.");
					printf(RESET "\n");
					close_socket(sockfd);
					return 1;
				}
			}

			continue;
		}

		if (strcmp(buffer, "WAIT") == 0) {
			printf("\nEs turno del otro jugador.\n");
			continue;
		}

		printf("%s\n", buffer);
	}

	close_socket(sockfd);
	return 0;
}