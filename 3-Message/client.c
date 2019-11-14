#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 256
#define MONEY_DIGIT_SIZE 10

void DieWithError(char *);
int prepare_client_socket(char *, int);
void my_scanf(char *, int);
void commun(int);
void read_until_delim(int, char *, char, int);
void read_certain_bytes(int, void *, int);

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

// エラーが生じた場合にメッセージを表示して終了
void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

// 通信用ソケットの生成
int prepare_client_socket(char *ipaddr, int port) {
	// ソケット生成
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        DieWithError("socket() failed");
    
	// サーバの情報を設定
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr(ipaddr);
    target.sin_port = htons(port);

	// サーバへ接続
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


// 区切り文字が出るまで、または最大文字数を受信するまで受信を繰り返す
// ただしmax_lengthはバッファサイズ。末尾に必ず'\0'をつけて返す。
void read_until_delim(int sock, char *buf, char delimiter, int max_length) {
	int len_r = 0;			// 受信文字数
	int index_letter = 0;	// 受信文字数の合計

	while (index_letter < max_length - 1) {
		// 1文字だけ受信
        if ((len_r = recv(sock, buf + index_letter, 1, 0)) <= 0)
			DieWithError("recv() failed");

		// 受信した文字が区切り文字ならループを抜ける
		if (buf[index_letter] == delimiter)
			break;
		else
			index_letter++;
	}
	// nullを末尾に追加
	buf[index_letter] = '\0';
}

// 特定のバイト数だけ受信する
void read_certain_bytes(int sock, void *buf, int length) {
    int len_r = 0;
    int len_sum = 0;

    while (len_sum < length) {
        if ((len_r = recv(sock, buf + len_sum, length - len_sum, 0)) <= 0)
			DieWithError("recv() failed");
        len_sum += len_r;
    }
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
            msgMoney.deposit = 0;
            msgMoney.withdraw = atoi(money);
            break;
        case '1':
            //預け入れ処理
            printf("預け入れる金額を入力してください > ");
            my_scanf(money, MONEY_DIGIT_SIZE);
            msgMoney.deposit = atoi(money);
            msgMoney.withdraw = 0;
            break;
        case '2':
            // 残高照会
            msgMoney.deposit = 0;
            msgMoney.withdraw = 0;
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

    // 表示処理
    printf("残高は%d円になりました\n", result);
}