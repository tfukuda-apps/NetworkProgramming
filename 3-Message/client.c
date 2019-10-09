#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 256
#define MONEY_DIGIT_SIZE 10

void DieWithError(char *errorMessage);
int prepare_client_socket(char *ipaddr, int port);
void my_scanf(char *buf, int num_letter);
void commun(int sock);

int main(int argc, char *argv[]) {
    char cmd[2] = "";   // コマンド入力用
    char withdraw[MONEY_DIGIT_SIZE+1];  // 引き出し額
    char deposit[MONEY_DIGIT_SIZE+1];   // 預け入れ額
    char msg[BUF_SIZE]; // 送信メッセージ

    if (argc != 3)
        DieWithError("usage: ./client ip_address port");
    
    int sock = prepare_client_socket(argv[1], atoi(argv[2]));

    commun(sock);

    close(sock);

    return 0;
}

void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

int prepare_client_socket(char *ipaddr, int port) {
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        DieWithError("socket() failed");
    
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr(ipaddr);
    target.sin_port = htons(port);
    if (connect(sock, (struct sockaddr*)&target, sizeof(target)) < 0)
        DieWithError("connect() failed");

    return sock;
}

void my_scanf(char *buf, int num_letter) {
    // num_letter文字だけ入力させる（scanfの脆弱性対策）
    char format[20];
    sprintf(format, "%s%d%s", "%", num_letter, "s%*[^\n]");
    scanf(format, buf);
    getchar();
}

void commun(int sock) {
}