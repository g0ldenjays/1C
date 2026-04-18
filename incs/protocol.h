#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include "game.h"
#include "net.h"

char *color_to_net(Color color);
char *type_to_net(CardType type);

int send_state_to_client(int sockfd, GameState *game, HandSlot hand[]);
int send_your_turn(int sockfd);
int send_wait(int sockfd);

int recv_command(int sockfd, char *buffer, size_t max_len);

Color color_from_net(char *text);
CardType type_from_net(char *text);

#endif