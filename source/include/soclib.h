#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

/**
 * reads uint32 from char*
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
 * writes uint16 to char*
 */
char *read_uint16(char *ptr, uint16_t *value) {
  *value = (((uint16_t)(ptr[0]))<<8) | (uint16_t)(ptr[1]);
  return ptr+2;
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
 *      -3: error while receiving request
 *      -4: error while sending response
 *    else: len of response
 */
int udp_request(int socket, int port, int ipv4, char* out_buf, int out_len, char* in_buf, int in_buf_len) {
  struct sockaddr_in server_addr;
  int in_len, slen = sizeof(server_addr);
	server_addr.sin_family = AF_INET; // IPv4
	server_addr.sin_addr.s_addr = htonl(ipv4); // Ip-Adresse
	server_addr.sin_port = htons(port); // Port
  if (sendto(socket, out_buf, out_len, MSG_CONFIRM, (struct sockaddr*) &server_addr, slen) == -1)
    return -4;
  if ((in_len = recvfrom(socket, in_buf, in_buf_len, MSG_WAITALL, (struct sockaddr *) &server_addr, &slen)) == -1)
    return -3;
  return in_len;
}