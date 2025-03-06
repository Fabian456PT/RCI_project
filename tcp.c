#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netdb.h>


int create_socket(int port){
    struct addrinfo hints, *res;
    int fd,n;

    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1){
        perror("socket not created");
        return -1;
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    n=getaddrinfo(NULL,port,&hints,&res);
    if(n!=0){
        perror("getaddrinfo failed");
        return -1;
    }

    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        perror("connection failed");
        return -1;
    }
}