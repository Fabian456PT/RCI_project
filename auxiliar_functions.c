#include "head.h"


void read_stdin(char *buffer){
    int n = 99;

    fgets(buffer, n, stdin);
    //CHECK IF THIS WORKS
    fflush(0);// this fflush makes sure that if the user writes more than 99 characteres the rest goes to the trash

}

void verify_commandline(char *buffer){
    char direct_join = "direct join";
    char dj = "dj";
    char join = "join";
    char j = "j";
    char st = "st";
    char show_topology = "show topology";

    char* token = strtok(str, " ");

    if(strcmp(token, dj) == 0){

    }


}

int create_client(char *ip, char *tcp){

    int fd,errcode;
    struct addrinfo hints,*res;
    char buffer[128];

    fd = socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (fd==-1) exit(1); //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode = getaddrinfo(ip, tcp, &hints, &res);
    if(errcode != 0)/*error*/exit(1);

    ssize_t n;
    n = connect(fd,res->ai_addr,res->ai_addrlen);
    if(n == -1)/*error*/exit(1);

    char message[50];
    sprintf(message, "ENTRY %s %s\n", ip, tcp);
    printf("%s\n", message);  
    n = write(fd,message,strlen(message));
    if(n==-1)/*error*/exit(1);

    //select aqui      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    n = read(fd,buffer,128);
    if(n == -1)/*error*/exit(1);
   
    return fd;
}


int create_server(){// FAZER ATÉ AO LISTEN

    int fd,errcode;
    ssize_t n;
    struct addrinfo hints,*res;
    char buffer[128];
    int newfd;

    fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (fd==-1) exit(1); //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(NULL,PORT,&hints,&res);
    if((errcode)!=0)/*error*/exit(1);

    LISTEN...

    return fd;
}
