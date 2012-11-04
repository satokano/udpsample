#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    int soc;
    int portno;
    int opt;
    struct servent *se;
    struct sockaddr_in my;
    int len;
    struct sockaddr_in from;
    char buf[512];
    char *ptf;
    int fromlen;
    
    /* 引数にポート番号が指定されているか？ */
    if (argc <= 1) {
        fprintf(stderr, "server port\n");
        return 1;
    }
    
    
}
