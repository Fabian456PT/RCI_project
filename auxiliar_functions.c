#include "head.h"


void read_stdin(char *buffer){
    int n = 99;

    fgets(buffer, n, stdin);
    //CHECK IF THIS WORKS
    fflush(0);// this fflush makes sure that if the user writes more than 99 characteres the rest goes to the trash

}

int verify_ip_tcp_connection(char *buffer, int number){

    if(number == 1 || number == 2){ // check if the IP and TCP are acceptable

    }
    if(number == 3 || number == 4){ // check is the "net" is acceptable
        
    }
    if(number == 5 || number == 6){
        printf("Vizinho externo: %s %s\n", node.vzext.ip, node.vzext.tcp);
        printf("Vizinho salvaguarda: %s %s\n", node.vzsalv.ip, node.vzsalv.tcp);

        for(int i = 0; i < 16; i++){
            printf("Vizinho externo: %s %s\n", node.intr[i].ip, node.intr[i].tcp);
        }
    }


}

void verify_commandline(char *buffer){
    char direct_join = "direct join";
    char dj = "dj";
    char join = "join";
    char j = "j";
    char st = "st";
    char show_topology = "show topology";
    char *res1, *res2, *res3, *res4, *res5, *res6;

    // strstr(main_string, substring), here we are checking if the subsrting is inside the main_string and keep the result in a variable
    res1 = strstr(buffer, direct_join);
    res2 = strstr(buffer, dj);
    res3 = strstr(buffer, join);
    res4 = strstr(buffer, j);
    res5 = strstr(buffer, st);
    res6 = strstr(buffer, show topology);

    // here we make sure the previous result is equal to what we want
    if(res1 != NULL && strcmp(res1, direct_join) == 0){
        verify_ip_tcp(buffer, 1);
    }
    else if(res2 != NULL && strcmp(res2, dj) == 0){
        verify_ip_tcp(buffer, 2);
    }
    else if(res3 != NULL && strcmp(res3, join) == 0){
        verify_ip_tcp(buffer, 3);
    }
    else if(res4 != NULL && strcmp(res4, j) == 0){
        verify_ip_tcp(buffer, 4);
    }
    else if(res5 != NULL && strcmp(res5, st) == 0){
        verify_ip_tcp(buffer, 5);
    }
    else if(res6 != NULL && strcmp(res6, show topology) == 0){
        verify_ip_tcp(buffer, 6);
    }
    else{
        printf("Invalid command line\n");
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
