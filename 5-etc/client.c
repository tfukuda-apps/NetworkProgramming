#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include "np_common.h"

#define BUF_SIZE 256

void commun(int);

int main(int argc, char *argv[]) {
    // 実行時にはサーバのIPアドレスとポートを添えなければならない
    if (argc != 3)
        DieWithError("usage: ./client ip_address port");
    
    // ソケット生成
    int sock = prepare_client_socket(argv[1], atoi(argv[2]));

    // サーバとの通信
    commun(sock);

    // 通信終了
    close(sock);

    return 0;
}

void commun(int sock) {
    int len_r = 0;
    char buf[BUF_SIZE];

    // 受信して表示
    len_r = recv(sock, buf, BUF_SIZE, 0);
    buf[len_r] = '\0';
    printf("%-10s: %s\n", "someone", buf);

    // 入力した内容を送信
    my_scanf(buf, BUF_SIZE);
    send(sock, buf, strlen(buf), 0);
    
}
