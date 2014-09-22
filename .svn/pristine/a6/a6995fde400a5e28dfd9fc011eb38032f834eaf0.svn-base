/******************************************************
 *   FileName: fileclient.cpp
 *     Author: Triones  2012-9-17 
 *Description:
 *******************************************************/
#include "fileclient.h"
#include <errno.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "header.h"
#include "log.h"
#include "sock_buffer.h"
#include "../header.h"
#include "tcp_socket.h"
#include "udp_socket.h"

static bool WriteFile( const char *szName, const char *szBuffer, const int nLen );

static char *ReadFile( const char *szFile , int &nLen );

int FileClient::on_connect(Socket *socket)
{
    return FSM_END;
}

int FileClient::on_close(Socket *socket)
{
    _user._state = OFF_LINE;
    _user._socket = NULL;

    return FSM_END;
}

int FileClient::on_read(Socket *socket)
{
	FifoBuffer::Packet packet;

	while (socket->get_one_packet(&packet))
	{
		char *data = (char*) packet.data;

		bigheader *header = (bigheader*) data;
		int cmd = ntohs(header->cmd);

		//printf("cmd = %d \n", cmd);
		if (cmd == BIG_OPEN_RSP)
		{
			bigloginrspall *resp = (bigloginrspall *)data;
			dlog1("recv login resp : %d \n", resp->resp.result);

            socket->ptr = &_user;
            _user._socket = socket;
			_user._state  = ON_LINE;
		}
		else if(cmd == BIG_WRITE_RSP)
		{
			bigwriterspall *resp = (bigwriterspall *)data;
            dlog1("recv write resp: %d \n", resp->resp.result);
		}
		else if(cmd == BIG_CLOSE_RSP)
		{

		}
		else
		{
			printf("cmd = %d , can not parse \n", cmd);
		}
	}

	return FSM_END;
}

bool FileClient::start(const char *ip, unsigned short port)
{
    _event_pool.start();

    UdpSocket *sock = new UdpSocket;
    if(! sock->connect(ip, port))
    {
    	return false;
    }

    sock->set_non_block(true);
    sock->set_net_service(this);
    sock->set_recv_buffer_size(100 * 1024);
    sock->set_split(&__ctfo_split);
    _event_pool.event_add(sock);

    bigloginreqall login("root", "root", 0);

    if( sock->send((const void*)&login, (int)sizeof(login)) < 0)
    {
    	printf("write error : %s \n", strerror(errno));
        return false;
    }

    return true;

    //连接, 然后赋值给User;
}

int FileClient::put_file(const char *local_file, const char *remote_file)
{
	int len = 0;
	string name =  local_file;
	char *buffer = ReadFile(name.c_str(), len);
	if (buffer == NULL || len <= 0)
	{
        dlog1("ReadFile fail");
		return FILE_FAIL;
	}

	int ret = writefile(buffer, len, remote_file);
	free(buffer);
	return ret;
}

//将数据写入到远程文件
int FileClient::writefile(const char *data, int data_len, const char *remote_file)
{
    while(_user._state != ON_LINE)
    {
    	usleep(1000);
    }

	if (data == NULL || data_len <= 0 || remote_file == NULL)
		return FILE_PARAM_ERROR;

	bigwritereqall writereq(remote_file, data_len);

	int msg_len = data_len + sizeof(bigwritereqall);
	char *send_data = (char *) malloc(msg_len);
	if (data == NULL)
	{
		free(send_data);
		printf("malloc fail \n");
		return FILE_FAIL;
	}

	memcpy(send_data, (char *) &writereq, sizeof(bigwritereqall));
	memcpy(send_data + sizeof(bigwritereqall), data, data_len);

    _user._socket->send(send_data, msg_len);

    free(send_data);

    return FILE_SUCCESS;
}

bool WriteFile( const char *szName, const char *szBuffer, const int nLen )
{
#ifdef _WIN32
	FILE *fp = fopen( szName, "wb" ) ;
	if ( fp == NULL )
	{
		return false;
	}
	fwrite( szBuffer, nLen, 1, fp ) ;
	fclose( fp ) ;
#else
	int fp = open( szName, O_CREAT | O_TRUNC | O_WRONLY ) ;
	if ( fp < 0 )
	{
		return false;
	}
	write( fp, szBuffer, nLen ) ;
	close( fp ) ;
	chmod( szName, 0777 ) ;
#endif

	return true;
}

// 读取文件
char *ReadFile( const char *szFile , int &nLen )
{
	char *szBuffer = NULL ;
#ifdef _WIN32

	FILE* fp = fopen( szFile , "rb" ) ;
	if ( fp != NULL )
	{
		int len = 0 ;
		fseek( fp , 0 , SEEK_END ) ;
		len = ftell( fp ) ;

		szBuffer = new char[len+1];

		fseek( fp , 0 , SEEK_SET ) ;
		fread( szBuffer , 1 , len , fp ) ;
		szBuffer[len] = 0 ;

		nLen  = len ;

		fclose( fp );
	}
	else
	{
		nLen  = 0 ;
		return NULL ;
	}

#else

	int fp = open( szFile , O_RDONLY ) ;

	if ( fp >= 0 )
	{
		// 先得到文件的大小
		struct stat buf ;
		fstat( fp , &buf ) ;

		int len = buf.st_size ;

		szBuffer = new char[len+1];

		read( fp , szBuffer , len ) ;
		szBuffer[len] = 0 ;

		nLen  = len ;

		close( fp );
	}
	else
	{
		nLen =  0 ;
		return NULL ;
	}

#endif

	return szBuffer;
}

