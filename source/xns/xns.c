#include <stdio.h>
#include <stdlib.h>
#include "../include/soclib.h"
#include "dnslib.h"

#define PORT 53
#define BUFLEN 512


void die(char *s) {
	perror(s);
	exit(1);
}


int main(int argc, char *argv[]) {
	struct sockaddr_in si_other;
  int s, recv_len, slen=sizeof(si_other);
  char buf[BUFLEN];
  dns_header_t dns_header;
  dns_body_t dns_body;

  // create socket
  if ((s = udp_socket_in(PORT)) < -1 )
    die("ERROR: while creating socket\n");


  while(1) {
    // receive
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
			die("ERROR: while receiving data\n");

    // parse_request
    dnslib_parse_request(buf, &dns_header, &dns_body);

    //print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    printf("id: %x\n", dns_header.id);
    printf("flags:\n");
    printf("\tflags(qr): %d\n", dns_header.flags.qr);
    printf("\tflags(opcode): %d\n", dns_header.flags.opcode);
    printf("\tflags(aa): %d\n", dns_header.flags.aa);
    printf("\tflags(tc): %d\n", dns_header.flags.tc);
    printf("\tflags(rd): %d\n", dns_header.flags.rd);
    printf("\tflags(ra): %d\n", dns_header.flags.ra);
    printf("\tflags(z): %d\n", dns_header.flags.z);
    printf("\tflags(rcode): %d\n", dns_header.flags.rcode);
    printf("qdcount: %x\n", dns_header.qdcount);
    printf("ancount: %x\n", dns_header.ancount);
    printf("nscount: %x\n", dns_header.nscount);
    printf("arcount: %x\n", dns_header.arcount);
    printf("domain: %s\n", dns_body.domain);
    printf("type: %x\n", dns_body.type);
    printf("class: %x\n", dns_body.class);

    // create dummy A record response
    if (dns_body.type != A)
      continue;
    if (dns_body.class != INET)
      continue;
    dns_body.ttl = 3600;
    dns_body.data_length = 4;
    dns_body.ipv4 = 0x7F000001; // localhost

    // build response
    dnslib_build_response(buf, &dns_header, &dns_body);

    // send reply
    if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
			die("ERROR: while sending data\n");
  }

  // close socket
  close(s);
  return 0;
}
