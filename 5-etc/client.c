#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include "np_common.h"

#define BUF_SIZE 256
#define MONEY_DIGIT_SIZE 10

struct money{
    int deposit;
    int withdraw;
};

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
    char cmd[2] = "";                       // コマンド入力用
    struct money msgMoney;                  // 引き出し額/預け入れ額
    char money[BUF_SIZE];                   // 金額入力用
    int result;                             // 結果
    
    printf("0:引き出し　1:預け入れ　2:残高照会　9:終了\n");
    printf("何をしますか？ > ");
    my_scanf(cmd, 1);
    
    // 入力値によって処理を分岐
    switch (cmd[0]){
        case '0':
            // 引き出し処理
            printf("引き出す金額を入力してください > ");
            my_scanf(money, MONEY_DIGIT_SIZE);
            msgMoney.deposit = htonl(0);
            msgMoney.withdraw = htonl(atoi(money));
            break;
        case '1':
            //預け入れ処理
            printf("預け入れる金額を入力してください > ");
            my_scanf(money, MONEY_DIGIT_SIZE);
            msgMoney.deposit = htonl(atoi(money));
            msgMoney.withdraw = htonl(0);
            break;
        case '2':
            // 残高照会
            msgMoney.deposit = htonl(0);
            msgMoney.withdraw = htonl(0);
            break;
        default:
            // 終了
            printf("番号が確認できませんでした。\n");
            return;
    }

    printf("%lu バイト\n", sizeof(msgMoney));

    // 送信処理
    if(send(sock, &msgMoney, sizeof(msgMoney), 0) != sizeof(msgMoney))
        DieWithError("send() sent a message of unexpected bytes");

    // 受信処理
    read_certain_bytes(sock, &result, (int)sizeof(int));

    // ネットワークバイトオーダから変換
    result = ntohl(result);

    // 表示処理
    printf("残高は%d円になりました\n", result);
}