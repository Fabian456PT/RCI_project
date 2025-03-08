#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct NdnConfig {
    int cache_size;
    char ip[16];
    int tcp_port;
    char reg_ip[16];
    int reg_udp;
};

int arguments(int argc, int *argv[]){
    if(argc != 6){
        printf("Usage: %s <cache_size> <ip> <tcp_port> <reg_ip> <reg_udp>\n", argv[0]);
        return 1;
    }

    printf("numero da porta: %d\n", *argv[3]);
    return 0;
}