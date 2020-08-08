#include "dnslib.h"
#include <stdlib.h>
#include <string.h>

void dnslib_parse_flags(uint16_t raw, dns_flags_t* flags) {
  flags->qr = (uint8_t)((0x8000 & raw)>>15);
  flags->opcode = (uint8_t)((0x7800 & raw)>>11);
  flags->aa = (uint8_t)((0x0400 & raw)>>10);
  flags->tc = (uint8_t)((0x0200 & raw)>>9);
  flags->rd = (uint8_t)((0x0100 & raw)>>8);
  flags->ra = (uint8_t)((0x0080 & raw)>>7);
  flags->z = (uint8_t)((0x0070 & raw)>>4);
  flags->rcode = (uint8_t)(0x000F & raw);
}

void dnslib_parse_request(char *buf, dns_header_t *dns_header, dns_body_t *dns_body) {
  char *buf_ptr, *domain_ptr;
  // parse dns_header into (dns_header)
  buf_ptr = buf;
  dns_header->id = (((uint16_t)(buf_ptr[0]))<<8) + (uint16_t)(buf_ptr[1]);
  dnslib_parse_flags((((uint16_t)(buf_ptr[2]))<<8) + (uint16_t)(buf_ptr[3]), &dns_header->flags);
  dns_header->qdcount = (((uint16_t)(buf_ptr[4]))<<8) + (uint16_t)(buf_ptr[5]);
  dns_header->ancount = (((uint16_t)(buf_ptr[6]))<<8) + (uint16_t)(buf_ptr[7]);
  dns_header->nscount = (((uint16_t)(buf_ptr[8]))<<8) + (uint16_t)(buf_ptr[9]);
  dns_header->arcount = (((uint16_t)(buf_ptr[10]))<<8) + (uint16_t)(buf_ptr[11]);
  buf_ptr += 12;
  // parse domain into (dns_body.domain)
  domain_ptr = dns_body->domain;
  while(*buf_ptr) {
    memcpy(domain_ptr, buf_ptr+1, *buf_ptr);
    domain_ptr += *buf_ptr;
    buf_ptr += *buf_ptr+1;
    *domain_ptr++ = '.';
  }
  *(--domain_ptr) = '\0';
  buf_ptr++;
  //parse type and class into (dns_body.body)
  dns_body->type = (((uint16_t)(buf_ptr[0]))<<8) + (uint16_t)(buf_ptr[1]);
  dns_body->class = (((uint16_t)(buf_ptr[2]))<<8) + (uint16_t)(buf_ptr[3]);
  buf_ptr += 4;
}

void dnslib_build_response(char *buf, dns_header_t *dns_header, dns_body_t *dns_body) {

}
