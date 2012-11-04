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
    
    /* ソケットの生成 */
    if ((soc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    
    /* ソケットオプション（再利用フラグ）設定 */
    opt = 1;
    if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(int)) != 0) {
        perror("setsockopt");
        return -1;
    }
    
    /* ポート番号の決定 */
    memset((char *)&my, 0, sizeof(my));
    my.sin_family = AF_INET;
    if ((se = getservbyname(argv[1], "udp")) == NULL) {
        /* サービスに見つからない:ポート番号数値 */
        if ((portno = atoi(argv[1])) == 0) {
            fprintf(stderr, "bad port no\n");
            return -1;
        }
        my.sin_port = htons(portno);
    } else {
        /* サービスに見つかった:該当ポート番号 */
        my.sin_port = se->s_port;
    }
    printf("port=%d\n", ntohs(my.sin_port));
    
    /* ソケットにアドレスを設定 */
    if (bind(soc, (struct sockaddr *)&my, sizeof(my)) == -1) {
        perror("bind");
        return -1;
    }
    
    /* 送受信 */
    while (1) {
        /* 受信 */
        fromlen = sizeof(from);
        if ((len = recvfrom(soc, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen)) < 0) {
            /* エラー */
            perror("recvfrom");
            break;
        }
        printf("recvfrom:%s:%d:len=%d\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port), len);
        /* 文字列化・表示 */
        buf[len] = '\0';
        if ((ptr = strpbrk(buf, "\r\n")) != NULL) {
            *ptr = '\0';
        }
        printf("[client]%s\n", buf);
        /* 応答文字列作成 */
        strcat(buf, ":OK\r\n");
        len = strlen(buf);
        /* 応答 */
        if ((len = sendto(soc, buf, len, 0, (struct sockaddr *)&from, fromlen)) < 0) {
            /* エラー */
            perror("sendto");
            break;
        }
    }
    
    /* ソケットクローズ */
    close(soc);
    
    return 0;
}
