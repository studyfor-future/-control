#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>


#define PORT 12000
#define BUFSIZE 2048

void print_err(char *str, int line, int err_no) {
	printf("%d, %s :%s\n",line,str,strerror(err_no));
	_exit(-1);
}

int main(int argc,char** argv)
{
	int len;
	int port;
	int sockfd;
    if ((2>argc)|| (argc >3))
	{
		exit(1);
	}
       if (3==argc) 
       {
		port = atoi(argv[2]);
       }
	int ret = -1;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( -1 == sockfd) {
		print_err("socket failed",__LINE__,errno);
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET; //设置tcp协议族
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(port);
	
	//主动发送连接请求
	ret = connect(sockfd,(struct sockaddr*)&server, sizeof(server));
	if(-1 == ret) print_err("connect failed", __LINE__, errno);

	char send_buf[2048] = {0};
	char recv_buf[2048] = {0};
	//客户端发送消息，并接受从服务端返回的消息
	while (1) {
        printf("send data>>");
		bzero(&send_buf, BUFSIZE);
		scanf("%s",send_buf);

		ret = send(sockfd,&send_buf,sizeof(send_buf), 0);
		if (-1 == ret) {
			print_err("send failed", __LINE__, errno);
		}
		bzero(&recv_buf, sizeof(recv_buf));
		ret = recv(sockfd, &recv_buf, sizeof(recv_buf), 0);
		if(-1 == ret) print_err("recv failed", __LINE__, errno);
		else if(ret > 0){
            printf("recv from server %s\n",recv_buf);
            if (0==memcmp(recv_buf,"quit",4))
            {
            perror("the break the client process\n");
            close(sockfd);
            exit(-1);
            }	
        }
	}
	
	return 0;
}
