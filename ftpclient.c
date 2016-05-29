/*************************************************************************
	> File Name: FTP_CLIENT.c 
	> Author: 
	> Mail: 
	> Created Time: Sun 29 May 2016 11:47:03 AM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    if(argc < 5){
        printf("input: %s ip port file_name dest_name", argv[0]);
        return -1;
    }

    const char * ip = argv[1];
    int port = atoi(argv[2]);
    const char * file_name = argv[3];
    const char * dest_name = argv[4];

    //创建socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        printf("socket error\n");
        return -1;
    }

    //先填写地址
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    //连接服务器
    int ret = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(ret < 0){
        printf("connect error\n");
        close(sockfd);
        return -1;
    }
    
    //向服务器发送要获取的文件名字
    int num = write(sockfd, file_name, strlen(file_name));
    if(num == 0){
        printf("write error\n");
        return -1;
    }

    //先创建一个文件
    int filefd = open(dest_name, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(filefd < 0){
        printf("create %s error\n",  file_name);
    }

    //读取服务器发送的文件内容
    char filebuff[1024];
    memset(filebuff, 0, 1024);

    while((num = read(sockfd, filebuff, sizeof(filebuff))) > 0){
        //printf("read: %s\n", filebuff);
        write(filefd, filebuff, strlen(filebuff));
        memset(filebuff, 0, 1024);
    }

    printf("%s文件接收成功,并保存为%s文件。\n", file_name, dest_name);
    
    close(filefd);
    close(sockfd);

    return 0;
}
