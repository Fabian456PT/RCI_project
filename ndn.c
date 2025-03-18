#include "head.h"

char buffer_fd[30];

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
    while(1){

        // update our select() function
        memcpy(&fd_read, &fd_buffer, sizeof(fd_set));

        if (select(our_node[0].biggest_socket + 1, &fd_read, (fd_set*)NULL,(fd_set*)NULL,(struct timeval*)NULL) < 0){
            printf("There was a error in select() function.\n");
        }

        // If this condition is true there is something to read
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
            get_message(buffer_fd, message_ip_tcp);

            after_someone_tried_to_connect(our_node, &newfd, message_ip_tcp, buffer_fd);
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
                int our_socket = direct_join(&go_direct_join, buffer, dj_connect, &net, our_node, ip_tcp_chosen, message_ip_tcp, tcp);   
                FD_SET(our_socket, &fd_buffer);              
            }
            // show topology
            else if(command == 5 || command == 6){
                show_topology(our_node);
            }
            // leave
            else if (command == 7 || command == 8){
                leave(our_node, &net);
            }

            else{
                printf("Invalid command line\n");
            }
        }
    } 
    
    return 0;
}