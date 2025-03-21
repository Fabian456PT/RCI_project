#include "head.h"
#include <unistd.h>

int join(char *buffer, id_struct *dj_connect, int *net, node *our_node, id_struct *ip_tcp_chosen, char *regip, char *regudp){

    // we are going to keep in a variable the terms of the j command line
    keep_commandline_values(buffer, 2, dj_connect, net, our_node);

    int join_fd = create_client_udp(regip, regudp, net, ip_tcp_chosen, our_node);
    our_node[0].udp_server_fd = join_fd;
    // Check which socket is bigger, to keep the bigger one
    if (join_fd > our_node[0].biggest_socket){
        our_node[0].biggest_socket = join_fd;
    }

    return join_fd;
}


int direct_join(int *go_direct_join, char *buffer, id_struct *dj_connect, int *net, node *our_node, 
                id_struct *ip_tcp_chosen, id_struct *message_ip_tcp, char *tcp){

    int our_socket;
    if(*go_direct_join == 0){ // if we did not write join first we just need to connect to someone we chose, so we are going to use dj_connect variable
        // we are going to keep in a variable the terms of the dj command line
        keep_commandline_values(buffer, 1, dj_connect, net, our_node);
    }
    
    char zero_ip[] = "0.0.0.0"; // variable used only for comparison

    // check if when writing dj we did not write 0.0.0.0 or when we wrote join we did not joinned an empty net
    if(strcmp(dj_connect[0].ip, zero_ip) != 0 && strcmp(ip_tcp_chosen->ip, zero_ip) != 0){ 
        if(*go_direct_join == 0){ // if we did not wrote join we send dj_connect variable to connect to someone we chose
            create_client_tcp(our_node, dj_connect, message_ip_tcp);
        }                
        else{// if we did wrote join we are going to connect to someone we chose from the node list using ip_tcp_chosen
            create_client_tcp(our_node, ip_tcp_chosen, message_ip_tcp);
        }
        our_socket = create_server_tcp(tcp); // set up as a server
        our_node[0].our_socket = our_socket; // registering our server socket

        // checking which socket is bigger, to keep the bigger one
        if (our_node[0].our_socket > our_node[0].biggest_socket){
            our_node[0].biggest_socket = our_node[0].our_socket;
        }
        return our_socket;
    }
    else{// here we wrote dj 0.0.0.0 or joinned an empty net
        printf("Just a server now\n");
        our_socket = create_server_tcp(tcp); // set up as a server
        our_node[0].our_socket = our_socket;

        // checking which socket is bigger, to keep the bigger one
        if (our_node[0].our_socket > our_node[0].biggest_socket){
            our_node[0].biggest_socket = our_node[0].our_socket;
        }
        return our_socket;
    }          
}


void show_topology(node *our_node){

    printf("Vizinho externo: IP: %s  TCP: %s\n", our_node[0].vzext.ip, our_node[0].vzext.tcp);
    printf("Vizinho salvaguarda: IP: %s  TCP: %s\n\n", our_node[0].vzsalv.ip, our_node[0].vzsalv.tcp);

    for(int i = 0; i < 16; i++){
        printf("Vizinho interno: IP: %s  TCP: %s\n", our_node[0].intr[i].ip, our_node[0].intr[i].tcp);
    }
}


void leave(node *our_node, int *net){

    socklen_t addrlen;
    struct sockaddr_in addr;
    char buffer[128];

    char buff_out[100];
    sprintf(buff_out,"UNREG %d %s %s", *net, our_node[0].id.ip, our_node[0].id.tcp);

    int n;
    n = sendto(our_node[0].udp_server_fd,buff_out,9,0,our_node[0].res->ai_addr,our_node[0].res->ai_addrlen);
    if(n == -1) /*error*/ exit(1);

    addrlen=sizeof(addr);

    n = recvfrom(our_node[0].udp_server_fd,buffer,128,0,
                (struct sockaddr*)&addr,&addrlen);
    if(n == -1) /*error*/ exit(1);
    write(1,"echo: ",7); write(1,buffer,n);

    close(our_node[0].udp_server_fd); 
    close(our_node[0].our_socket);
    close(our_node[0].salv_fd);
    close(our_node[0].ext_fd);
    for (int i=0; i < our_node->intr_num; i++) {
        close(our_node[0].intr_fd[i]);
    }
    
}

