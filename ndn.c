#include "head.h"
#include <complex.h>
#include <stdlib.h>

char buffer_fd[50];

///// IP é o nosso e o TCP é a porta que tou a usar pra ouvir, nossa tb

int main(int argc, char *argv[]){

    if(argc != 6){
        exit(0);
    }

    int cache = atoi(argv[1]);  // cache  
    char *ip = argv[2]; // our ip
    char *tcp = argv[3]; // our tcp 
    char *regip = argv[4];
    char *regudp = argv[5];

    // Here we initialize our node

    node our_node[1];

    initialize_our_node(our_node, ip, tcp);

    // this is for the select() funtioc used to get messages 
    fd_set fd_buffer;
    fd_set fd_read;
    FD_ZERO(&fd_buffer);
    FD_SET(0,&fd_buffer);
    FD_SET(our_node[0].our_socket,&fd_buffer);

    int newfd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    id_struct message_ip_tcp[2];//variable used when receiving messages from possible clients
    id_struct ip_tcp_chosen[1]; // used when join to choose a node from the node list
    message_type name[1];
    while(1){

        // update our select() function
        memcpy(&fd_read, &fd_buffer, sizeof(fd_set));

        if (select(our_node[0].biggest_socket + 1, &fd_read, (fd_set*)NULL,(fd_set*)NULL,(struct timeval*)NULL) < 0){
            printf("There was a error in select() function.\n");
        }

        if(FD_ISSET(our_node[0].ext_fd, &fd_read) != 0){
            char buffer[50];
            // if our outer node left
            if(read(our_node[0].ext_fd, buffer, 50) == 0){
                FD_CLR(our_node[0].ext_fd, &fd_read); 
                outer_node_left(our_node, fd_buffer, name);
            }
            else{ 
                read(our_node[0].ext_fd, buffer, 50);
                get_message(buffer, message_ip_tcp, name);
                // in case we receive a SAFE message
                if(strcmp(message_ip_tcp[1].ip, "\0") != 0){
                    // update our safe node
                    strcpy(our_node[0].vzsalv.ip, message_ip_tcp[1].ip);
                    strcpy(our_node[0].vzsalv.tcp, message_ip_tcp[1].tcp);
                }
                else{
                    name[0].flag = -1; // just making sure that is the outer node that is receiving something to read, we say that putting "flag" as "-1"
                    retrive(our_node, NULL, cache, name);
                }
                
            }
        }
        // Checking if any of our inner nodes left or objects moment
        for(int i = 0; i < our_node[0].intr_num; i++){
            if(FD_ISSET(our_node[0].intr_fd[i], &fd_read) != 0){
                char buffer[50];
                // if we read "0" from one of our inner nodes socket that means that it left
                if(read(our_node[0].intr_fd[i], buffer, 50) == 0){
                    one_inner_node_left(our_node, i);
                }
                // if not is maybe an object
                else{
                    get_message(buffer, message_ip_tcp, name);
                    name[0].flag = i;
                    retrive(our_node, NULL, cache, name);
                }
            }
        }
        // If this condition is true there is something to read (someone is trying to connect)
        if (FD_ISSET(our_node[0].our_socket, &fd_read) != 0){

            printf("Someone is trying to connect\n");

            // Some possible inner node is trying to connect
            newfd = accept(our_node[0].our_socket, (struct sockaddr*)&addr, &addrlen);

            ssize_t n;
            n = read(newfd, buffer_fd, 30);
            if(n == -1){
                printf("error in read() function1.\n");
            }

            // ENTRY message that some possible inner node is trying to give           
            get_message(buffer_fd, message_ip_tcp, name);

            after_someone_tried_to_connect(our_node, &newfd, message_ip_tcp, buffer_fd, name);
            FD_SET(newfd, &fd_buffer); // add a new socket to read


        }

        else if (FD_ISSET(0, &fd_read) != 0){

            char buffer[100];
            read_stdin(buffer);// this function gets the command line the user wrote and put it in a buffer

            // here we are going to check what command line the user wrote
            int command = verify_commandline(buffer);

            if(command == 0){
                printf("Error typing the command line\n");
                return 0;
            }

            int net; // variable used if the user writes join or j
            id_struct dj_connect[1]; // variable used to keep the ip and tcp when the user writes direct join or dj
            int go_direct_join = 0; // variable used when we did join and need to go to direct join after

            // join
            if(command == 3 || command == 4){
                int join_fd = join(buffer, dj_connect, &net, our_node, ip_tcp_chosen, regip, regudp);  
                go_direct_join = 1;             
                FD_SET(join_fd, &fd_buffer);

            }
            // if the command line is direct join or it was join, we enter here
            if(command == 1 || command == 2 || go_direct_join == 1){
                int our_socket = direct_join(&go_direct_join, buffer, dj_connect, &net, our_node, ip_tcp_chosen, message_ip_tcp, tcp, fd_buffer, name);   
                FD_SET(our_socket, &fd_buffer);              
            }
            // show topology
            else if(command == 5 || command == 6){
                show_topology(our_node);
            }
            // show names 
            else if (command == 15|| command == 16) {
                show_names(our_node);
            }
            //create
            else if (command == 9 || command == 10) {
                create(our_node, buffer, cache);
            }
            //delete
            else if (command == 11 || command == 12) {
                delete_obj(our_node, buffer);
            }
            // leave
            else if (command == 7 || command == 8){
                leave(our_node, &net);
            }
            // exit
            else if (command == 19 || command == 20) {
                exit(0);
            }
            else{
                printf("Invalid command line\n");
            }
        }
    } 
    
    return 0;
}