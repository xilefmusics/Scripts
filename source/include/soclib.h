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
