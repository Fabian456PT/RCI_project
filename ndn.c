#include "head.h"

///// ENUNCIADO

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

    //VER CONDIÇOES PARA IP E TCP DE ENTRADA PQ TALVEZ SO SEJA NECESSÁRIO CHARA A FUNÇAO CREATE SERVER

    //iniciar estrutura dos nós
    int fd_client = create_client(ip, tcp);
    int fd_server = create_server("estrutura dos nós op");


    // por loop infinito aqui pró select QUE VAI ESTAR Á ESCUTA DE PESSOAS PRA SE LIGAR E RECEBER MENSAGENS DE SALVAGUARDA E TAMBEM QUANDO ALGUEM SAI DE MIM
    

    
    
    return 0;
}




/*int main(int argc, char *argv[]){

    struct sockaddr_in addr1, addr2, addr3;

    int fd1 = socket(AF_INET, SOCK_STREAM, 0);
    if (fd1 == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    int fd2 = socket(AF_INET, SOCK_STREAM, 0);
    if (fd2 == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    int fd3 = socket(AF_INET, SOCK_STREAM, 0);
    if (fd3 == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    int maxfd;
    if (fd1 >= fd2 && fd1 >= fd3) {
        maxfd = fd1;
    } else if (fd2 >= fd1 && fd2 >= fd3) {
        maxfd = fd2;
    } else {
        maxfd = fd3;
    }



    int counter=select(maxfd+1,&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval*)NULL);

    return 0;
}*/