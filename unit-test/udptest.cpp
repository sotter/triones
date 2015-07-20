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
#include <string>

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
	uint64_t sockid;
	triones_sockaddr sockaddr;
};

uint64_t sock_addr2id(struct sockaddr_in *sockaddr)
{
	seriaddr id;
	id.sockaddr.family =  sockaddr->sin_family;
	id.sockaddr.port = sockaddr->sin_port;
	id.sockaddr.host = sockaddr->sin_addr.s_addr;

	return id.sockid;
}

void sock_id2addr(uint64_t sockid, struct sockaddr_in *sockaddr)
{
	seriaddr addr;
	addr.sockid = sockid;
	sockaddr->sin_family = addr.sockaddr.family;
	sockaddr->sin_port = addr.sockaddr.port;
	sockaddr->sin_addr.s_addr = addr.sockaddr.host;

	return;
}

string sock_addr2str(struct sockaddr_in *sockaddr)
{
    char dest[32];
    unsigned long ad = ntohl(sockaddr->sin_addr.s_addr);
	const char *type = ntohs(sockaddr->sin_family) == SOCK_STREAM ? "tcp" : "udp";
    sprintf(dest, "%s:%d.%d.%d.%d:%d", type,
            static_cast<int>((ad >> 24) & 255),
            static_cast<int>((ad >> 16) & 255),
            static_cast<int>((ad >> 8) & 255),
            static_cast<int>(ad & 255),
            ntohs(sockaddr->sin_port));
    return dest;
}

string sock_id2str(uint64_t id)
{
	sockaddr_in sockaddr;
	sock_id2addr(id, &sockaddr);
	return sock_addr2str(&sockaddr);
}

uint64_t my_sock_addr2id(struct sockaddr_in *sockaddr, bool is_tcp, bool is_server)
{
	uint64_t sockid = 0;

	sockid <<= 1;
	sockid |= (is_tcp ? 1 : 0);

	sockid <<= 1;
	sockid |= (is_server ? 1: 0);

	sockid <<= 16;
	sockid |= sockaddr->sin_port;

	sockid <<= 32;
	sockid |= sockaddr->sin_addr.s_addr;

	return sockid;
}


void my_sock_id2addr(uint64_t sockid, struct sockaddr_in *sockaddr)
{
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = (sockid >> 32) & 0X0000FFFF;			// 取高32位的低16位
	sockaddr->sin_addr.s_addr = sockid & 0X00000000FFFFFFFF;	// 取低32位

	return;
}

std::string my_sock_addr2str(struct sockaddr_in *sockaddr)
{
	char dest[32];
	unsigned long ad = ntohl(sockaddr->sin_addr.s_addr);
//	const char *type = (sockaddr->sin_family) == SOCK_STREAM ? "tcp" : "udp";
	sprintf(dest, "%d.%d.%d.%d:%d", static_cast<int>((ad >> 24) & 255),
	        static_cast<int>((ad >> 16) & 255), static_cast<int>((ad >> 8) & 255),
	        static_cast<int>(ad & 255), ntohs(sockaddr->sin_port));
	return dest;
}

int main()
{
//	struct sockaddr_in toAddr;
//	memset(&toAddr, 0, sizeof(sockaddr_in));
//
//	toAddr.sin_family = AF_INET;
//	toAddr.sin_addr.s_addr = inet_addr("192.168.100.148");
//	toAddr.sin_port = htons(4000);
//
//	uint64_t n = sock_addr2id(&toAddr);
//
//	printf("n = %llu \n", n);
//
//	struct sockaddr_in Addr;
//	memset(&Addr, 0, sizeof(sockaddr_in));
//	sock_id2addr(n, &Addr);
//
//	int cmp = memcmp(&toAddr, &Addr, sizeof(uint64_t));
//	printf("cmp = %d \n", cmp);
//
//	uint64_t n2  = sock_addr2id(&Addr);
//
//	printf("n2 = %llu \n", n2);
//
//	printf("n2 string : %s \n", sock_id2str(n2).c_str());
//	printf("addr string: %s \n", sock_addr2str(&Addr).c_str());

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

	struct sockaddr_in from;
	struct sockaddr_in to;

	from.sin_family = AF_INET;
	from.sin_addr.s_addr = inet_addr("192.168.100.48");
	from.sin_port = htons(1234);

	uint64_t mid = my_sock_addr2id(&from, true, true);
	my_sock_id2addr(mid, &to);

	printf("addr string: %s \n", my_sock_addr2str(&from).c_str());
	printf("addr string: %s \n", my_sock_addr2str(&to).c_str());
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

