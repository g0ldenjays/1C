#ifndef SESSION_H
#define SESSION_H

#include "colors.h"
#include "net.h"
#include "protocol.h"

int run_host(void);
int run_client(char *ip);

#endif