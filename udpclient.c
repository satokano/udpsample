#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    
    struct in_addr addr;
    struct in_addr *aptr;
    struct hostent *host;
    struct sockaddr_in server;
    struct servent *se;
    int soc;
    int portno;
    int len;
    char buf[512];
    struct sockaddr_in to;
    struct sockaddr_in from;
    int tolen;
    int fromlen;
    fd_set Mask;
    fd_set readOk;
    int width;
    struct timeval timeout;
    int error;
    
    /* 引数にホスト名、ポート番号が指定されているか */
    if (argc <= 2) {
        fprintf(stderr, "client server-host port\n");
        return 1;
    }
    
    /* ホスト名がIPアドレスと仮定してホスト情報取得 */
    if ((addr.s_addr = inet_addr(argv[1])) == -1) {
        /* ホスト名が名称としてホスト情報取得 */
        host = gethostbyname(argv[1]);
        if (host == NULL) {
            perror("gethostbyname");
            return -1; // 1と-1は？
        }
        aptr = (struct in_addr *)*host->h_addr_list;
        memcpy(&addr, aptr, sizeof(struct in_addr));
    }
    fprintf(stderr, "addr=%s\n", inet_ntoa(addr));
    
    /* ソケットの生成 */
    if ((soc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    
    /* ポート番号の決定 */
    memset((char *)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    if ((se = getservbyname(argv[2], "udp")) == NULL) {
        /* サービスに見つからない:ポート番号数値 */
        if ((portno = atoi(argv[2])) == 0) {
            fprintf(stderr, "bad port no\n");
            return -1;
        }
        server.sin_port = htons(portno);
    } else {
        /* サービスに見つかった:該当ポート番号 */
        server.sin_port = se->s_port;
    }
    printf("port=%d\n", ntohs(server.sin_port));
    
    /* ホストアドレスの指定 */
    server.sin_addr = addr;
    
    /* 送信先情報 */
    memcpy(&to, &server, sizeof(server));
    tolen = sizeof(server);
    
    /* select()用マスク */
    FD_ZERO(&Mask);
    FD_SET(soc, &Mask);
    FD_SET(0, &Mask);
    width = soc + 1;
}
