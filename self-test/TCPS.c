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

int execute(char* cmd,char* buf)
{

    FILE* fp;
    char* errbuf="command cannot execute\n";
    int rect;
    int counter;
    fp=NULL;
    counter=0;
    memset(buf,0,BUFSIZE);

    if ((fp = popen(cmd, "r")) == NULL)
    {
        perror("open failed!");
        //即使不能机型也要发送数据，尽可能发送数据
        return -1;
    }
    //feof C 库函数 int feof(FILE *stream) 测试给定流 stream 的文件结束标识符。
    while((!feof(fp))&&(counter<2048))
    {
        buf[counter]=fgetc(fp);
        counter++;     
    }

    if(0>rect){
        return -1;
    }
    return counter;

}


void print_err(char *str, int line, int err_no) {
	printf("%d, %s :%s\n",line,str,strerror(err_no));
	_exit(-1);
}

/*子线程中先接收从客户端发来的消息，再发送一个消息给客户端*/
void *receive(void *pth_arg) {
	int ret = 0;
	long sfd = (long)pth_arg;
	char buf[2048];
    char send_buf[2048];
    int sendt = 0;
	while(1) {
		bzero(&buf, BUFSIZE);
        bzero(&send_buf, BUFSIZE);
		ret = recv(sfd, &buf, sizeof(buf),0);	
		if (-1 == ret) {
			printf("recv failed");
		}
		else if (ret > 0){
			printf("recv from client %s \n",buf);
            if (0==strcmp(buf,"quit"))
		    {
			    perror("quitting remote controling\n");
                ret = send(sfd,"quit", sizeof("quit"), 0);
		    }else{
            sendt = execute(buf,send_buf);
		    ret = send(sfd,send_buf, sizeof(send_buf), 0);
            }
		    if (-1 == ret) printf("send failed");
        }     
	}
}

int main()
{
	int sockfd = -1, ret = -1;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if ( -1 == sockfd) {
		printf("socket failed");
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET; //设置tcp协议族
	addr.sin_port = htons(PORT); //设置端口号
	addr.sin_addr.s_addr =INADDR_ANY; //设置ip地址
	ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if ( -1 == ret) {
		printf("bind failed");
	}
	ret = listen(sockfd, 3);
	if ( -1 == ret ) {
		printf("listen failed");
	}	
	//使用accept阻塞形式得监听客户端的发来的连接，并返回通信描述符
	long connected = -1;
	pthread_t id;
	while (1) {
		struct sockaddr_in client = {0};
		int size = sizeof(client);
		connected = accept(sockfd, (struct sockaddr*)&client, &size);
		if (-1 == connected) {
			printf("accept failed");
		}
        printf("tcp>>");
		//使用accept返回到描述符，创建子线程进行数据传输
		int ret = pthread_create(&id,NULL,receive,(void*)connected);
		if(-1 == ret) printf("accept failed"); 
	}
	return 0;
}
