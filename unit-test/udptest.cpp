/******************************************************
 *   FileName: udptest.cpp
 *     Author: triones  2014-10-21
 *Description:
 *******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <map>

using namespace std;
//
//int udp_server(const char *host);
//int udp_client(const char *host);

//发送64KB的数据包
#define TEST_PACK_SIZE (1024 * 128)

//void *thd_server(void *arg)
//{
//	(void)udp_server("127.0.0.1");
//	return NULL;
//}
//
//void *thd_client(void *arg)
//{
//	(void)udp_client("127.0.0.1");
//	return NULL;
//}

struct triones_sockaddr
{
	unsigned short family;
	unsigned short port;
	unsigned int   host;
};

union seriaddr
{
	uint64_t seri;
	triones_sockaddr sockaddr;
};



uint64_t seri_sockaddr(struct sockaddr_in *sockaddr)
{

//	uint64_t n = 0;
//	n |= sockaddr->sin_family << 48;
//	n |= sockaddr->sin_port << 32;
//	n |= sockaddr->sin_addr.s_addr;


	seriaddr addr;
	addr.sockaddr.family =  sockaddr->sin_family;
	addr.sockaddr.port = sockaddr->sin_port;
	addr.sockaddr.host = sockaddr->sin_addr.s_addr;

//	memcpy(&addr, &sockaddr, sizeof(addr));
//	printf("sizeof(addr)= %d \n", sizeof(addr));
//	printf("sizeof(addr)= %d \n", sizeof(sockaddr->sin_family));
//	printf("sizeof(addr)= %d \n", sizeof(sockaddr->sin_port));
//	printf("sizeof(addr)= %d \n", sizeof(sockaddr->sin_addr));

	return addr.seri;
}

void reseri_sockaddr(uint64_t n, struct sockaddr_in *sockaddr)
{
//	sockaddr->sin_family = n >> 48;
//	sockaddr->sin_port = (n >> 32) & (0xffff);
//	sockaddr->sin_addr.s_addr = n & 0xffffffff;

	seriaddr addr;
	addr.seri = n;
	sockaddr->sin_family = addr.sockaddr.family;
	sockaddr->sin_port = addr.sockaddr.port;
	sockaddr->sin_addr.s_addr = addr.sockaddr.host;
}

int main()
{
	struct sockaddr_in toAddr;
	memset(&toAddr, 0, sizeof(sockaddr_in));

	toAddr.sin_family = AF_INET;
	toAddr.sin_addr.s_addr = inet_addr("192.168.100.148");
	toAddr.sin_port = htons(4000);

	uint64_t n = seri_sockaddr(&toAddr);

	printf("n = %llu \n", n);

	struct sockaddr_in Addr;
	memset(&Addr, 0, sizeof(sockaddr_in));
	reseri_sockaddr(n, &Addr);

	int cmp = memcmp(&toAddr, &Addr, sizeof(uint64_t));
	printf("cmp = %d \n", cmp);

	uint64_t n2  = seri_sockaddr(&Addr);

	printf("n2 = %llu \n", n2);


//	pthread_t t1, t2;
//	pthread_create(&t1, NULL, thd_server, NULL);
//
//	sleep(1);
//	pthread_create(&t2, NULL, thd_client, NULL);
//
//	while(1)
//	{
//		sleep(100);
//	}

	return 0;
}

//bool setIntOpt(int fd, int option, int value)
//{
//	return (setsockopt(fd, SOL_SOCKET, option, (const void*)(&value), sizeof(value)) == 0);
//}
//
//int udp_server(const char *host)
//{
//	int sock;
//	//sendto中使用的对方地址
//
//	struct sockaddr_in toAddr;
//	//在recvfrom中使用的对方主机地址
//	struct sockaddr_in fromAddr;
//	int recvLen;
//	unsigned int addrLen;
//	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//	if (sock < 0)
//	{
//		printf("创建套接字失败了.\r\n");
//		exit(0);
//	}
//	if( ! setIntOpt(sock, SO_SNDBUF, TEST_PACK_SIZE * 2))
//	{
//		printf("setintopt fail %s \n", strerror(errno));
//	}
//	setIntOpt(sock, SO_RCVBUF, TEST_PACK_SIZE * 2);
//
//	memset(&fromAddr, 0, sizeof(fromAddr));
//	fromAddr.sin_family = AF_INET;
//	fromAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	fromAddr.sin_port = htons(4000);
//	if (bind(sock, (struct sockaddr*) &fromAddr, sizeof(fromAddr)) < 0)
//	{
//		printf("bind() 函数使用失败了.\r\n");
//		close(sock);
//		exit(1);
//	}
//
//	char *recvBuffer = (char *)malloc(TEST_PACK_SIZE);
//
//	while (1)
//	{
//		addrLen = sizeof(toAddr);
//		if ((recvLen = recvfrom(sock, recvBuffer, TEST_PACK_SIZE, 0, (struct sockaddr*) &toAddr, &addrLen))
//		        < 0)
//		{
//			printf("server - recv len %d \r\n", recvLen);
//			close(sock);
//			exit(1);
//		}
//
//		printf("server - recv len %d \r\n", recvLen);
//
//		if (sendto(sock, recvBuffer, recvLen, 0, (struct sockaddr*) &toAddr, sizeof(toAddr))
//		        != recvLen)
//		{
//			printf("sendto fail\r\n");
//			close(sock);
//			exit(0);
//		}
//	}
//
//	free(recvBuffer);
//
//	return 0;
//}
//
//int udp_client(const char *host)
//{
//	int sock;
////sendto中使用的对方地址
//	struct sockaddr_in toAddr;
////在recvfrom中使用的对方主机地址
//	struct sockaddr_in fromAddr;
//	unsigned int fromLen;
//
//
//	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//	if (sock < 0)
//	{
//		printf("创建套接字失败了.\r\n");
//		exit(1);
//	}
//
//	setIntOpt(sock, SO_SNDBUF, TEST_PACK_SIZE * 2);
//	setIntOpt(sock, SO_RCVBUF, TEST_PACK_SIZE * 2);
//
//	memset(&toAddr, 0, sizeof(toAddr));
//	toAddr.sin_family = AF_INET;
//	toAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
//	toAddr.sin_port = htons(4000);
//
//	char *recvBuffer = (char *)malloc(TEST_PACK_SIZE);
//	memset(recvBuffer, 0x31, TEST_PACK_SIZE);
//
//	int send_len = sendto(sock, recvBuffer, TEST_PACK_SIZE, 0, (struct sockaddr*) &toAddr, sizeof(toAddr));
//
//	printf("client-sendto len %d: %s \n", send_len, strerror(errno));
//
//	if(send_len < 0)
//	{
//		close(sock);
//		exit(1);
//	}
//
//	fromLen = sizeof(fromAddr);
//	int recv_len = recvfrom(sock, recvBuffer, TEST_PACK_SIZE, 0, (struct sockaddr*) &fromAddr, &fromLen) ;
//	printf("client - recv() fail :%s \r\n", recv_len, strerror(errno));
//
//	if(recv_len < 0)
//	{
//		close(sock);
//		exit(1);
//	}
//
//	close(sock);
//}

