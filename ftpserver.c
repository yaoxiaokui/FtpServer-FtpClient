/*************************************************************************
	> File Name: serverWritev.c
	> Author: 
	> Mail: 
	> Created Time: Sun 29 May 2016 11:47:03 AM CST
 ************************************************************************/

//该宏是提示拥有splice函数的两个宏
#define _GNU_SOURCE
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
    if(argc < 3){
        printf("input: %s ip port", argv[0]);
        return -1;
    }

    const char * ip = argv[1];
    int port = atoi(argv[2]);

    //创建socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        printf("socket error\n");
        return -1;
    }

    //绑定socket和地址
    //先填写地址
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);
    //绑定地址
    int ret = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(ret != 0){
        printf("bind error\n");
        return -1;
    }

    //监听
    ret = listen(sockfd, 5);
    if(ret != 0){
        printf("listen error\n");
        return -1;
    }

    struct sockaddr_in clientaddr;
    bzero(&clientaddr, sizeof(clientaddr));
    socklen_t clientaddrLen;

    //等待请求连接
    while(1){
        int connfd = accept(sockfd, (struct sockaddr*)&clientaddr, &clientaddrLen);
        if(connfd < 0){
            printf("accept error\n");
            continue;
        }
    
        //如果accept成功，则将http的响应和要发送的文件的内容发送给客户端
        char file_name[1024];
        memset(file_name, 0, sizeof(file_name));

        int num = read(connfd, file_name, sizeof(file_name));
        if(num == 0){
            printf("read error\n");
            continue;
        }

        //获取文件的信息
        struct stat file_stat;
        ret = stat(file_name, &file_stat);
        if(ret != 0){
            printf("stat error\n");
        }


        //判断请求的是不是一个文件
        if(S_ISDIR(file_stat.st_mode)){//如果是常规的文件
            printf("请求的是目录，不是一个文件.\n");
            return -1;
        }

        //打开文件
        int filefd = open(file_name, O_RDONLY);
        if(filefd < 0){
            printf("open %s failed\n", file_name);
            close(connfd); 
            continue;
        }

        
        char filebuff[1024];
        memset(filebuff, 0, 1024);

        while((num = read(filefd, filebuff, sizeof(filebuff))) > 0){
            write(connfd, filebuff, num);
            //printf("write: %s\n", filebuff);
            memset(filebuff, 0, 1024);
        }

        printf("%s文件发送成功!\n", file_name);

        close(filefd);
        close(connfd);
    }

    return 0;
}
