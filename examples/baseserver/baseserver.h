/******************************************************
*FileName: testserver.cpp
*Author: triones  2014-7-29
*Description:
*******************************************************/
#include <net/tcptransport.h>
#include <net/udptransport.h>

class BaseServer
{
	public:
		BaseServer();

		virtual ~BaseServer();

		bool tcp_svr_init(IEventReactor *reactor,TransProtocol* protocol);

		bool udp_svr_init(IEventReactor *reactor,TransProtocol* protocol);

		bool tcp_svr_start(const char *ip, unsigned short port, int thread_num);

		bool udp_svr_start(const char *ip, unsigned short port, int thread_num);

		bool tcp_svr_stop();

		bool udp_svr_stop();

		int  tcp_send(Socket* sock,const char* data_buf,int data_len);

		int  udp_send(Socket* sock,const char* data_buf,int data_len);

	private:

		TcpTransPort* _tcp;
		UdpTransPort* _udp;
};



