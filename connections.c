#include "head.h"

int create_client_udp(char *regip, char *regudp, int *net, id_struct *ip_tcp_chosen, node *our_node){

    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[128];

    fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd == -1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errcode = getaddrinfo(regip,regudp,&hints,&res);
    if(errcode != 0) /*error*/ exit(1);

    our_node[0].res = res;// keep res value

    char buffer_join[30];
    sprintf(buffer_join, "NODES %03d", *net);
    n = sendto(fd,buffer_join,9,0,res->ai_addr,res->ai_addrlen);
    if(n == -1) /*error*/ exit(1);

    char list[1000];
    addrlen = sizeof(addr);
    n = recvfrom(fd,list,1000,0,(struct sockaddr*)&addr,&addrlen);

    printf("list: %s", list);

    get_ipandtcp_from_node_list(list, ip_tcp_chosen);

    char buffer_entry[50];
    sprintf(buffer_entry, "REG  %d %s %s", *net, our_node[0].id.ip, our_node[0].id.tcp);
    n = sendto(fd,buffer_entry,50,0,res->ai_addr,res->ai_addrlen);
    if(n == -1) /*error*/ exit(1);

    addrlen = sizeof(addr);
    n = recvfrom(fd,buffer,128,0,
    (struct sockaddr*)&addr,&addrlen);
    if(n == -1) /*error*/ exit(1);
    write(1,"echo: ",6); write(1,buffer,n);
    
    freeaddrinfo(res);

    return fd;
}


int create_client_tcp(node *our_node, id_struct *dj_connect, id_struct *message_ip_tcp){

    int fd,errcode;
    //socklen_t addrlen;
    struct addrinfo hints,*res;
    //struct sockaddr_in addr;
    char buffer[500];

    fd = socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (fd == -1) exit(1); //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode = getaddrinfo(dj_connect->ip, dj_connect->tcp, &hints, &res);
    if(errcode != 0)/*error*/exit(1);
    
    ssize_t n;
    n = connect(fd,res->ai_addr,res->ai_addrlen);
    if(n == -1)/*error*/exit(1);
    
    char message_buff[50];
    sprintf(message_buff, "ENTRY %s %s\n", our_node[0].id.ip, our_node[0].id.tcp);
    printf("mensagem: %s\n", message_buff);  
    n = write(fd,message_buff,strlen(message_buff));
    if(n == -1)/*error*/exit(1);

    // if we dont have an outer node
    // Here we use the select() function to wait for the SAFE message
    ssize_t counter;
    fd_set fd_connect;
    FD_ZERO(&fd_connect);
    FD_SET(fd, &fd_connect);

    int fd_max = fd + 1;

    counter = select(fd_max, &fd_connect, (fd_set*)NULL,(fd_set*)NULL,(struct timeval*)NULL);

    // If the condition is true it means that there was an error
    if (counter == -1){
        printf("There was a error receiving SAFE message!!\n");
    }

    // We receive the SAFE message
    n = read(fd,buffer,500);
    if(n == -1)/*error*/exit(1);

    // Saving our outer node
    strcpy(our_node[0].vzext.ip, dj_connect->ip); 
    strcpy(our_node[0].vzext.tcp, dj_connect->tcp);

    // write the SAFE message in our_node variable
    get_message(buffer,message_ip_tcp);

    if(strcmp(message_ip_tcp[0].ip, "/0") == 0){// if we just received a SAFE message and not a ENTRY + SAFE
        // Saving "salvaguarda" node 
        strcpy(our_node[0].vzsalv.ip, message_ip_tcp[1].ip);
        strcpy(our_node[0].vzsalv.tcp, message_ip_tcp[1].tcp);

        printf("IP salvaguarda: %s\n", our_node[0].vzsalv.ip);
        printf("TCP salvaguarda: %s\n", our_node[0].vzsalv.tcp);

    }
    else{// if we receive a ENTRY message e are going to also receive a SAFE message

        // If we receive both ENTRY and SAFE message at the same time
        if(strcmp(message_ip_tcp[0].ip, "/0") != 0 && strcmp(message_ip_tcp[1].ip, "/0") != 0){
            // Saving as our inner node
            strcpy(our_node[0].intr[our_node[0].intr_num].ip, dj_connect->ip);
            strcpy(our_node[0].intr[our_node[0].intr_num].tcp, dj_connect->tcp);
            our_node[0].intr_fd[our_node[0].intr_num] = fd; // keep the new inner socket
            our_node[0].intr_num++;// increasing the number of inner nodes

            // Saving "salvaguarda" node 
            strcpy(our_node[0].vzsalv.ip, message_ip_tcp[1].ip);
            strcpy(our_node[0].vzsalv.tcp, message_ip_tcp[1].tcp);

            //send SAFE message
            char buff[30];
            sprintf(buff, "SAFE %s %s\n", our_node[0].vzext.ip, our_node[0].vzext.tcp);
            n = write(fd, buff, 30);
            if(n == -1){
                printf("Error sending SAFE message\n");
                exit(1);
            }
        }
        else{
            // Here we just receive only the ENTRY message first and we are waiting for the SAFE message
            if(strcmp(message_ip_tcp[0].ip, "/0") != 0){
                // Saving as our inner node
                strcpy(our_node[0].intr[our_node[0].intr_num].ip, dj_connect->ip);
                strcpy(our_node[0].intr[our_node[0].intr_num].tcp, dj_connect->tcp);
                our_node[0].intr_fd[our_node[0].intr_num] = fd; // keep the new inner socket
                our_node[0].intr_num++;// increasing the number of inner nodes
            }
            // We receive the SAFE message
            n = read(fd,buffer,128);
            if(n == -1)/*error*/exit(1);

            //SAFE MESSAGE arrived
            get_message(buffer,message_ip_tcp);

            // Saving "salvaguarda" node 
            strcpy(our_node[0].vzsalv.ip, message_ip_tcp[1].ip);
            strcpy(our_node[0].vzsalv.tcp, message_ip_tcp[1].tcp);

            //send SAFE message
            char buff[30];
            sprintf(buff, "SAFE %s %s\n", our_node[0].vzext.ip, our_node[0].vzext.tcp);
            n = write(fd, buff, 30);
            if(n == -1){
                printf("Error sending SAFE message\n");
                exit(1);
            }

        }
    }

    write(1,"echo: ",7); 
    write(1,buffer,n);
    printf("\n");

    freeaddrinfo(res);

    //close(fd);
    return fd;
}


int create_server_tcp(char *tcp){

    int fd,errcode;
    ssize_t n;
    struct addrinfo hints,*res;
    char buffer[128];
    int newfd;

    fd = socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (fd == -1) exit(1); //error

    int reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    hints.ai_flags=AI_PASSIVE;

    errcode = getaddrinfo(NULL,tcp,&hints,&res);
    if((errcode) != 0)/*error*/exit(1);

    n = bind(fd,res->ai_addr,res->ai_addrlen);

    if(n == -1) /*error*/ exit(1);

    if(listen(fd,5) == -1)/*error*/exit(1);



    return fd;
}
