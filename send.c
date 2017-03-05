//GEORGES BRANTLEY
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>

#define BUFSIZE 2048

//Code gotten from cs.rutgers
int main (int argc, char **argv) {
    struct hostent *hp;
    struct sockaddr_in servaddr;
    char my_message[40];
    strcpy(my_message,"$1111111111$");

    int fd;
    fd = socket(AF_INET,SOCK_DGRAM,0);

    memset((char*)&servaddr, 0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8010);

    hp = gethostbyname("xinu02.cs.purdue.edu");
    memcpy((void*)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

    sendto(fd, my_message, strlen(my_message), 0,(struct sockaddr *)&servaddr, sizeof(servaddr));

    char buf[50];
    socklen_t servlen = sizeof(servaddr);
    recvfrom(fd,buf,50,0,(struct sockaddr*)&servaddr,&servlen);
    printf("RECIEVED %s\n",buf);
}
