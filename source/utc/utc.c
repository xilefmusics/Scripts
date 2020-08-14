#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/soclib.h"

#define BUFLEN 16384

int s;
uint16_t port = 8080;
uint32_t ipv4 = 0x7F000001;
char *msg = "Hello from Client";
char buf[BUFLEN];

int main(int argc, char *argv[]) {
  if (argc > 1)
    port = atoi(argv[1]);
  if (argc > 2)
    ipv4 = atoipv4(argv[2]);
  if (argc > 3)
    msg = argv[3];

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    return 1;
  printf("Send %d Bytes\n%s\n", strlen(msg)+1, msg);
  int len = udp_request(s, port, ipv4, msg, strlen(msg)+1, buf, BUFLEN);
  printf("Received %d Bytes\n%s\n", len, buf);
  return 0;
}
