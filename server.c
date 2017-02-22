//GEORGES BRANTLEY
#include <sys/socket.h>
#define BUFSIZE 2048

//Code gotten from cs.rutgers
int main (int argc, char **argv) {
    struct sockaddr_in myaddr; /* our address */ 
    struct sockaddr_in remaddr; /* remote address */ 
    socklen_t addrlen = sizeof(remaddr); /* length of addresses */ 
    int recvlen; /* # bytes received */ 
    int fd; /* our socket */ 
    unsigned char buf[BUFSIZE]; /* receive buffer */ 
    /* create a UDP socket */ 
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        perror("cannot create socket\n"); 
        return 0; 
    } 
    /* bind the socket to any valid IP address and a specific port */ 
    
    memset((char *)&myaddr, 0, sizeof(myaddr)); 
    myaddr.sin_family = AF_INET; 
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    myaddr.sin_port = htons(PORT); 
    
    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) { 
        perror("bind failed"); 
        return 0; 
    } 

    /* now loop, receiving data and printing what we received */ 
    for (;;) { 
        printf("waiting on port %d\n", PORT); 
        recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen); 
        printf("received %d bytes\n", recvlen);     
        if (recvlen > 0) { 
            buf[recvlen] = 0; 
            printf("received message: \"%s\"\n", buf); 
        } 
    } /* never exits */
}
