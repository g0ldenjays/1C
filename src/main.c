#include <stdio.h>
#include <string.h>

#include "session.h"
#include "game.h"

int main(int argc, char **argv)
{
	if (argc >= 2 && strcmp(argv[1], "host") == 0) {
		return run_host();
	}

	if (argc >= 3 && strcmp(argv[1], "client") == 0) {
		return run_client(argv[2]);
	}

	printf("Uso:\n");
	printf("  ./build/uno host\n");
	printf("  ./build/uno client 127.0.0.1\n");
	return 1;
}