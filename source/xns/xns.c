#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

# define PORT 53
# define BUFLEN 512

typedef struct dns_header {
  short id;
  short flags;
  short qdcount;
  short ancount;
  short nscount;
  short arcount;
} dns_header_t;

typedef struct dns_req_body {
  char domain[BUFLEN];
  short type;
  short class;
} dns_req_body_t;

void die(char *s) {
	perror(s);
	exit(1);
}

void parse_request(char *buf, dns_header_t *dns_header, dns_req_body_t *dns_req_body) {
  char *buf_ptr, *domain_ptr;
  // parse dns_header into (dns_header)
  buf_ptr = buf;
  dns_header->id = (((short)(buf_ptr[0]))<<8) + (short)(buf_ptr[1]);
  dns_header->flags = (((short)(buf_ptr[2]))<<8) + (short)(buf_ptr[3]);
  dns_header->qdcount = (((short)(buf_ptr[4]))<<8) + (short)(buf_ptr[5]);
  dns_header->ancount = (((short)(buf_ptr[6]))<<8) + (short)(buf_ptr[7]);
  dns_header->nscount = (((short)(buf_ptr[8]))<<8) + (short)(buf_ptr[9]);
  dns_header->arcount = (((short)(buf_ptr[10]))<<8) + (short)(buf_ptr[11]);
  buf_ptr += 12;
  // parse domain into (dns_req_body.domain)
  domain_ptr = dns_req_body->domain;
  while(*buf_ptr) {
    memcpy(domain_ptr, buf_ptr+1, *buf_ptr);
    domain_ptr += *buf_ptr;
    buf_ptr += *buf_ptr+1;
    *domain_ptr++ = '.';
  }
  *domain_ptr = '\0';
  buf_ptr++;
  //parse type and class into (dns_req.body)
  dns_req_body->type = (((short)(buf_ptr[0]))<<8) + (short)(buf_ptr[1]);
  dns_req_body->class = (((short)(buf_ptr[2]))<<8) + (short)(buf_ptr[3]);
  buf_ptr += 4;
}

int main(int argc, char *argv[]) {
	struct sockaddr_in si_me, si_other;
  int s, recv_len, slen=sizeof(si_other);
  char buf[BUFLEN];
  dns_header_t dns_header;
  dns_req_body_t dns_req_body;

  // create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    die("ERROR: while creating socket\n");
	}

  // create address
	si_me.sin_family = AF_INET; // IPv4
	si_me.sin_addr.s_addr = htonl(INADDR_ANY); // Ip-Adresse
	si_me.sin_port = htons(PORT); // Port

  // bind socket to port
  if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
		die("ERROR: while binding to port\n");
	}

  while(1) {
    // receive
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1) {
			die("ERROR: while receiving data\n");
		}

    // parse_request
    parse_request(buf, &dns_header, &dns_req_body);

    //print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    printf("id: %x\n", dns_header.id);
    printf("flags: %x\n", dns_header.flags);
    printf("qdcount: %x\n", dns_header.qdcount);
    printf("ancount: %x\n", dns_header.ancount);
    printf("nscount: %x\n", dns_header.nscount);
    printf("arcount: %x\n", dns_header.arcount);
    printf("domain: %s\n", dns_req_body.domain);
    printf("type: %x\n", dns_req_body.type);
    printf("type: %x\n", dns_req_body.class);

    // send reply
    if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1) {
			die("ERROR: while sending data\n");
		}
  }

  // close socket
  close(s);
  return 0;
}
