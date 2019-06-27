#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>


#define BUF_SIZE 256

void DieWithError(char *errorMessage) {
	perror(errorMessage);
	exit(1);
}

void commun (int sock) {
	char buf[BUF_SIZE];
	int len_r;
	if ((len_r = recv(sock, buf, BUF_SIZE, 0)) <= 0)
		DieWithError(“recv() failed”);
	buf[len_r] = ‘\0’;
	printf( “%s\n”, buf );
	
	if (send(sock, buf, strlen(buf), 0) != strlen(buf))
		DieWithError(“send() sent a message of unexpected bytes”);
}

int main(int arg0, char **argv) {
	int buf_size = 512;
	char buf[buf_size];
	
	/***********************/
	/*  クライアント情報の保存用  */
	/***********************/
	struct sockaddr_in clientAddress
	unsinged int szClientAddr;
	int cliSock;

	/***********************/
	/*  待ち受けソケットの作成  */
	/***********************/
	int servSock = socket(PF_INET, SOCK_STREAM, 0);

	/******************/
	/*  マシン情報作成  */
	/******************/
	struct sockaddr_in servAddress;

	servAddress.sin_family      = PF_INET;
	servAddress.sin_port        = (in_port_t)htons(80);  // 待ち受けソケットのポート番号
	servAddress.sin_addr.s_addr = htonl(INADDR_ANY);        // 待ち受けるための（自分の）IPアドレス（普段はINADDR_ANY）

	/*********************************/
	/* 待ち受けソケットにマシンの情報をセット  */
	/*********************************/
	bind(servSock, (struct sockaddr *)&servAddress, sizeof(servAddress));

	/********************/
	/*   待ち受け開始   */
	/********************/
	listen(servSock, 5);

	while(1) {
		szClientAddr = sizeof(clientAddress);
		cliSock = accept(servSock, (struct sockaddr *)&clientAddress, &szClientAddr);
		
		/****************/
		/* 　　通信開始    */
		/****************/
		
		commun(cliSock);

		close(cliSock);
	}

	/********************/
	/* 待受用ソケットを */
	/* 閉じる           */
	/********************/
	close(servSock);

	return 0;
}
