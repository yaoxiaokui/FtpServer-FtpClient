/*************************************************************************
	> File Name: FTP_SERVER.c
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
#include <sys/sendfile.h>
#include "mysocket.h"

int main(int argc, char *argv[])
{
    if(argc < 3){
        printf("input: %s ip port", argv[0]);
        return -1;
    }

    const char * ip = argv[1];
    int port = atoi(argv[2]);

    //创建socket对象
    Socket sockfd(AF_INET, SOCK_STREAM, 0);

    //绑定socket和地址
    sockfd.Bind(AF_INET, ip, port);

    //监听
    sockfd.Listen(5);


    //等待请求连接
    while(1){
        int connfd = sockfd.Accept();
        if(connfd < 0){
            printf("accept error\n");
            continue;
        }
        sockfd.printHostaddr();
        sockfd.printPeeraddr();
    
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
        int ret = stat(file_name, &file_stat);
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

        
        //使用splice发送文件
        int pipefds[2];
        pipe(pipefds);
        //将文件中的数据流向管道的输入端pipefds[1]
        splice(filefd, NULL, pipefds[1], NULL, file_stat.st_size, SPLICE_F_MOVE | SPLICE_F_MORE);

        //从管道的输出端pipefds[0]流向连接socket 
        splice(pipefds[0], NULL, connfd, NULL, file_stat.st_size, SPLICE_F_MOVE | SPLICE_F_MORE);


        printf("%s文件发送成功!\n", file_name);

        close(filefd);
        close(connfd);
    }

    return 0;
}
