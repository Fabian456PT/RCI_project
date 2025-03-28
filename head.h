#ifndef head
#define head

#include <asm-generic/socket.h>
#include <stddef.h>
#include <string.h>

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
#include <ctype.h>

typedef struct{
    char ip[16];
    char tcp[6];
}id_struct;

// structure for "OBJECT", "INTEREST" and "NOOBJECT"
typedef struct{
    char type[10];
    char name[101];
    int flag;
}message_type;

typedef struct{
    id_struct id;
    int our_socket;
    id_struct vzext;
    int ext_fd;
    id_struct vzsalv;
    int salv_fd;
    id_struct intr[16];
    int intr_num;
    int intr_fd[16];
    int biggest_socket;
    int udp_server_fd;
    struct addrinfo *res;
    int objects_num;
    int interest_num;
    char interest[100][101];
    char object[][101];
}node;

// auxiliar funtions
void initialize_our_node(node *our_node, char *ip, char *tcp);

void outer_node_left(node *our_node, fd_set fd_buffer, message_type *name);

void one_inner_node_left(node *our_node, int i);

void reorganize_inner_nodes(node *our_node, int i);

void read_stdin(char *buffer);

void keep_commandline_values(char *buffer, int number, id_struct *dj_connect, int *net, node *our_node);

int verify_commandline(char *buffer);

void get_message(char *buffer, id_struct *message_ip_tcp, message_type *name);

void get_ipandtcp_from_node_list(char *buffer, id_struct *ip_tcp_chosen);

void after_someone_tried_to_connect(node *our_node, int *newfd, id_struct *message_ip_tcp, char *buffer_fd, message_type *name);

// events
int create_client_udp(char *regip, char *regudp, int *net, id_struct *ip_tcp_chosen, node *our_node);

int create_client_tcp(node *our_node, id_struct *dj_connect, id_struct *message_ip_tcp, message_type *name);

int create_server_tcp(char *tcp);

// command lines
int join(char *buffer,id_struct *dj_connect,int *net,node *our_node, id_struct *ip_tcp_chosen, char *regip, char *regudp);

int direct_join(int *go_direct_join, char *buffer, id_struct *dj_connect, int *net, node *our_node, 
    id_struct *ip_tcp_chosen, id_struct *message_ip_tcp, char *tcp, fd_set fd_buffer, message_type *name);

void show_topology(node *our_node);

void show_names(node *our_node);

void show_interest_table(node *our_node, char* buffer);

void create(node *our_node, char *buffer, int cache);

void delete_obj(node *our_node, char *buffer);

void leave(node *our_node, int *net);

void exit();

int no_obj_ournode(node *our_node, char *buffer, message_type *name, char *buffer_out);

void retrive(node *our_node, char *buffer, int cache, message_type *name);

#endif