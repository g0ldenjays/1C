#ifndef NET_H
#define NET_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "colors.h"

int create_server_socket(int port);
int accept_client(int server_fd);
int connect_to_server(char *ip, int port);

int send_all(int sockfd, char *buffer, size_t length);
int send_line(int sockfd, char *line);
int recv_line(int sockfd, char *buffer, size_t max_len);

void close_socket(int sockfd);

#endif