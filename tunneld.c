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
#define MAX 5000 

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
    int *tunTest = (int*)tunNum;
    int tun = *tunTest; 
    char IP[50];
    char outPort[50]; 
    int myport = tuns[tun].myPort;
    strcpy(IP,tuns[tun].servIP);
    strcpy(outPort,tuns[tun].servPort);
    char buf[MAX];

    //CREATE myADDR and stuff TO TALK WITH A
    struct sockaddr_in myaddr; //OUR ADDRESS
    struct sockaddr_in remaddr; //REMOTE ADDRESS
    socklen_t addrlen = sizeof(remaddr);
    int recvlen;
    int fd;
    //make udp socket
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("SOCKET ERROR\n");
    }
    //bind the socket to any valid IP and specific port
    memset((char*)&myaddr, 0,sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(myport);
    if (bind(fd, (struct sockaddr*)&myaddr, sizeof(myaddr)) < 0) {
        printf("BIND FAIL\n");
    }

    //CREATE HOSTENT and SERVADDR TO TALK WITH B
    struct hostent *hp; //host info
    struct sockaddr_in servaddr; //server address
    socklen_t servlen = sizeof(servaddr);
    //fill in server info
    memset((char*)&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //servaddr.sin_port = htons(atoi(outPort));
    servaddr.sin_port = htons(atoi(outPort));
    //look up address of server with name
    hp = gethostbyname(IP);
    if(!hp){
            printf("Could not find IP\n");
    }
    //put hte hosts address into the server address struct
    memcpy((void*)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

    while(1) {
        printf("\nPORT %d is Waiting\n",myport);
        bzero(buf,MAX);
        //LISTEN TO myport FOR A
        recvlen = recvfrom(fd,buf,MAX,0,(struct sockaddr *)&remaddr,&addrlen);
        printf("RECIEVED at PORT %d:\n'%s'\n",myport,buf);
        //cull down length
        char *b = buf;
        int i = 0;
        while (*b) {
            i++; 
            b++;
        }
        b=buf;
        if (*b == '$') {
            printf("PING PROTOCOL\n");
            //SEND INFO TO IP (B)
            printf("SENDING TO %s-%s\n",IP,outPort);
            sendto(fd,buf,i,0,(struct sockaddr*)&servaddr, sizeof(servaddr));
            //LISTEN FOR RESPONSE? (B)
            bzero(buf,MAX);
            recvfrom(fd,buf,MAX,0,(struct sockaddr*)&servaddr, &servlen); 
            printf("RECIEVED from %s-%s:%s\n",IP,outPort,buf);

            b = buf;
            i = 0;
            while (*b) {
                i++; 
                b++;
            }
            //FWD RESPONSE TO (A)
            sendto(fd,buf,i,0,(struct sockaddr *) &remaddr, addrlen);
        } else if (*b == 'L') {
            printf("TRAFFIC PROTOCOL\n");
            //SEND INFO TO IP (B)
            printf("SENDING TO %s-%s\n",IP,outPort);
            sendto(fd,buf,i,0,(struct sockaddr*)&servaddr, sizeof(servaddr));
            //LISTEN FOR RESPONSES for X times (B)
            for (int x = 0; x < i-1; ++x) {
                //Get message
                bzero(buf,MAX);
                recvlen = recvfrom(fd,buf,MAX,0,(struct sockaddr *)&remaddr,&addrlen);
                printf("RECIEVED at PORT %d:\n'%s'\n",myport,buf);
                //send message 
                printf("SENDING TO %s-%s\n",IP,outPort);
                sendto(fd,buf,i,0,(struct sockaddr*)&servaddr, sizeof(servaddr));
            }
            for (int x = 0; x < 3; ++x) {
                bzero(buf,MAX);
                recvlen = recvfrom(fd,buf,MAX,0,(struct sockaddr *)&remaddr,&addrlen);
                printf("RECIEVED at PORT %d:\n'%s'\n",myport,buf);
                //send message 
                printf("SENDING TO %s-%s\n",IP,outPort);
                sendto(fd,buf,3,0,(struct sockaddr*)&servaddr, sizeof(servaddr));
            } 
            printf("TRAFFIC PROTOCOL END\n");
        } 
    }
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
        printf("TEST2:%s, %s\n",targetIP,targetPort); //this works
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
            int send = amountT;
            pthread_create(&listen,NULL, connectionListen,&send);
            
            //RETURN PORT TO USER
            //use SENDTO to report port back
            char portNumber[50];
            sprintf(portNumber, "%d",returnPort);
            sendto(fd,portNumber,50,0,(struct sockaddr *)&remaddr,addrlen);
            ++amountT;
        }
    } /* never exits */
}

