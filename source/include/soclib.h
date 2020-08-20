#ifndef SOCLIB
#define SOCLIB

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <stdio.h>

/**
 * writes uint16 to char*
 */
char *write_uint16(char *ptr, uint16_t value){
  ptr[0] = (char) (value>>8);
  ptr[1] = (char) value;
  return ptr+2;
}

/**
 * writes uint32 to char*
 */
char *write_uint32(char *ptr, uint32_t value){
  ptr[0] = (char) (value>>24);
  ptr[1] = (char) (value>>16);
  ptr[2] = (char) (value>>8);
  ptr[3] = (char) value;
  return ptr+4;
}

/**
 * reads uint16 from char*
 */
char *read_uint16(char *ptr, uint16_t *value) {
  *value = (((uint16_t)(ptr[0]))<<8) | (uint16_t)(ptr[1]);
  return ptr+2;
}

/**
 * reads uint32 from char*
 */
char *read_uint32(char *ptr, uint32_t *value) {
  *value = (((uint32_t)(ptr[0]))<<24) | (((uint32_t)(ptr[1]))<<16) | (((uint32_t)(ptr[2]))<<8) | (uint32_t)(ptr[3]);
  return ptr+2;
}

/**
 * converts a char array into an ipv4 (uint32_t)
 */
uint32_t atoipv4(char *ptr) {
  char b[6];
  uint32_t ipv4;
  sscanf(ptr, "%d.%d.%d.%d", b, b+1, b+2, b+3);
  read_uint32(b, &ipv4);
  return ipv4;
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
int udp_server(uint16_t port, char* in_buf, int in_buf_len, int (*handler)(char *in_buf, char **out_buf, int in_len)) {
  struct sockaddr_in si_me, si_other;
  int s, len, slen = sizeof(si_other);
  char *out_buf;
  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    return -1;
  si_me.sin_family = AF_INET; // IPv4
  si_me.sin_addr.s_addr = htonl(INADDR_ANY); // Ip-Adresse
  si_me.sin_port = htons(port); // Port
  if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
    close(s);
    return -2;
  }
  while (1) {
    if ((len = recvfrom(s, in_buf, in_buf_len, 0, (struct sockaddr *) &si_other, &slen)) == -1) {
      close(s);
      return -3;
    }
    out_buf = NULL;
    len = handler(in_buf, &out_buf, len);
    if (!out_buf)
      out_buf = in_buf;
    if (!len)
      continue;
    if (len < 0)
      break;
    if (sendto(s, out_buf, len, 0, (struct sockaddr*) &si_other, slen) == -1) {
      close(s);
      return -4;
    }
  }
  close(s);
  return 0;
}

/**
 * creates simple udp request
 *
 * arguments:
 *  - int socket
 *  - int port (server)
 *  - int ipv4 (server)
 *  - char *out_buf
 *  - int out_len
 *  - char *in_buf
 *  - int in_buf_len
 *
 *  returns:
 *      -1: error while creating socket
 *      -3: error while receiving response
 *      -4: error while sending request
 *      -8: error while setting timeout
 *    else: len of response
 */
int udp_request(uint16_t port, uint32_t ipv4, char* out_buf, int out_len, char* in_buf, int in_buf_len, int timeout) {
  struct sockaddr_in server_addr;
  struct timeval tv;
  int s, in_len, slen = sizeof(server_addr);
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    return -1;
  tv.tv_sec = 0;
  tv.tv_usec = timeout;
  if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    return -8;
  server_addr.sin_family = AF_INET; // IPv4
  server_addr.sin_addr.s_addr = htonl(ipv4); // Ip-Adresse
  server_addr.sin_port = htons(port); // Port
  if (sendto(s, out_buf, out_len, MSG_CONFIRM, (struct sockaddr*) &server_addr, slen) == -1)
    return -4;
  if ((in_len = recvfrom(s, in_buf, in_buf_len, MSG_WAITALL, (struct sockaddr *) &server_addr, &slen)) == -1)
    return -3;
  return in_len;
}

/**
 * runs a simple tcp server
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
 *  -5: error while listen
 *  -6: error while accepting connection
 *
 * handler function:
 *  arguments:
 *    - char *in_buffer: buffer with request
 *    - char **out_buffers (16): always NULL, can be set to an buffer with the response,
 *      if not in buffer will be sent back
 *    - int in_len: length of request
 *  returns:
 *    >0: length of response
 *     0: no response
 *    <0: server will be terminated
 *
 */
int tcp_server(uint16_t port, char* in_buf, int in_buf_len, int (*handler)(char *in_buf, char **out_bufs, int in_len), int len_queue) {
  struct sockaddr_in si_me, si_other;
  int s, c, len, slen = sizeof(si_other);
  char *out_bufs[16];
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    return -1;
  si_me.sin_family = AF_INET; // IPv4
  si_me.sin_addr.s_addr = INADDR_ANY; // Ip-Adresse
  si_me.sin_port = htons(port); // Port
  if (bind(s, (struct sockaddr *) &si_me, sizeof(si_me)) == -1)
    return -2;
  if ((listen(s, len_queue) == -1))
    return -5;
  while (1) {
    if ((c = accept(s, (struct sockaddr *) &si_other, &slen)) == -1)
      return -3;
    if ((len = recv(c, in_buf, in_buf_len, 0)) == -1)
      return -6;
    for (int i = 0; i < 16; ++i)
      out_bufs[i] = NULL;
    len = handler(in_buf, out_bufs, len);
    if (!out_bufs[0])
      out_bufs[0] = in_buf;
    if (!len)
      continue;
    if (len < 0)
      break;
    for (int i = 0; i < 16; ++i) {
      if (!out_bufs[i])
        break;
      if ((send(c, out_bufs[i], len, 0)) == -1)
        return -4;
    }
    close(c);
  }
  close(s);
  return 0;
}

/**
 * creates simple tcp request
 *
 * arguments:
 *  - int socket
 *  - int port (server)
 *  - int ipv4 (server)
 *  - char *out_buf
 *  - int out_len
 *  - char *in_buf
 *  - int in_buf_len
 *
 *  returns:
 *      -3: error while receiving response
 *      -4: error while sending request
 *      -7: error while connecting
 *    else: len of response
 */
int tcp_request(int socket, uint16_t port, uint32_t ipv4, char* out_buf, int out_len, char* in_buf, int in_buf_len) {
  struct sockaddr_in server_addr;
  int c, in_len, slen = sizeof(server_addr);
  server_addr.sin_family = AF_INET; // IPv4
  server_addr.sin_addr.s_addr = htonl(ipv4); // Ip-Adresse
  server_addr.sin_port = htons(port); // Port
  if (connect(socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
    return -7;
  if(send(socket, out_buf, out_len, 0) == -1)
    return -4;
  if ((in_len = recv(socket, in_buf, in_buf_len, 0)) == -1)
    return -3;
  return in_len;
}

#endif
