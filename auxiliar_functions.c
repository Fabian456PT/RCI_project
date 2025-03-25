#include "head.h"
#include <asm-generic/socket.h>
#include <stddef.h>
#include <string.h>


void initialize_our_node(node *our_node, char *ip, char *tcp){

    memset(&our_node[0], 0, sizeof(our_node[0])); // initialize every byte of the node structure as zero
    // initialize some variables in our_node with values 
    strcpy(our_node[0].id.ip, ip); 
    strcpy(our_node[0].id.tcp, tcp);
    our_node[0].intr_num = 0; // initial number of inner nodes is 0

    our_node[0].biggest_socket = 0; 
    our_node[0].our_socket = -1; // variable used when we need to create a server
    our_node[0].ext_fd = -1; // our outer node socket
    our_node[0].salv_fd = -1; // our save node socket

}


void outer_node_left(node *our_node, fd_set fd_buffer){
    int new_ext_fd; // used if we are not the top node
    id_struct message_ip_tcp[2];
    id_struct dj_connect[1]; // variable used to send a ENTRY message 

    memset(&our_node[0].vzext, 0, sizeof(our_node[0].vzext)); // put vzext field as zero
    our_node[0].ext_fd = -1; // our outer node socket is now -1

    // If we are not at the top we just try to connect to our save node
    if(strcmp(our_node[0].id.ip, our_node[0].vzsalv.ip) != 0 || strcmp(our_node[0].id.tcp, our_node[0].vzsalv.tcp) != 0){
        strcpy(dj_connect->ip, our_node[0].vzsalv.ip);
        strcpy(dj_connect->tcp, our_node[0].vzsalv.tcp); 
        new_ext_fd = create_client_tcp(our_node, dj_connect, message_ip_tcp);
        our_node[0].ext_fd = new_ext_fd;
        FD_SET(new_ext_fd, &fd_buffer);

        // mandar mensagem de salvaguarda a todos os filhos internos
    }
    else{
        
    }


}


void read_stdin(char *buffer){
    int n = 99;

    fgets(buffer, n, stdin);

    fflush(0);// this fflush makes sure that if the user writes more than 99 characteres the rest goes to the trash

}

// this function keeps in a variable the terms of dj and j command lines
void keep_commandline_values(char *buffer, int number, id_struct *dj_connect, int *net, node *our_node){

    int a, b, c, d;
    if(number == 1){// check if the IP and TCP are acceptable

        // Check IP address but first skip all letters and go to the first digit
        buffer += strcspn(buffer, "0123456789");
        if (sscanf(buffer, "%d.%d.%d.%d", &a, &b, &c, &d) == 4 &&
            a >= 0 && a <= 255 && b >= 0 && b <= 255 &&
            c >= 0 && c <= 255 && d >= 0 && d <= 255){

            // Put IP as a string
            char ip_str[16];
            sprintf(ip_str, "%d.%d.%d.%d", a, b, c, d);
            strcpy(dj_connect[0].ip, ip_str);

            buffer += strcspn(buffer, " "); // Move to the first space after IP
            buffer += strspn(buffer, " ");  // Skip spaces

            // See what number comes after the IP
            int connect_tcp;
            if (sscanf(buffer, "%d", &connect_tcp) == 1 && connect_tcp > 0 && connect_tcp <= (65536 - 1)){
                char tcp_str[16];
                sprintf(tcp_str, "%d", connect_tcp);
                strcpy(dj_connect[0].tcp, tcp_str);
            } 
            else{
                printf("Invalid tcp connection.\n");
            }

            printf("ligar1: %s\n", dj_connect->ip);
            printf("ligar1: %s\n", dj_connect->tcp);
            
            return;
        } 

    }
    if(number == 2){ // check is the "net" is acceptable

        // Scan for a three-digit number in the string
        while (*buffer){ // reads until '/0'
            // here we only find atmost 3 numbers
            if (sscanf(buffer, "%3d", net) == 1 && net[0] >= 0 && net[0] <= 999){
                return;
            }
            buffer++; // Move to the next character
        }
        
    }
    
}

