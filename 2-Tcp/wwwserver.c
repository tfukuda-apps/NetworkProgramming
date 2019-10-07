#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <regex.h>

#define BUF_SIZE 256

void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

void commun(int sock) {
    printf("start");
    char buf[BUF_SIZE];
    char buf_old[BUF_SIZE];
    char buf2[2*BUF_SIZE];
    int len_r;
	char response[BUF_SIZE];

    regex_t regBuf;
    regmatch_t regMatch[1];	
    const char *pattern = "GET / HTTP";
    char result[100];
    char *uri;

	buf_old[0] = '\0';
    result[0] = '\0';

    if (regcomp(&regBuf, pattern, REG_EXTENDED | REG_NEWLINE) != 0) {
        DieWithError("regcomp failed.");
    }

    while((len_r = recv(sock, buf, BUF_SIZE, 0)) > 0){
        buf[len_r] = '\0';
        sprintf(buf2, "%s%s", buf_old, buf);

        printf("%s", buf);

        if ( regexec(&regBuf, buf2, 1, regMatch, 0) == 0 ){
        printf("%s", buf2);
	        int startIndex = regMatch[0].rm_so;
	        int endIndex = regMatch[0].rm_eo;
        	int lenReqLine = endIndex - startIndex;
        	strncpy(result, buf2 + startIndex, lenReqLine);
        	result[lenReqLine] = '\0';
        }

        if (strstr(buf2, "\r\n\r\n")) {
            break;
        }
		
		sprintf(buf_old, "%s", buf);
    }
    regfree(&regBuf);

    if (result[0] != '\0') {
        uri = strtok(result, " ");
        uri = strtok(NULL, " ");
        printf("%s\n", uri);
    } else {
        DieWithError("No URI.");
    }

    if (len_r <= 0)
        DieWithError("received() failed.");
    
    printf("received HTTP Request.\n");

    sprintf(response, "HTTP/1.1 200 OK\r\n");
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
    
    sprintf(response, "Content-Type: text/html; charset=utf-8\r\n");
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
        
    sprintf(response, "\r\n");
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
    
    sprintf(response, "<!DOCTYPE html><html><head><title>");
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
    
    sprintf(response, "ネットワークプログラミングのwebサイト");
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
    
    sprintf(response, "</title></head><body>ネットワークダイスキ</body></html>");
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");

}

int main(int argc, char *argv[]) {
	struct sockaddr_in clientAddress;
	unsigned int szClientAddr;
	int cliSock;

	int servSock = socket(PF_INET, SOCK_STREAM, 0);
    if (servSock < 0)
        DieWithError("socket() failed");
    
    struct sockaddr_in servAddress;
    servAddress.sin_family = AF_INET;
    servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddress.sin_port = htons(80);

	bind(servSock, (struct sockaddr*)&servAddress, sizeof(servAddress));

	listen(servSock, 5);

	while (1) {
		szClientAddr = sizeof(clientAddress);
		cliSock = accept(servSock, (struct sockaddr *)&clientAddress, &szClientAddr);

		commun(cliSock);

		close(cliSock);
	}

    close(servSock);
    return 0;
}