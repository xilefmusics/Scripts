#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/soclib.h"

#define PORT 53
#define FORWARD 0x01010101
#define BUFLEN 512
#define MAX_DOMAIN_LENGTH 64
#define CLASS_IN 1
#define RECORD_A 1

struct recordA {
  char *domain;
  char *ipv4;
  uint16_t ttl;
} recordsA[] = {
  {"me.intern", "127.0.0.1", 3600}
};
int num_recordsA = 1;

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

int lookupA(char *domain, uint32_t *ipv4, uint16_t *ttl) {
  for (int i = 0; i < num_recordsA; ++i) {
    if (!strcmp(recordsA[i].domain, domain)) {
      *ipv4 = atoipv4(recordsA[i].ipv4);
      *ttl = recordsA[i].ttl;
      return 0;
    }
  }
  return 1;
}

char tmp_buf[BUFLEN];
int forward(char *in_buf, int in_len) {
    int len = udp_request(53, FORWARD, in_buf, in_len, tmp_buf, BUFLEN, 500000);
    if (len > 0) {
      memcpy(in_buf, tmp_buf, len);
      return len;
    }
    write_uint16(in_buf+2,  0x8182);      // FLAGS (Server Failure)
    return in_len;
}

int dns_handler(char *in_buf, char **out_buf, int in_len) {
  // check qcount and forward, if not 1
  uint16_t qcount;
  read_uint16(in_buf+4, &qcount);
  if (qcount != 1)
    return forward(in_buf, in_len);

  // parse question and forward if not CLASS_IN and RECORD_A
  char *ptr = in_buf+12;
  uint16_t type, class;
  ptr = parse_domain(ptr);              // read domain
  ptr = read_uint16(ptr, &type);        // read type (A)
  ptr = read_uint16(ptr, &class);       // read class (IN)
  if (class != CLASS_IN || type != RECORD_A)
    return forward(in_buf, in_len);

  // lookup and forward if not found in database
  uint32_t ipv4;
  uint16_t ttl;
  if (lookupA(domain, &ipv4, &ttl))
    return forward(in_buf, in_len);

  // write answer body
  ptr = write_uint16(ptr, 0xc00c);      // write pointer/offset (c|00c)
  ptr = write_uint16(ptr, type);        // write type
  ptr = write_uint16(ptr, class);       // write class
  ptr = write_uint32(ptr, ttl);         // write time to live (1h)
  ptr = write_uint16(ptr, 4);           // write length of answer (4 Byte for IPv4)
  ptr = write_uint32(ptr, ipv4);        // write IPv4 address (127.0.0.1)

  // write header
  write_uint16(in_buf+2,  0x8180);      // FLAGS
  write_uint16(in_buf+4,  1);           // QCOUNT
  write_uint16(in_buf+6,  1);           // ACOUNT
  write_uint16(in_buf+8,  0);           // NSCOUNT
  write_uint16(in_buf+10, 0);           // ARCOUNT

  // return length of answer
  return ptr-in_buf;
}

int main(int argc, char *argv[]) {
  char buf[BUFLEN];
  return udp_server(PORT, buf, BUFLEN, dns_handler);
}
