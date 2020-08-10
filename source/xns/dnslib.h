#include <stdint.h>

#define MAX_DOMAIN_LENGTH 64

#define INET 0x0001

#define A 0x0001
#define NS 0x0002
#define MX 0x000F

#define RCODE_NO_ERROR 0
#define RCODE_FORMAT_ERROR 1 // unable to parse query
#define RCODE_SERVER_FAILUR 2
#define RCODE_NAME_ERROR 3 // domain does not exist
#define RCODE_REFUSED 4 // refused query for policy reasons


typedef struct dns_flags {
  uint8_t qr; // 1bit: 0 query, 1 response
  uint8_t opcode; // 4bit: kind of query (default 0000)
  uint8_t aa; // 1bit: is response authoritive 0/1
  uint8_t tc; // 1 bit: message was truncated(gekuerzt) 0/1 (default 0)
  uint8_t rd; // 1bit: recursion desired 0/1 (default 1)
  uint8_t ra; // 1bit: recursion available 0/1 (default 1)
  uint8_t z; // 3bit; reserved for future (default 000)
  uint8_t rcode; // 4bit: statuscodes (defined above)
} dns_flags_t;

typedef struct dns_header {
  uint16_t id; // id (same in request and response)
  dns_flags_t flags; // flags parsed
  uint16_t qdcount; // question count (default 1 also in response)
  uint16_t ancount; // answer count (default 1)
  uint16_t nscount; // number of NS records
  uint16_t arcount; // number of other records
} dns_header_t;

typedef struct dns_body {
  // get from request
  char domain[MAX_DOMAIN_LENGTH];
  uint16_t type; // record type (default A)
  uint16_t class; // class (default INET)
  // additional for response
  uint16_t ttl; // time to live
  uint16_t data_length; // length of response data (default 4 (ipv4))
  uint32_t ipv4; // response data for A record
} dns_body_t;

void dnslib_parse_request(char *buf, dns_header_t *dns_header, dns_body_t *dns_body);
void dnslib_build_response(char *buf, dns_header_t *dns_header, dns_body_t *dns_body);
