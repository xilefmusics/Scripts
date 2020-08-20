#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/soclib.h"

#define BUFLEN 16384

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

  int len = udp_request(port, ipv4, msg, strlen(msg)+1, buf, BUFLEN, 500000);
  printf("Received %d Bytes\n%s\n", len, buf);
  return 0;
}
