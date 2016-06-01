/*************************************************************************
	> File Name: mysocket.h
	> Author: 
	> Mail: 
	> Created Time: Wed 01 Jun 2016 02:49:17 PM CST
 ************************************************************************/

#ifndef _MYSOCKET_H
#define _MYSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/***************************************************
*
*   类名称：Socket
*   类功能: socket套接字函数的封装
*
**************************************************/
class Socket
{
private:
    int sockfd;
    struct sockaddr_in hostaddr;
    struct sockaddr_in peeraddr;

public:
    Socket(int domain, int type, int protocol);
    ~Socket();
    int Bind(int domain, const char *ip, int port);
    int Listen(int backlog);
    int Accept();
    int Connect(int domain, const char *ip, int port);

    int getSockfd();
    struct sockaddr_in getHostaddr() const;
    struct sockaddr_in getPeeraddr() const;
    void printHostaddr() const;
    void printPeeraddr() const;

};



/****************************************************************
*   函数名称：Socket(int domain, int type, int protocol)
*   功能描述: 构造函数 
*   参数列表: domain是使用的协议，type是服务类型，protocol一般为0
*   返回结果：无
*****************************************************************/
Socket::Socket(int domain, int type, int protocol)
{
    bzero(&hostaddr, sizeof(hostaddr));
    bzero(&peeraddr, sizeof(peeraddr));

    sockfd = socket(domain, type, protocol);
    if(sockfd < 0){
        printf("socket error\n");
    }
}

/****************************************************************
*   函数名称：~Socket()
*   功能描述: 析构函数 
*   参数列表: 
*   返回结果：
*****************************************************************/
Socket::~Socket()
{
    close(sockfd);
}

/****************************************************************
*   函数名称：int Bind(int domain, const char *ip, int port)
*   功能描述: 将套接字和地址绑定 
*   参数列表: domain是协议族，ip是要绑定的ip地址，port是要绑定的端口号
*   返回结果：0表示成功，-1表示失败
*****************************************************************/
int Socket::Bind(int domain, const char *ip, int port)
{
    bzero(&hostaddr, sizeof(hostaddr));
    //填写地址
    hostaddr.sin_family = domain;
    inet_pton(domain, ip, &hostaddr.sin_addr);
    hostaddr.sin_port = htons(port);
    //绑定地址
    int ret = bind(sockfd, (struct sockaddr*)&hostaddr, sizeof(hostaddr));
    if(ret != 0){
        printf("bind error\n");
        return -1;
    }

    return 0;
}

/****************************************************************
*   函数名称：int Listen(int backlog)
*   功能描述: 监听套接字，创建监听队列 
*   参数列表: backlog是监听队列的最大长度
*   返回结果：0表示成功，-1表示失败
*****************************************************************/
int Socket::Listen(int backlog)
{
    int ret = listen(sockfd, backlog);
    if(ret != 0){
        printf("listen error\n");
        return -1;
    }

    return 0;
}

/****************************************************************
*   函数名称：int Accept()
*   功能描述: 从listen监听队列中接受一个连接 
*   参数列表: 
*   返回结果：-1表示失败,大于0表示返回的是连接socket
*****************************************************************/
int Socket::Accept()
{
    socklen_t peeraddrlen;

    int connfd = accept(sockfd, (struct sockaddr*)&peeraddr, &peeraddrlen);
    if(connfd < 0){
        printf("accept error\n");
        return -1;
    }
    return connfd;
}

/****************************************************************
*   函数名称：int Connect(int domain, const char *ip, int port)
*   功能描述: 请求与服务器建立连接
*   参数列表: domain是协议族,ip是要连接的服务器ip，
*             port是要连接的服务器的端口号
*   返回结果：-1表示失败,0表示成功
*****************************************************************/
int Socket::Connect(int domain, const char *ip, int port)
{
    //先填写地址
    bzero(&peeraddr, sizeof(peeraddr));

    peeraddr.sin_family = domain;
    inet_pton(domain, ip, &peeraddr.sin_addr);
    peeraddr.sin_port = htons(port);

    int ret = connect(sockfd, (struct sockaddr*)&peeraddr, sizeof(peeraddr));
    if(ret < 0){
        printf("connect error\n");
        close(sockfd);
        return -1;
    }
    return ret;
    
}


/****************************************************************
*   函数名称：printHostaddr()const
*   功能描述: 打印本地地址 
*   参数列表: 
*   返回结果：
*****************************************************************/
void Socket::printHostaddr() const
{
    printf("本地地址：%s:%d\n", inet_ntoa(hostaddr.sin_addr), ntohs(hostaddr.sin_port));
}

/****************************************************************
*   函数名称：printPeeraddr()const
*   功能描述: 打印对端地址 
*   参数列表: 
*   返回结果：
*****************************************************************/
void Socket::printPeeraddr() const
{
    printf("对端地址: %s:%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
}

/****************************************************************
*   函数名称：getHostaddr()const
*   功能描述: 返回本地地址 
*   参数列表: 
*   返回结果：
*****************************************************************/
struct sockaddr_in Socket::getHostaddr() const
{
    return hostaddr; 
}

/****************************************************************
*   函数名称：getPeeraddr()const
*   功能描述: 返回对端地址 
*   参数列表: 
*   返回结果：
*****************************************************************/
struct sockaddr_in Socket::getPeeraddr() const
{
    return peeraddr; 
}

/****************************************************************
*   函数名称：getSockfd()
*   功能描述: 返回sockfd 
*   参数列表: 
*   返回结果：
*****************************************************************/
int Socket::getSockfd()
{
    return sockfd;
}


#endif
