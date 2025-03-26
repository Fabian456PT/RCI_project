#include "head.h"
#include <stdio.h>
#include <string.h>
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
                id_struct *ip_tcp_chosen, id_struct *message_ip_tcp, char *tcp, fd_set fd_buffer, message_type *name){

    int our_socket;
    if(*go_direct_join == 0){ // if we did not write join first we just need to connect to someone we chose, so we are going to use dj_connect variable
        // we are going to keep in a variable the terms of the dj command line
        keep_commandline_values(buffer, 1, dj_connect, net, our_node);
    }
    
    char zero_ip[] = "0.0.0.0"; // variable used only for comparison

    // check if when writing dj we did not write 0.0.0.0 or when we wrote join we did not joinned an empty net
    if(strcmp(dj_connect[0].ip, zero_ip) != 0 && strcmp(ip_tcp_chosen->ip, zero_ip) != 0){ 
        if(*go_direct_join == 0){ // if we did not wrote join we send dj_connect variable to connect to someone we chose
          int ext_fd = create_client_tcp(our_node, dj_connect, message_ip_tcp, name);
          our_node[0].ext_fd = ext_fd;
          FD_SET(ext_fd, &fd_buffer);
        }                
        else{// if we did wrote join we are going to connect to someone we chose from the node list using ip_tcp_chosen
            int ext_fd = create_client_tcp(our_node, ip_tcp_chosen, message_ip_tcp, name);
            our_node[0].ext_fd = ext_fd;
            FD_SET(ext_fd, &fd_buffer);
        }
        our_socket = create_server_tcp(tcp); // set up as a server
        our_node[0].our_socket = our_socket; // registering our server socket

        // checking which socket is bigger, to keep the bigger one
        if (our_node[0].our_socket > our_node[0].biggest_socket){
            our_node[0].biggest_socket = our_node[0].our_socket;
        }
        if (our_node[0].ext_fd > our_node[0].biggest_socket){
            our_node[0].biggest_socket = our_node[0].ext_fd;
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
    // falta mandar protocolo de saida
}

void create(node *our_node, char *buffer, int cache) {
    // Check if we have space in the cache
    if (our_node->objects_num >= cache) {  
        printf("Error: Cache is full! Cannot store more objects.\n");
        return;
    }

    // Skip the "create" command and leading spaces
    buffer += strcspn(buffer, " "); // skip for the first space
    buffer += strspn(buffer, " ");  // skip all spaces

    int name_len = strcspn(buffer, " \n"); // Get the length of the object name

    // Check if the name is too long
    if (name_len > 100) {
        printf("Error: Object name is too long!\n");
        return;
    }

    // Store the object name in the node structure
    strncpy(our_node->object[our_node->objects_num], buffer, name_len);
    our_node->object[our_node->objects_num][name_len] = '\0'; // Make sure the string is null-terminated

    // Increment the counter for the stored objects
    our_node->objects_num++;

    printf("Object created: %s\n", our_node->object[our_node->objects_num - 1]);
}

void delete_obj(node *our_node, char *buffer){
    
    buffer += strcspn(buffer, " "); // skip for the first space
    buffer += strspn(buffer, " ");  // skip all spaces
    //printf("encontrei-te boi %s\n", buffer);
    for (int i=0; i<our_node[0].objects_num; i++) {
        if (strncmp(buffer, our_node[0].object[i], strlen(buffer)) == 0) { // compare the 2 strings 

            for (int j = i+1; j < our_node[0].objects_num; j++) {
                strncpy(our_node[0].object[i], our_node[0].object[j], 101); //shift the objects to the left, in the array
            }

            // decrease the number of the objects registed in our node
            our_node[0].objects_num --;

            printf("object deleted %s\n", buffer);
            return;
        }
        
    }
    printf("Object not found: %s\n", buffer);
}

void show_names(node *our_node){
    
    printf("saved objects in this node:\n");
    
    for (int i=0; i<our_node[0].objects_num; i++) {
        printf("%s\n", our_node[0].object[i]);
    }
}    

void retrive(node *our_node, char *buffer, int cache){
    buffer += strcspn(buffer, " "); // skip for the first space
    buffer += strspn(buffer, " ");  // skip all spaces

    char buffer_out[115];
    int flag = 0;

    // verify if we have the object in our node 
    for (int i= 0; i < our_node[0].objects_num; i++) {
        if(strcmp(our_node[0].object[i], buffer) == 0 /*|| strcmp(our_node[0].object[i], o que vai chegar do salva)*/){     
            
            printf("encontrado objeto: %s\n", buffer);
            sprintf(buffer_out, "OBJECT %s", buffer);
            //write(/*socket correspondente ao vizinho que mandou o interesse*/, buff_out, 7 + strlen(buffer));
            flag = 1; // object found
        }
    }   

    if (flag == 1) {  // eliminar da tabela de interesses
        for (int i=0; i<our_node[0].interest_num; i++) {
            if (strncmp(buffer, our_node[0].interest[i], strlen(buffer)) == 0) { // compare the 2 strings 
    
                for (int j = i+1; j < our_node[0].interest_num; j++) {
                    strncpy(our_node[0].interest[i], our_node[0].interest[j], 101); //shift the objects to the left, in the array
                }
    
                // decrease the number of the objects registed in our node
                our_node[0].interest_num --;
    
                printf("object deleted from interst table %s\n", buffer);
                return;
            }
            
        }
    }

    if (flag == 0) {
        sprintf(buffer_out,"INTEREST %s", buffer);
        write(our_node[0].ext_fd, buffer_out,  9+ strlen(buffer));

        for (int j = 0; j < our_node[0].interest_num; j++) {
            sprintf(buffer_out,"INTEREST %s", buffer);
            write(our_node[0].intr_fd[j], buffer_out,  9+ strlen(buffer));
        }

        printf("nao foi encontrado o objeto: %s\n", buffer);
        // verify if we have already registed the interest
        for (int i = 0; i < our_node[0].interest_num; i++) {
            
            if (strcmp(our_node[0].interest[i], buffer) == 0) {
                printf("Interesse já registado.\n");
                return;
            }
                
        }
        
        strncpy(our_node[0].interest[our_node[0].interest_num],buffer, 101);    
        our_node[0].interest_num++;    

    }

}

void show_interest_table(node *our_node, char* buffer){
    printf("Pendent interests: \n");
    for (int i = 0; i < our_node[0].interest_num; i++) {
        printf("%s\n", our_node[0].interest[i]);
    }
}