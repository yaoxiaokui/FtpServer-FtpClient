/*************************************************************************
	> File Name: FTP_CLIENT.c 
	> Author: 
	> Mail: 
	> Created Time: Sun 29 May 2016 11:47:03 AM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mysocket.h"

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
    Socket Sockfd(AF_INET, SOCK_STREAM, 0);

    //连接服务器
    int ret = Sockfd.Connect(AF_INET, ip, port);
    if(ret < 0){
        printf("connect error\n");
        return -1;
    }
    
    //先获得本地套接字，方便操作
    int sockfd = Sockfd.getSockfd();


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

    /*
    while((num = read(sockfd, filebuff, sizeof(filebuff))) > 0){
        //printf("read: %s\n", filebuff);
        write(filefd, filebuff, strlen(filebuff));
        memset(filebuff, 0, 1024);
    }
    */

    int file_len = 1000000000;//为了简单没有让服务器发送该文件的大小

    int pipefds[2];
    pipe(pipefds);

    //将socket文件描述符中的数据流向管道的写入端
    splice(sockfd, 0, pipefds[1], NULL, file_len, SPLICE_F_MOVE | SPLICE_F_MORE);

    //将管道的读出端pipefd[0]数据流向文件描述符
    splice(pipefds[0], NULL, filefd, NULL, file_len, SPLICE_F_MOVE | SPLICE_F_MORE);


    printf("%s文件接收成功,并保存为%s文件。\n", file_name, dest_name);
    
    close(filefd);
    close(sockfd);

    return 0;
}
