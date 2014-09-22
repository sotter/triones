/**
 * author: Triones
 * date  : 2014-08-28
 */

#include <cstdint>
#include <string>
#include <cstring>
#include <net/if.h>
#include <sys/ioctl.h>
#include "net.h"

namespace triones
{

bool ipv4_str_to_addr(const char *ip, struct in_addr &addr /* out */)
{
	if(1 != inet_pton(AF_INET, ip, &addr))
	{
		return false;
	}

	return true;
}

bool ipv4_addr_to_str(const struct in_addr &addr, std::string &ip_str/* out */)
{
	char ip_s[INET_ADDRSTRLEN];
	memset(ip_s, 0, sizeof(ip_s));

	if(NULL == inet_ntop(AF_INET, &addr, ip_s, sizeof(ip_s)))
	{
		return false;
	}

	ip_str.assign(ip_s);

	return true;
}

// 得到设备的 IP 地址
bool ipv4_get_addr(const char *dev_name, struct in_addr &addr /* out */, bool skip_loopback/* = true*/)
{
	int fd, i;
	struct ifreq  ifs[16];
	struct ifconf ifc;
	struct sockaddr_in* sa = NULL;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
	{
		return false;
	}

	ifc.ifc_len = sizeof(ifs);
	ifc.ifc_buf = (caddr_t)ifs;

	if (ioctl(fd, SIOCGIFCONF, &ifc) < 0)
	{
		close(fd);
		return false;
	}

	i = ifc.ifc_len / sizeof(struct ifreq);
	for (; i > 0; --i)
	{
		if (ioctl(fd, SIOCGIFFLAGS, &ifs[i]) < 0)
		{
			continue;
		}

		if (skip_loopback && ((ifs[i].ifr_flags & IFF_LOOPBACK) != 0))
		{
			continue;
		}

		if ((ifs[i].ifr_flags & IFF_UP) == 0)
		{
			continue;
		}

		if (dev_name != NULL && strcmp(dev_name, ifs[i].ifr_name) != 0)
		{
			continue;
		}

		if (ioctl(fd, SIOCGIFADDR, &ifs[i]) < 0)
		{
			continue;
		}

		sa = (struct sockaddr_in *) (&ifs[i].ifr_addr);
		addr = sa->sin_addr;

		close(fd);
		return true;
	}

	close(fd);
	return false;
}

bool ipv4_is_local(const struct in_addr &addr /* out */, bool skip_loopback/* = true*/)
{
	int fd, i;
	struct ifreq  ifs[16];
	struct ifconf ifc;
	struct sockaddr_in* sa = NULL;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
	{
		return false;
	}

	ifc.ifc_len = sizeof(ifs);
	ifc.ifc_buf = (caddr_t)ifs;

	if (ioctl(fd, SIOCGIFCONF, &ifc) < 0)
	{
		close(fd);
		return false;
	}

	i = ifc.ifc_len / sizeof(struct ifreq);
	for (; i > 0; --i)
	{
		if (ioctl(fd, SIOCGIFFLAGS, &ifs[i]) < 0)
		{
			continue;
		}

		if (skip_loopback && ((ifs[i].ifr_flags & IFF_LOOPBACK) != 0))
		{
			continue;
		}

		if ((ifs[i].ifr_flags & IFF_UP) == 0)
		{
			continue;
		}

		if (ioctl(fd, SIOCGIFADDR, &ifs[i]) < 0)
		{
			continue;
		}

		sa = (struct sockaddr_in *) (&ifs[i].ifr_addr);
		if (sa->sin_addr.s_addr != addr)
		{
			continue;
		}

		close(fd);
		return true;
	}

	close(fd);
	return false;
}

// 把存储在 uint64_t 里的 IP, PORT 转成字符串
// 其中，高四字节存 PORT，低四字节存 IP
void ipv4_ip_port_to_str(uint64_t ip_port, std::string& str)
{
	char buf[32];
	uint32_t ip   = (uint32_t) (ip_port & 0xffffffff);
	uint16_t port = (uint16_t) ((ip_port >> 32) & 0xffff);
	unsigned char *bytes = (unsigned char *) &ip;

	if (port > 0)
	{
		snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%d", bytes[0], bytes[1], bytes[2], bytes[3], port);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
	}

	str.assign(buf);
}

// 把 IP, PORT 转成 uint64_t
// 其中，高四字节存 PORT，低四字节存 IP
bool ipv4_str_to_ip_port(const char *ip, uint16_t port, uint64_t& ip_port)
{
	struct in_addr addr;

	const char *p = strchr(ip, ':');
	if (p != NULL && p > ip)
	{
		std::string ip_str(ip, p - ip);

		if (!ipv4_str_to_addr(ip_str.c_str(), addr))
		{
			return false;
		}

		port = atoi(p + 1);
	}
	else if (!ipv4_str_to_addr(ip, addr))
	{
		return false;
	}

	ip_port = port;
	ip_port <<= 32;
	ip_port |= addr.s_addr;

	return true;
}

} // namespace triones
