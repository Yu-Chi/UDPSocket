/*
    Simple udp client
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
 
#define RECEIVER "127.0.0.1"
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data

// Can create separate header file (.h) for all headers' structure
// The IP header's structure
struct ipheader {
 unsigned char      iph_ihl:5, iph_ver:4;
 unsigned char      iph_tos;
 unsigned short int iph_len;
 unsigned short int iph_ident;
 unsigned char      iph_flag;
 unsigned short int iph_offset;
 unsigned char      iph_ttl;
 unsigned char      iph_protocol;
 unsigned short int iph_chksum;
 unsigned int       iph_sourceip;
 unsigned int       iph_destip;
};
 
// UDP header's structure
struct udpheader {
 unsigned short int udph_srcport;
 unsigned short int udph_destport;
 unsigned short int udph_len;
 unsigned short int udph_chksum;
};

/* 
    96 bit (12 bytes) pseudo header needed for udp header checksum calculation 
*/
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};
 
/*
 Generic checksum calculation function
 */
unsigned short csum(unsigned short *ptr,int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
    
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
    
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
    
    return(answer);
}


void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main(void)
{
    struct sockaddr_in si_other;
    int handler, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
struct pseudo_header psh;
    
    //Datagram to represent the packet
    char datagram[4096] , source_ip[32] , *data , *pseudogram;
    
    //zero out the packet buffer
    memset (datagram, 0, 4096);
    
 
    //Create Socket
    if ( (handler=socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
    {
        printf("%s\n","Unsuccessful!");
        die("socket");
       
    }
    
    //IP header
    struct ipheader *iph = (struct ipheader *) datagram;
    
    //UDP header
    struct udpheader *udph = (struct udpheader *) (datagram + sizeof (struct ipheader));
//     
//     //Data part
//         data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
        strcpy(data , "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
//     
//     //some address resolution
        strcpy(source_ip , RECEIVER);
//  
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
//     
//     //Fill in the IP Header

    // standard header structures but assign our own values.
iph->iph_ihl = 5;
iph->iph_ver = 4;
iph->iph_tos = 16; // Low delay
iph->iph_len = sizeof(struct ipheader) + sizeof(struct udpheader);
iph->iph_ident = htons(54321);
iph->iph_ttl = 64; // hops
iph->iph_protocol = IPPROTO_UDP; // UDP
// Source IP address, can use spoofed address here!!!
iph->iph_sourceip = inet_addr ( source_ip );    //Spoof the source ip address
// The destination IP address
iph->iph_destip = si_other.sin_addr.s_addr;

//     iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data);
//     iph->id = htonl (54321); //Id of this packet
//     iph->frag_off = 0;
//     iph->ttl = 255;
//     iph->protocol = IPPROTO_UDP;
//     iph->check = 0;      //Set to 0 before calculating checksum
//     iph->saddr = inet_addr ( source_ip );    //Spoof the source ip address
//     iph->daddr = sin.sin_addr.s_addr;
//     
//     //Ip checksum
    iph->iph_chksum = csum ((unsigned short *) datagram, iph->iph_len);
//     
//     //UDP header
    udph->udph_srcport = htons (6666);
    udph->udph_destport = htons (8622);
    udph->udph_len = htons(8 + strlen(data)); //tcp header size
    udph->udph_chksum = 0; //leave checksum 0 now, filled later by pseudo header
//     
//     //Now the UDP checksum using the pseudo header
    psh.source_address = inet_addr( source_ip );
    psh.dest_address = si_other.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(sizeof(struct udphdr) + strlen(data) );
//     
    int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
    pseudogram = malloc(psize);
//     
    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + strlen(data));
//     
    udph->udph_chksum = csum( (unsigned short*) pseudogram , psize);
//     
//     //loop if you want to flood :)
    //while (1)
    {
//         //Send the packet
        if (sendto (handler, datagram, iph->iph_len ,  0, (struct sockaddr *) &si_other, sizeof (si_other)) < 0)
        {
            perror("sendto failed");
        }
//         //Data send successfully
        else
        {
            printf ("Packet Send. Length : %d \n" , iph->iph_len);
        }
    }
    
//    if (inet_aton(RECEIVER , &si_other.sin_addr) == 0)
//    {
//        fprintf(stderr, "inet_aton() failed\n");
//        exit(1);
//    }
 
//    while(1)
//    {
//        printf("Enter message1 : ");
//        gets(message);
//         
//        //send the message
//        if (sendto(handler, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
//        {
//            die("sendto()");
//        }
//         
//        //receive a reply and print it
//        //clear the buffer by filling null, it might have previously received data
//        memset(buf,'\0', BUFLEN);
//        
//    }
 
//     closesocket(handler);
    return 0;
}