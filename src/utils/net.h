/**
 * author: Triones
 * date  : 2014-08-28
 */
#ifndef __TRIONES_NET_H__
#define __TRIONES_NET_H__

#include <arpa/inet.h>

namespace triones
{

bool ipv4_str_to_addr(const char *ip, struct in_addr &addr);
bool ipv4_addr_to_str(const struct in_addr &addr, std::string &ip_str);
bool ipv4_get_addr(const char *dev_name, struct in_addr &addr, bool skip_loopback = true);
bool ipv4_is_local(const struct in_addr &addr, bool skip_loopback = true);
void ipv4_ip_port_to_str(uint64_t ip_port, std::string& str);
bool ipv4_str_to_ip_port(const char *ip, uint16_t port, uint64_t& ip_port);

} // namespace triones

#endif // #ifndef __TRIONES_NET_H__

