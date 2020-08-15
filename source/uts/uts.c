#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/soclib.h"

#define BUFLEN 16384

int port = 8080;
char *msg = "Hello from Server";
char buffer[BUFLEN];

int handler(char *in_buf, char **out_buf, int in_len) {
  printf("Received %d bytes\n%s\n", in_len, in_buf);
  printf("Send %d bytes\n%s\n", strlen(msg)+1, msg);
  *out_buf = msg;
  return strlen(msg)+1;
}

int main(int argc, char *argv[]) {
  if (argc > 1)
    port = atoi(argv[1]);
  if (argc > 2)
    msg = argv[2];
  return udp_server(port, buffer, BUFLEN, handler);
}
