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


//GLOBALS
//Max Connections
#define N 5
#define MAX 1024

//Struct of a tunnel
struct tunnel{
    //info about server I want to tunnel to
    char servIP[50];
    char servPort[50];
    
    //info about where to listen for info
    int myPort;
};
//List of tunnels
struct tunnel tuns[N];
//Amount of tunnels
int amountT;

//THREADED
//Takes the tunnel number 
void *connectionListen(void* tunNum){
    int *tun = (int*)tunNum;
    //TODO
    //start listening on myPort
    //relay information to servIP/Port
    //TODO write client

}
//Listens for incoming stuff
int main (int argc, char *argv[]) {
    //GET PORT
    int port = 0;
    if (argc <= 1) {
        port = 8000; 
    } else {
            try{
                port = atoi(argv[1]);
            } catch(...){
                port = 8000;
            }
        if (port == 0 || port < 1024){
            printf("Illegal Port Entered, assuming port 8000\n");
            port = 8000;
        }
        printf("Assigned Port is %d\n",port);
    }

    //init connections
    amountT = 0;

    //START LISTENING FOR INPUTS
    //CODE TAKEN from CS.rutgers
    struct sockaddr_in myaddr; /* our address */ 
    struct sockaddr_in remaddr; /* remote address */ 
    socklen_t addrlen = sizeof(remaddr); /* length of addresses */ 
    int recvlen; /* # bytes received */ 
    int fd; /* our socket */ 
    char buf[MAX]; /* receive buffer */ 
    /* create a UDP socket */ 
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        printf("cannot create socket\n"); 
        return 0; 
    } 
    /* bind the socket to any valid IP address and a specific port */ 
    
    memset((char *)&myaddr, 0, sizeof(myaddr)); 
    myaddr.sin_family = AF_INET; 
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    myaddr.sin_port = htons(port); 
    
    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) { 
        printf("bind failed"); 
        return 0; 
    } 

    /* now loop, receiving data and printing what we received */ 
    for (;;) { 
        printf("waiting on port %d\n", port); 
        recvlen = recvfrom(fd, buf, MAX, 0, (struct sockaddr *)&remaddr, &addrlen); 
        printf("received %d bytes\n", recvlen);     
        if (recvlen > 0) { 
            buf[recvlen] = 0; 
            printf("received message: \"%s\"\n", buf); 
        } 
        //PULL APART MESSAGE
        //Language:MYIP MYPORT targetIP targetPort (we can ignore MY*)
        char *targetIP;
        char *targetPort;
        char *w = buf;
        int flag = 0;
        while (*w != 0) {
            if (*w == ' ') {
                *w = 0;
                flag++;
                if (flag == 2) 
                    targetIP = (w+1);
                else if (flag ==3){
                    targetPort = (w+1);
                    break;
                }
            }
            ++w;
        }
        //Open up connection
        if (amountT >= N) {
            // SEND FULL
            char reply[50];
            strcpy(reply, "Full on Connections\n");
            sendto(fd,reply,50,0,(struct sockaddr *)&remaddr,addrlen);
        } else {
            //ADD CONNECTION
            strcpy(tuns[amountT].servIP,targetIP);
            strcpy(tuns[amountT].servPort,targetPort);
            //Get port number 
            int returnPort = 8010;
            returnPort += amountT;
            tuns[amountT].myPort = returnPort;
            
            //START LISTENING THREAD
            pthread_t listen;
            pthread_create(&listen,NULL, connectionListen,&amountT);
            
            //RETURN PORT TO USER
            //use SENDTO to report port back
            char portNumber[50];
            sprintf(portNumber, "%d",returnPort);
            sendto(fd,portNumber,50,0,(struct sockaddr *)&remaddr,addrlen);
            ++amountT;
        }
    } /* never exits */
}