int verify_commandline(char *buffer){
    char *commands[] = {
        "direct join", "dj",           // direct join ou dj           1 & 2
        "join", "j",                   // join ou j                   3 & 4
        "show topology", "st",         // st ou show topology         5 & 6
        "leave", "l",                  // leave ou l                  7 & 8
        "create", "c",                 // create ou c                 9 & 10
        "delete", "dl",                // delete ou dl                11 & 12
        "retrive", "r",                // retrive ou r                13 & 14
        "show names", "sn",            // show names ou sn            15 & 16
        "show interest table", "si",   // show interest table ou si   17 & 18
        "exit", "x"                    // exit ou x                   19 & 20
    };

    // Trim leading and trailing spaces in the input buffer
    while (isspace((unsigned char)*buffer)) buffer++; // verifica se o primeiro caractere em buffer é um espaço em branco
    char *end = buffer + strlen(buffer) - 1;
    while (end > buffer && isspace((unsigned char)*end)) end--; // trim trailing spaces
    *(end + 1) = '\0'; // null-terminate after trimming

    // Check for long commands first (e.g., "direct join", "join", etc.)
    for (int i = 0; i < 20; i += 2) {
        // Check if the command matches, and ensure no extra characters are present
        if (strncmp(buffer, commands[i], strlen(commands[i])) == 0) {
            if (buffer[strlen(commands[i])] == '\0' || buffer[strlen(commands[i])] == ' ') {
                return i + 1;  // Return corresponding command number
            }
        }
    }

    // Check for short commands next (e.g., "dj", "j", etc.)
    for (int i = 1; i < 20; i += 2) {
        // Check if the command matches, and ensure no extra characters are present
        if (strncmp(buffer, commands[i], strlen(commands[i])) == 0) {
            if (buffer[strlen(commands[i])] == '\0' || buffer[strlen(commands[i])] == ' ') {
                return i + 1;  // Return corresponding command number
            }
        }
    }

    printf("Invalid command line\n");
    return 0;
}

void get_message(char *buffer, id_struct *message_ip_tcp){

    int a, b, c, d;
    char zero[] = "/0";

    //initialize the all as zero just in case we dont have an ENTRY message or SAFE message
    strcpy(message_ip_tcp[0].ip, zero);
    strcpy(message_ip_tcp[0].tcp, zero);
    strcpy(message_ip_tcp[1].ip, zero);
    strcpy(message_ip_tcp[1].tcp, zero);

    printf("buffer in  get message: %s\n", buffer);

    // Check for "ENTRY" prefix
    if (strncmp(buffer, "ENTRY", 5) == 0){
        buffer += 5;
        buffer += strspn(buffer, " \n"); // Skip spaces and newlines
        
        // Check IP address
        buffer += strcspn(buffer, "0123456789");
        if (sscanf(buffer, "%d.%d.%d.%d", &a, &b, &c, &d) == 4 &&
            a >= 0 && a <= 255 && b >= 0 && b <= 255 &&
            c >= 0 && c <= 255 && d >= 0 && d <= 255){
            
            char ip_str[16];
            sprintf(ip_str, "%d.%d.%d.%d", a, b, c, d);
            strcpy(message_ip_tcp[0].ip, ip_str);//saving entering ip

            buffer += strcspn(buffer, " ");// Move to the first space after IP
            buffer += strspn(buffer, " ");// Skip spaces

            int connect_tcp;
            if (sscanf(buffer, "%d", &connect_tcp) == 1 && connect_tcp > 0 && connect_tcp <= 65535){
                char tcp_str[16];
                sprintf(tcp_str, "%d", connect_tcp);
                strcpy(message_ip_tcp[0].tcp, tcp_str);//saving entering port
            } else {
                printf("Invalid ENTRY tcp connection.\n");
            }
        } else {
            printf("Invalid ENTRY IP format.\n");
        }
        //buffer += strcspn(buffer, "\n"); // Move to next line !!!!!!!!!!! ver se funciona sem isto
        buffer += strspn(buffer, " \n"); // Skip spaces and newlines
    }
    
    // Check for SAFE
    if (strncmp(buffer, "SAFE", 4) == 0){
        buffer += 4;
        buffer += strspn(buffer, " \n"); // Skip spaces and newlines
    } 
    else{//if we only have an ENTRY message we return
        return;
    }
    
    // Check IP address
    buffer += strcspn(buffer, "0123456789");// searching for the first number
    if (sscanf(buffer, "%d.%d.%d.%d", &a, &b, &c, &d) == 4 &&
        a >= 0 && a <= 255 && b >= 0 && b <= 255 &&
        c >= 0 && c <= 255 && d >= 0 && d <= 255){
        
        char ip_str[16];
        sprintf(ip_str, "%d.%d.%d.%d", a, b, c, d);
        strcpy(message_ip_tcp[1].ip, ip_str); //saving second ip 

        printf("passou aqui ip: %s\n", ip_str);

        buffer += strcspn(buffer, " ");
        buffer += strspn(buffer, " ");

        int connect_tcp;
        if (sscanf(buffer, "%d", &connect_tcp) == 1 && connect_tcp > 0 && connect_tcp <= 65535){
            char tcp_str[16];
            sprintf(tcp_str, "%d", connect_tcp);
            strcpy(message_ip_tcp[1].tcp, tcp_str);// saving second port 
            printf("passou aqui tcp: %s\n", tcp_str);
        } else {
            printf("Invalid SAFE tcp connection.\n");
        }
    } 
    else{
        printf("Invalid SAFE IP format.\n");
    }

    
    
    // If ENTRY was present, return its data, otherwise return SAFE
    return;
}

