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
    
    /* �����Ƀz�X�g���A�|�[�g�ԍ����w�肳��Ă��邩 */
    if (argc <= 2) {
        fprintf(stderr, "client server-host port\n");
        return 1;
    }
    
    /* �z�X�g����IP�A�h���X�Ɖ��肵�ăz�X�g���擾 */
    if ((addr.s_addr = inet_addr(argv[1])) == -1) {
        /* �z�X�g�������̂Ƃ��ăz�X�g���擾 */
        host = gethostbyname(argv[1]);
        if (host == NULL) {
            perror("gethostbyname");
            return -1; // 1��-1�́H
        }
        aptr = (struct in_addr *)*host->h_addr_list;
        memcpy(&addr, aptr, sizeof(struct in_addr));
    }
    fprintf(stderr, "addr=%s\n", inet_ntoa(addr));
    
    /* �\�P�b�g�̐��� */
    if ((soc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    
    /* �|�[�g�ԍ��̌��� */
    memset((char *)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    if ((se = getservbyname(argv[2], "udp")) == NULL) {
        /* �T�[�r�X�Ɍ�����Ȃ�:�|�[�g�ԍ����l */
        if ((portno = atoi(argv[2])) == 0) {
            fprintf(stderr, "bad port no\n");
            return -1;
        }
        server.sin_port = htons(portno);
    } else {
        /* �T�[�r�X�Ɍ�������:�Y���|�[�g�ԍ� */
        server.sin_port = se->s_port;
    }
    printf("port=%d\n", ntohs(server.sin_port));
    
    /* �z�X�g�A�h���X�̎w�� */
    server.sin_addr = addr;
    
    /* ���M���� */
    memcpy(&to, &server, sizeof(server));
    tolen = sizeof(server);
    
    /* select()�p�}�X�N */
    FD_ZERO(&Mask);
    FD_SET(soc, &Mask);
    FD_SET(0, &Mask);
    width = soc + 1;
    
    /* ����M */
    error = 0;
    while (1) {
        readOk = Mask;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        switch (select(width, (fd_set *)&readOk, NULL, NULL, &timeout)) {
        case -1:
            perror("select");
            break;
        case 0:
            break;
        default:
            if (FD_ISSET(0, &readOk)) {
                /* �W�����̓��f�B */
                /* �W�����͂���1�s�ǂݍ��� */
                fgets(buf, sizeof(buf), stdin);
                if (feof(stdin)) {
                    error = 1;
                    break;
                }
                /* ���M */
                if ((len = sendto(soc, buf, strlen(buf), 0, (struct sockaddr *)&to, tolen)) < 0) {
                    /* �G���[ */
                    perror("sendto");
                    error = 1;
                    break;
                }
            }
            if (FD_ISSET(soc, &readOk)) {
                /* �\�P�b�g���f�B */
                /* ��M */
                fromlen = sizeof(from);
                if ((len = recvfrom(soc, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen)) < 0) {
                    /* �G���[ */
                    perror("recvfrom");
                    error = 1;
                    break;
                }
                /* �����񉻁E�\�� */
                buf[len] = '\0';
                printf("> %s", buf);
            }
        }
        if (error) {
            break;
        }
    }
    
    /* �\�P�b�g�N���[�Y */
    close(soc);
    
    return 0;
}
