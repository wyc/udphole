// UDP hole punching example, client code
// Base UDP code stolen from http://www.rapapaing.com/blog/?p=24

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
 
#define BUFLEN 512
#define NPACK 10
#define PORT 9930
 
#define SRV_IP "50.22.61.235"
 
struct client {
    int host;
    short port;
};
 
int main(int argc, char* argv[])
{
    struct sockaddr_in client, server;
    int s, i, f, j, k;
    socklen_t server_len=sizeof(si_other);
    struct client buf;
    struct client server;
    struct client peers[10];
    int n = 0;
 
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        diep("socket");
 
    memset(si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    if (inet_aton(SRV_IP, &si_other.sin_addr)==0)
        diep("aton");
 
    server.host = si_other.sin_addr.s_addr;
    server.port = si_other.sin_port;
 
    if (sendto(s, "hi", 2, 0, (struct sockaddr*)(&si_other), slen)==-1)
        diep("sendto");
 
    while (1) {
        if (recvfrom(s, &buf, sizeof(buf), 0, (struct sockaddr*)(&si_other), &slen)==-1)
            diep("recvfrom");
        printf("Data from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        if (server.host == si_other.sin_addr.s_addr && server.port == (short)(si_other.sin_port)) {
            f = 0;
            for (i = 0; i < n && f == 0; i++) {
                if (peers[i].host == buf.host && peers[i].port == buf.port)
                {
                    f = 1;
                }
            }
            if (f == 0) {
                peers[n].host = buf.host;
                peers[n].port = buf.port;
                n++;
            }
            si_other.sin_addr.s_addr = buf.host;
            si_other.sin_port = buf.port;
            printf("Got endpoint %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
            printf("%d peers\n", n);
            for (k = 0; k < 10; k++) {
                for (i = 0; i < n; i++) {
                    si_other.sin_addr.s_addr = peers[i].host;
                    si_other.sin_port = peers[i].port;
                    if (sendto(s, "This is a datagram: bG9sIHRlc3RpbmcK!", 2, 0, (struct sockaddr*)(&si_other), slen)==-1)
                        diep("sendto()");
                }
            }
        }
        else {
            for (i = 0; i < n; i++) {
                if (peers[i].host == buf.host && peers[i].port == (short)(buf.port)) {
                    printf("Got payload from peer %d!\n", i);
                    printf("Message: %s\n", (char *)&buf);
                    break;
                }
            }
        }
    }
 
    close(s);
    return 0;
}

