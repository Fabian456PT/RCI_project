#ifndef head
#define head

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

#define PORT "58001"  // Porta de comunicação
#define BUFFER_SIZE 128

int max_fd = 0;

typedef struct{
    char ip[16];
    char tcp[6];
}id_struct;

typedef struct{
    id_struct id;
    id_struct vzext;
    id_struct vzsalv;
    id_struct intr[16];
    int intr_num;

}node;

void read_stdin(char *buffer);

void verify_commandline(char *buffer);

int create_client();

int create_server();


#endif
