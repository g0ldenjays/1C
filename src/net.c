#include "net.h"

/**
 * @brief Crea un socket de servidor y lo deja como escuchable
 * 
 * @param port Puerto del socket
 * @return int Socket de servidor
 */
int create_server_socket(int port)
{
	// Direccion de socket
	struct sockaddr_in addr;

	int server_fd; // socket de escucha
	int opt = 1;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);	 // socket(): crea un socket  |  AF_INET: IPv4  |  SOCK_STREAM: TCP  |  0: sin protocolo
	
	if (server_fd < 0) {
		printf(BG_RED "\nerror: socket");
		printf(RESET"\n");
		return -1;
	}

	// setsockopt(): configura las opciones del socket
	// SO_REUSEADDR: permite que el socket se use más de una vez
	// SOL_SOCKET: nivel de opciones a nivel de socket, indica que las opciones aplican al socket en general
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		printf(BG_RED "\nerror: setsockopt");
		printf(RESET"\n");
		close(server_fd);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));			 // memset(): rellena un buffer de memoria  |  &addr: dirección de socket
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;		 // INADDR_ANY: acepta cualquier dirección
	addr.sin_port = htons(port);			 // htons(): convierte un entero en un short

	
	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {		 // bind(): asocia el socket con la dirección dada
		printf(BG_RED "\nerror: bind");
		printf(RESET"\n");
		close(server_fd);
		return -1;
	}

	
	if (listen(server_fd, 1) < 0) {			// listen(): configura el socket para que acepte conexiones 
		printf(BG_RED "\nerror: listen");
		printf(RESET"\n");
		close(server_fd);
		return -1;
	}
	// A partir de aqui el host queda esperando

	return server_fd;
}

/**
 * @brief Acepta una conexión del cliente
 * 
 * @param server_fd Socket del servidor
 * @return int Socket del cliente
 */
int accept_client(int server_fd)
{
	int client_fd; // socket de conexión

	
	client_fd = accept(server_fd, NULL, NULL);		// accept(): acepta una conexión especifica de un socket
	if (client_fd < 0) {
		printf(BG_RED "\nerror: accept\n" RESET);
		return -1;
	}

	return client_fd;
}

/**
 * @brief Conecta al servidor
 *
 * @param ip Dirección IP del servidor
 * @param port Puerto del servidor
 * @return int File descriptor del socket conectado, o -1 si falla
 */
int connect_to_server(char *ip, int port)
{
	int sockfd; // File descriptor del socket de cliente
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf(BG_RED "\nerror: socket\n" RESET);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	
	if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {		// inet_pton(): convierte una dirección IP en un entero
		printf(BG_RED "\nerror: inet_pton\n" RESET);
		close(sockfd);
		return -1;
	}

	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {		// connect(): intenta abrir la conexion TCP con el servidor
		printf(BG_RED "\nerror: connect\n" RESET);
		close(sockfd);
		return -1;
	}

	return sockfd;
}

/**
 * @brief Envía todo el buffer con un loop while de send()
 *
 * Envía la cantidad exacta de bytes especificada en length. Si send() envía menos de lo solicitado, reintenta el envío del resto hasta completar.
 * 
 * @param sockfd Socket donde enviar
 * @param buffer Datos a enviar
 * @param length Cantidad de bytes a enviar
 * @return int 0 si éxito, -1 si falla
 */
int send_all(int sockfd, char *buffer, size_t length)
{
	size_t total = 0;
	ssize_t sent;

	while (total < length) {
		sent = send(sockfd, buffer + total, length - total, 0); 	// send(): envía un buffer de datos
		if (sent <= 0) {
			printf(BG_RED "\nerror: send\n" RESET);
			return -1;
		}
		total += (size_t)sent;
	}

	return 0;
}

/**
 * @brief Envía una línea de texto seguida de '\n'
 * 
 * @param sockfd Socket donde enviar
 * @param line Texto a enviar (sin '\n')
 * @return int 0 si éxito, -1 si falla
 */
int send_line(int sockfd, char *line)
{
	size_t len = strlen(line);

	if (send_all(sockfd, line, len) < 0) {
		return -1;
	}

	if (send_all(sockfd, "\n", 1) < 0) {
		return -1;
	}

	return 0;
}

/**
 * @brief Recibe una línea de texto hasta encontrar '\n' o max_len y almacena en buffer
 * 
 * @param sockfd Socket de donde recibir
 * @param buffer Donde almacenar la línea leída
 * @param max_len Tamaño máximo del buffer
 * @return int 0 si éxito, -1 si falla o se corta la conexión
 */
int recv_line(int sockfd, char *buffer, size_t max_len)
{
	size_t i = 0;
	char c;
	ssize_t n;

	if (max_len == 0) {
		return -1;
	}

	while (i < max_len - 1) {
		n = recv(sockfd, &c, 1, 0); 		// recv(): recibe un buffer de datos
		if (n <= 0) {
			return -1;
		}

		if (c == '\n') {
			break;
		}

		buffer[i++] = c;
	}

	buffer[i] = '\0';
	return 0;
}

void close_socket(int sockfd)
{
	close(sockfd);		// close(): cierra un socket
}