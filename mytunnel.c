#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <ctype.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 1024

int main (int argc, char *argv[]) {
    //Takes console input
    //::: VPNip VPNport servIP servPORT
    if (argc != 5) {
        printf ("Not enough arguments!\n");
        exit(0);
    }
    //get commands
    char VPNip[50];
    char VPNport[50];
    char SERVip[50];
    char SERVport[50];
    strcpy(VPNip,argv[1]);
    strcpy(VPNport,argv[2]);
    strcpy(SERVip,argv[3]);
    strcpy(SERVport,argv[4]);
    
    struct hostent *hp;
    struct sockaddr_in servaddr;
    memset((char*)&servaddr, 0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(VPNport));
    int fd; /* our socket */ 

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        printf("cannot create socket\n"); 
        return 0; 
    } 
    //get host name ip
    hp = gethostbyname(VPNip);
    if (!hp) {
        printf("FAILED\n");
    }

    //put host address into server address structure
    memcpy((void*)&servaddr.sin_addr, hp->h_addr_list[0],hp->h_length);
    //ready to send message
    
    //make message
    char buf[MAX];
    strcpy(buf,VPNip);
    strcat(buf," ");
    strcat(buf,VPNport);
    strcat(buf," ");
    strcat(buf,SERVip);
    strcat(buf," ");
    strcat(buf,SERVport);
   
    //create listener
    struct sockaddr_in myaddr; /* our address */ 
    struct sockaddr_in remaddr; /* remote address */ 
    socklen_t addrlen = sizeof(remaddr); /* length of addresses */ 
    int recvlen; /* # bytes received */ 
    /* bind the socket to any valid IP address and a specific port */ 
    
    memset((char *)&myaddr, 0, sizeof(myaddr)); 
    myaddr.sin_family = AF_INET; 
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    //ITS LISTENING ON 8000
    myaddr.sin_port = 8000; 
    
    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) { 
        printf("bind failed"); 
        return 0; 
    } 
 


    //send message
    sendto(fd,buf,MAX,0,(struct sockaddr *)&servaddr, sizeof(servaddr));
    bzero(buf,MAX);
    //wait for return message
    recvlen = recvfrom(fd, buf, MAX, 0, (struct sockaddr *)&remaddr, &addrlen); 
    printf("VPN SOCKET IS ON %s\n",buf);
    close(fd);
}