void get_ipandtcp_from_node_list(char *buffer, id_struct *ip_tcp_chosen){

    int a, b, c, d;
    char zero_ip[] = "0.0.0.0";

    // Skip firts line to get the first IP and TCP
    buffer += strcspn(buffer, "\n");
    if (sscanf(buffer, "%d.%d.%d.%d", &a, &b, &c, &d) == 4 &&
        a >= 0 && a <= 255 && b >= 0 && b <= 255 &&
        c >= 0 && c <= 255 && d >= 0 && d <= 255){

        // Put IP as a string
        char ip_str[16];
        sprintf(ip_str, "%d.%d.%d.%d", a, b, c, d);
        strcpy(ip_tcp_chosen->ip, ip_str);

        buffer += strcspn(buffer, " "); // Move to the first space after IP
        buffer += strspn(buffer, " ");  // Skip spaces

        // See what number comes after the IP
        int connect_tcp;
        if (sscanf(buffer, "%d", &connect_tcp) == 1 && connect_tcp > 0 && connect_tcp <= (65536 - 1)){
            char tcp_str[16];
            sprintf(tcp_str, "%d", connect_tcp);
            strcpy(ip_tcp_chosen->tcp, tcp_str);
        } 

        printf("chosen: %s\n", ip_tcp_chosen->ip);
        printf("chosen: %s\n", ip_tcp_chosen->tcp);
    } 
    else{
        strcpy(ip_tcp_chosen->ip, zero_ip);
        printf("There is no node to join you are going to be the first.\n");
    }
    return;

}


void after_someone_tried_to_connect(node *our_node, int *newfd, id_struct *message_ip_tcp, char *buffer_fd){

    int n;
    // if we dont have an outer node we chose the entering node as our outer node
    if(strcmp(our_node[0].vzext.ip, "") == 0 || strcmp(our_node[0].vzext.tcp, "") == 0){
        strcpy(our_node[0].vzext.ip, message_ip_tcp[0].ip);
        strcpy(our_node[0].vzext.tcp, message_ip_tcp[0].tcp);
        our_node[0].ext_fd = *newfd;   
        
        // Registering the new node as an inner node and its socket value
        strcpy(our_node[0].intr[our_node[0].intr_num].ip, message_ip_tcp[0].ip);
        strcpy(our_node[0].intr[our_node[0].intr_num].tcp, message_ip_tcp[0].tcp);
        our_node[0].intr_fd[our_node[0].intr_num] = *newfd; // keep the new inner socket
        our_node[0].intr_num++;// increasing the number of inner nodes

        // See which onde is the biggest socket for the select() function
        if(our_node[0].biggest_socket < *newfd){
            our_node[0].biggest_socket = *newfd;               
        }

        // Sending SAFE message (ENTRY our node SAFE our outer node which is the node that tried to connect)
        char buffer_entry[50];
        char buffer_safe[50];
        sprintf(buffer_entry, "ENTRY %s %s\n", our_node[0].id.ip, our_node[0].id.tcp);
        // sending ENTRY message
        n = write(*newfd, buffer_entry, 50);
        if(n == -1){
            printf("Error sending SAFE message\n");
            exit(1);
        }

        sprintf(buffer_entry, "SAFE %s %s\n", our_node[0].vzext.ip, our_node[0].vzext.tcp);
        // sending SAFE message
        n = write(*newfd, buffer_entry, 50);
        if(n == -1){
            printf("Error sending SAFE message\n");
            exit(1);
        }

        n = read(*newfd, buffer_fd, 30);
        if(n == -1){
            printf("error in read() function1.\n");
        }
        
        // Get safe message
        get_message(buffer_fd, message_ip_tcp);

        strcpy(our_node[0].vzsalv.ip, our_node[0].id.ip); 
        strcpy(our_node[0].vzsalv.tcp, our_node[0].id.tcp); 
    }
    else{
        // Registering the new node as an inner node and its socket value
        strcpy(our_node[0].intr[our_node[0].intr_num].ip, message_ip_tcp[0].ip);
        strcpy(our_node[0].intr[our_node[0].intr_num].tcp, message_ip_tcp[0].tcp);
        our_node[0].intr_fd[our_node[0].intr_num] = *newfd; // keep the new inner socket
        our_node[0].intr_num++;// increasing the number of inner nodes

        // See which onde is the biggest socket for the select() function
        if(our_node[0].biggest_socket < *newfd){
            our_node[0].biggest_socket = *newfd;               
        }

        // Sending SAFE message
        sprintf(buffer_fd, "SAFE %s %s\n", our_node[0].vzext.ip, our_node[0].vzext.tcp);
        n = write(*newfd, buffer_fd, 30);
        if(n == -1){
            printf("Error sending SAFE message\n");
            exit(1);
        }
    }

}