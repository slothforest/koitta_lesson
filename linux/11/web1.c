#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE		1024
#define SMALL_BUF		100

typedef struct sockaddr_in		si;
typedef struct sockaddr *		sp;

void err_handler(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void send_error(FILE *fp)
{
	char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
	char server[] = "Server:Linux Web Server\r\n";
	char cnt_len[] = "Content-length:2048\r\n";
	char cnt_type[] = "Content-type:text/html\r\n\r\n";
	char content[] = "<html><head><title>Web Network</title></head>"
					"<body><font size=+5><br>Error Occurance! Need to Check"
					"</font></body></html>";

	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);
	//fputs(content, fp);
	fflush(fp);
}

void *content_type(char *file)
{
	char extension[SMALL_BUF];
	char file_name[SMALL_BUF];
	strcpy(file_name, file);//매개변수 file을 file_name로 복사
	strtok(file_name, ".");//file_name token 분리 첫번째 strtok NULL 반환
	strcpy(extension, strtok(NULL, ".")); //strtok . 다음 포인터를 반환 해당 포인터 뒤로 문자열을 extenstion에 복사
											//확장자를 나누기위해서 위 함수 사용

	if(!strcmp(extension, "html") || !strcmp(extension, "htm"))
		return (void *)"text/html";
	else
		return (void *)"text/plain";
}

void send_data(FILE *fp, char *ct, char *file_name)
{
	char protocol[] = "HTTP/1.0 200 OK\r\n";
	char server[] = "Server:Linux Web Server\r\n";
	char cnt_len[] = "Content-Length:2048\r\n";
	char cnt_type[SMALL_BUF] = "";
	char buf[BUF_SIZE] = "";
	char *send_file;

	sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);
	send_file = fopen(file_name, "r");

	if(send_file == NULL)
	{
		send_error(fp);
		return;
	}

	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);

	while(fgets(buf, BUF_SIZE, send_file) != NULL)
	{
		fputs(buf, fp);
		fflush(fp);
	}

	fflush(fp);
	fclose(fp);
}

void *request_handler(void *arg)
{
	int clnt_sock = *((int *)arg);
	char req_line[SMALL_BUF];
	FILE *clnt_read;
	FILE *clnt_write;

	char method[10];
	char ct[15];
	char file_name[30];

	clnt_read = fdopen(clnt_sock, "r");
	clnt_write = fdopen(dup(clnt_sock), "w");
	fgets(req_line, SMALL_BUF, clnt_read);

	if(strstr(req_line, "HTTP/") == NULL)
	{
		send_error(clnt_write);
		fclose(clnt_read);
		fclose(clnt_write);
		return;
	}

	strcpy(method, strtok(req_line, " /"));
	strcpy(file_name, strtok(NULL, " /"));
	strcpy(ct, content_type(file_name));

	if(strcmp(method, "GET") != 0)
	{
		send_error(clnt_write);
		fclose(clnt_read);
		fclose(clnt_write);
		return;
	}

	fclose(clnt_read);
	send_data(clnt_write, ct, file_name);
}

int main(int argc, char **argv)
{
	int serv_sock, clnt_sock;
	si serv_addr, clnt_addr;
	int clnt_addr_size;
	char buf[BUF_SIZE];
	pthread_t t_id;

	if(argc != 2)
	{
		printf("Use: %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (sp)&serv_addr, sizeof(serv_addr)) == -1)
		err_handler("bind() error");

	if(listen(serv_sock, 30) == -1)
		err_handler("listen() error");

	for(;;)
	{
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (sp)&clnt_addr, &clnt_addr_size);
		printf("Connection Request: %s:%d\n",
			inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

		pthread_create(&t_id, NULL, request_handler, &clnt_sock);
		pthread_detach(t_id);	
	}

	close(serv_sock);

	return 0;
}
