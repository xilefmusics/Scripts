#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

int udp_socket_in(int port) {
  struct sockaddr_in si_me;
  int s;
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    return -1;
	si_me.sin_family = AF_INET; // IPv4
	si_me.sin_addr.s_addr = htonl(INADDR_ANY); // Ip-Adresse
	si_me.sin_port = htons(port); // Port
  if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
    return -2;
  return s;
}

/**
 * runs a simple udp server
 *
 * arguments:
 *  - int port to listen on
 *  - char* in_buf
 *  - int in_buf_len
 *  - functionpointer to function who handles the request
 *
 * returns:
 *   0: server was normally shut down
 *  -1: error while creating socket
 *  -2: error while binding socket to port
 *  -3: error while receiving request
 *  -4: error while sending response
 *
 * handler function:
 *  arguments:
 *    - char *in_buffer: buffer with request
 *    - char **out_buffer: always NULL, can be set to an buffer with the response,
 *      if not in buffer will be sent back
 *    - int in_len: length of request
 *  returns:
 *    >0: length of response
 *     0: no response
 *    <0: server will be terminated
 *
 */
int udp_server(int port, char* in_buf, int in_buf_len, int (*handler)(char *in_buf, char **out_buf, int in_len)) {
  struct sockaddr_in si_me, si_other;
  int s, len, slen = sizeof(si_other);
  char *out_buf;
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    return -1;
	si_me.sin_family = AF_INET; // IPv4
	si_me.sin_addr.s_addr = htonl(INADDR_ANY); // Ip-Adresse
	si_me.sin_port = htons(port); // Port
  if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
    return -2;
  while (1) {
    if ((len = recvfrom(s, in_buf, in_buf_len, 0, (struct sockaddr *) &si_other, &slen)) == -1)
      return -3;
    out_buf = NULL;
    len = handler(in_buf, &out_buf, len);
      if (!out_buf)
        out_buf = in_buf;
      if (!len)
        continue;
      if (len < 0)
        break;
    if (sendto(s, out_buf, len, 0, (struct sockaddr*) &si_other, slen) == -1)
      return -4;
  }
  return 0;
}
