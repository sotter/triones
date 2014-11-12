/******************************************************
*   FileName: main.cpp
*     Author: triones  2014-10-9
*Description:
*******************************************************/

#include "baseclient.h"
#include "comm/comlog.h"

uint64_t getId(struct sockaddr_in &_address)
{
	uint64_t ip = _address.sin_family;
	ip <<= 16;
	ip |= ntohs(_address.sin_port);
	ip <<= 32;
	ip |= _address.sin_addr.s_addr;

	return ip;
}

void getadress(uint64_t sockid, struct sockaddr_in &_address)
{
	_address.sin_family = (sockid >> 48);
	_address.sin_port = htons(sockid >> 32);
	_address.sin_addr.s_addr = sockid;
}

int main()
{
//	struct sockaddr_in address;
//	memset(&address, 0, sizeof(address));
//
//	address.sin_family = AF_INET;
//	address.sin_port  = htons(7008);
//	address.sin_addr.s_addr = inet_addr("192.168.100.49");
//
//	uint64_t sockid = getId(address);
//
//	printf("sockid = %llu \n", (unsigned long long )sockid);
//
//	struct sockaddr_in address1;
//	memset(&address1, 0, sizeof(address1));
//	getadress(sockid, address1);
//
//	int ret = memcmp(&address, &address1, sizeof(struct sockaddr_in));
//
//	printf("cmp ret = %d \n", ret);
//
//	printf("sockid1 = %llu \n", (unsigned long long )getId(address1));

	CHGLOG("client.log");

	BaseClient base_client;
	base_client.start("tcp:127.0.0.1:7406", 1);
	base_client.start("udp:127.0.0.1:7407", 1);

	while (1)
	{
		sleep(3);
	}

	base_client.destroy();

	OUT_INFO(NULL, 0, NULL, "good luck");

	sleep(4);

	LOGSTOP();


}


