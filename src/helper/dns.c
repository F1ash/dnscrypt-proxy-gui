//DNS Query Program on Linux
//Author : Silver Moon ([email protected])
//Dated : 29/4/2009

// remaked for application usage

//Header Files
//#include    <stdio.h>       //printf
#include    "dns.h"
#include    <string.h>      //strlen
#include    <stdlib.h>      //malloc
#include    <sys/socket.h>  //you know what this is for
//#include    <arpa/inet.h>   //inet_addr , inet_ntoa , ntohs etc
//#include    <netinet/in.h>
#include    <unistd.h>      //getpid
#include    <time.h>        //time

#define T_A 1       //Ipv4 address
#define T_AAAA 28   //Ipv6 address
#define T_NS 2      //Nameserver
#define T_CNAME 5   // canonical name
#define T_SOA 6     /* start of authority zone */
#define T_PTR 12    /* domain name pointer */
#define T_MX 15     //Mail server

//Function Prototypes
void ChangetoDnsNameFormat (unsigned char*,unsigned char*);

//DNS header structure
struct DNS_HEADER
{
    unsigned short id;          // identification number

    unsigned char rd :1;        // recursion desired
    unsigned char tc :1;        // truncated message
    unsigned char aa :1;        // authoritive answer
    unsigned char opcode :4;    // purpose of message
    unsigned char qr :1;        // query/response flag

    unsigned char rcode :4;     // response code
    unsigned char cd :1;        // checking disabled
    unsigned char ad :1;        // authenticated data
    unsigned char z :1;         // its z! reserved
    unsigned char ra :1;        // recursion available

    unsigned short q_count;     // number of question entries
    unsigned short ans_count;   // number of answer entries
    unsigned short auth_count;  // number of authority entries
    unsigned short add_count;   // number of resource entries
};

//Constant sized fields of query structure
struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};

//Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA
{
    unsigned short  type;
    unsigned short  _class;
    unsigned int    ttl;
    unsigned short  data_len;
};
#pragma pack(pop)

//Pointers to resource record contents
struct RES_RECORD
{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};

//Structure of a Query
typedef struct
{
    unsigned char   *name;
    struct QUESTION *ques;
} QUERY;

/*
 * Perform a DNS query by sending a packet
 * */
uint16_t is_responsible(unsigned long *t, int _family)
{
    sleep(1); // dirty action to wait when the dnscrypt-proxy will runned
    unsigned char host[100] = "google.com";
    unsigned char buf[65536],*qname;

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;

    //printf("Resolving %s" , host);

    struct sockaddr_in dest;

    int s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP); //UDP packet for DNS queries
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        //perror(" setsocketopt Error");
    }
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    //dns servers resolver
    dest.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *)&buf;

    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    //point to the query portion
    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];

    ChangetoDnsNameFormat(qname , host);
    qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)]; //fill it

    //qinfo->qtype = htons( query_type ); //type of the query , A , MX , CNAME , NS etc
    //type of the query , A , MX , CNAME , NS etc
    qinfo->qtype = htons( (_family==4)? T_A : T_AAAA );
    qinfo->qclass = htons(1); //its internet (lol)

    //printf("\nSending Packet...");
    sendto(
                s,
                (char*)buf,
                sizeof(struct DNS_HEADER)
                    + (strlen((const char*)qname)+1)
                    + sizeof(struct QUESTION),
                0,
                (struct sockaddr*)&dest,
                sizeof(dest));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    //Receive the answer
    recvfrom(
                s,
                (void*)buf,
                65536,
                0,
                (struct sockaddr*)&dest,
                (socklen_t*)sizeof(dest));
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    close(s);

    *t = (end.tv_sec - start.tv_sec)*1000000
       + (end.tv_nsec - start.tv_nsec)/1000;

    return ntohs(dns->ans_count);
}

/*
 * This will convert www.google.com to 3www6google3com
 * got it :)
 * */
void ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host)
{
    int lock = 0 , i;
    strcat((char*)host,".");

    for(i = 0 ; i < strlen((char*)host) ; i++)
    {
        if(host[i]=='.')
        {
            *dns++ = i-lock;
            for(;lock<i;lock++)
            {
                *dns++=host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++='\0';
}
