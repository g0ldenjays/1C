#include "session.h"

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
		else if (sscanf(buffer, "PLAY %d", &move) == 1) {
			if (move < 1 || move > count_valid_cards(game.player2)) {
				if (send_line(client_fd, DARK_RED "INVALID" RESET "\n") < 0)
					return 1;
				continue;
			}

			if (!validate_move(&game, game.player2[move - 1].card)) {
				if (send_line(client_fd, DARK_RED "INVALID" RESET "\n") < 0)
					return 1;
				continue;
			}

			skip = play_card_logic(&game, move, game.player2);

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

				printf("%s\n", buffer);
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
				snprintf(command, sizeof(command), "PLAY %d", move);

				if (send_line(sockfd, command) < 0) {
					printf(DARK_RED "No se pudo enviar PLAY.");
					printf(RESET"\n");
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