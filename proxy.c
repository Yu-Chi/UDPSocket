/*
    Simple udp client
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define IP "127.0.0.1"
#define BUFLEN 512  //Max length of buffer
#define RECEIVERPORT 8888   //The port on which to send data

#define SENDERPORT 9999
 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main(void)
{
    //Receiver
    struct sockaddr_in si_me, si_other1;
    
    int s, i, slen1 = sizeof(si_other1) , recv_len;
    char buf1[BUFLEN];
    
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
    
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(RECEIVERPORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }

    
    //Sender
    struct sockaddr_in si_other;
    int skt, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
 
    if ( (skt=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(SENDERPORT);
     
    if (inet_aton(IP , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    
    
 
    while(1)
    {
        //Receiver
        fflush(stdout);
        
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf1, BUFLEN, 0, (struct sockaddr *) &si_other1, &slen1)) == -1)
        {
            die("recvfrom()");
        }
        //
        //        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other1.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf1);
        
        
        //Sender

        
        //send the message
        if (sendto(skt, buf1, BUFLEN , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
        
    }
 
    closesocket(s);
    closesocket(skt);
    return 0;
}