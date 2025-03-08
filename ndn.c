#include "head.h"

///// IP é o nosso e o TCP é a porta que tou a usar pra ouvir, nossa tb

int main(int argc, char *argv[]){

    if(argc != 6){
        exit(0);
    }

    int cache = atoi(argv[1]);
    printf("%d\n", cache);
    
    char *ip = argv[2];
    printf("%s\n", ip);

    char *tcp = argv[3];
    printf("%s\n", tcp);
    
    char *regip = argv[4];
    printf("%s\n", regip);

    char *regudp = argv[5];
    printf("%s\n",regudp);


    // por aqui um loop infinito para estar á espera das funções (interface)

    ssize_t n;
    fd_set fd_commandline;
    FD_ZERO(&fd_commandline);
    FD_SET(0,&fd_commandline);

    n = select(1, &fd_commandline, (fd_set*)NULL,(fd_set*)NULL,(struct timeval*)NULL);

    // If the condition is true it means that there was an error
    if (n == -1){
        printf("There was a error typing the command line\n");
    }

    // Here we initialize our node

    node our_node[1];

    memset(&our_node[0], 0, sizeof(our_node[0])); // initialize every byte of the node structure as zero
    // initialize some variables in our_node with values 
    strcpy(our_node[0].id.ip, ip); 
    strcpy(our_node[0].id.tcp, tcp);
    our_node[0].intr_num = 0;  

    // If this condition is true there was an exception
    if (FD_ISSET(0, &fd_commandline) != 0){

        char buffer[100];
        read_stdin(buffer);// this function gets the command line the user wrote and put it in a buffer

        // here we are going to check what command line the user wrote
        int command = verify_commandline(buffer);

        //here we are going to take the rest information about the command line (ip and tcp, if command line is just st or show topology we just do the command)

        int net[1] = {0}; // variable used if the user writes join or j
        id_struct dj_connect[1]; // variable used to keep the ip and tcp when the user writes direct join or dj

        if(command == 1){
            verify_ip_tcp_connection(buffer, 1, dj_connect, net, our_node);
        }
        else if(command == 2){
            verify_ip_tcp_connection(buffer, 2, dj_connect, net, our_node);
        }
        else if(command == 3){
            verify_ip_tcp_connection(buffer, 3, dj_connect, net, our_node);
        }
        else if(command == 4){
            verify_ip_tcp_connection(buffer, 4, dj_connect, net, our_node);
        }
        else if(command == 5){
            verify_ip_tcp_connection(buffer, 5, dj_connect, net, our_node);
        }
        else if(command == 6){
            verify_ip_tcp_connection(buffer, 6, dj_connect, net, our_node);
        }
        else{
            printf("Invalid command line\n");
        }
    }

    //VER CONDIÇOES PARA IP E TCP DE ENTRADA PQ TALVEZ SO SEJA NECESSÁRIO CHARA A FUNÇAO CREATE SERVER

    //int fd_client = create_client_tcp(ip, tcp);
    int fd_server = create_server_tcp("estrutura dos nós op");


    // por loop infinito aqui pró select QUE VAI ESTAR Á ESCUTA DE PESSOAS PRA SE LIGAR E RECEBER MENSAGENS DE SALVAGUARDA E TAMBEM QUANDO ALGUEM SAI DE MIM
    
    
    return 0;
}
