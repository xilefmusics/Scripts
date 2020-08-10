#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/soclib.h"

#define PORT 53
#define BUFLEN 512
#define MAX_DOMAIN_LENGTH 64

char domain[MAX_DOMAIN_LENGTH];

char *parse_domain(char *buf_ptr) {
  char * domain_ptr = domain;
  while(*buf_ptr) {
    memcpy(domain_ptr, buf_ptr+1, *buf_ptr);
    domain_ptr += *buf_ptr;
    buf_ptr += *buf_ptr+1;
    *domain_ptr++ = '.';
  }
  *(--domain_ptr) = '\0';
  return buf_ptr + 1;
}

char *write_uint16(char *ptr, uint16_t value){
  ptr[0] = (char) (value>>8);
  ptr[1] = (char) value;
  return ptr+2;
}
char *write_uint32(char *ptr, uint32_t value){
  ptr[0] = (char) (value>>24);
  ptr[1] = (char) (value>>16);
  ptr[2] = (char) (value>>8);
  ptr[3] = (char) value;
  return ptr+4;
}
char *read_uint16(char *ptr, uint16_t *value) {
  *value = (((uint16_t)(ptr[0]))<<8) | (uint16_t)(ptr[1]);
  return ptr+2;
}

int dns_handler(char *in_buf, char **out_buf, int in_len) {
  char *ptr = in_buf+2;
  uint16_t type, class;

  // write header
  ptr = write_uint16(ptr, 0x8180);      // FLAGS
  ptr = write_uint16(ptr, 1);           // QCOUNT
  ptr = write_uint16(ptr, 1);           // ACOUNT
  ptr = write_uint16(ptr, 0);           // NSCOUNT
  ptr = write_uint16(ptr, 0);           // ARCOUNT

  // read question body
  ptr = parse_domain(ptr);              // read domain
  ptr = read_uint16(ptr, &type);        // read type
  ptr = read_uint16(ptr, &class);       // read class
  printf("domain: %s\n", domain);

  // write answer body
  ptr = write_uint16(ptr, 0xc00c);      // write pointer/offset (c|00c)
  ptr = write_uint16(ptr, type);        // write type
  ptr = write_uint16(ptr, class);       // write class
  ptr = write_uint32(ptr, 3600);        // write time to live (1h)
  ptr = write_uint16(ptr, 4);           // write length of answer (4 Byte for IPv4)
  ptr = write_uint32(ptr, 0x7F000001);  // write IPv4 address (127.0.0.1)

  // return length of answer
  return ptr-in_buf;
}

int main(int argc, char *argv[]) {
  char buf[BUFLEN];
  return udp_server(PORT, buf, BUFLEN, dns_handler);
}
