/******************************************************
 *   FileName: fileserver.h
 *     Author: Triones  2012-10-15
 *Description:
 *******************************************************/

#ifndef FILESERVER_H_
#define FILESERVER_H_

#include "netop.h"
#include "eventpool.h"
#include "socket.h"

class FileServer;

class FileService : public NetService
{
public:
	virtual int on_connect(Socket *socket);
	virtual int on_close(Socket *socket);
	virtual int on_read(Socket *socket);

private:
    FileServer *_file_server;
};

class FileServer
{
public:
	FileServer(){}
	virtual ~FileServer(){}
	bool start(const char *ip, unsigned short port, NetService *appop);
	bool stop();

private:
	eventpool _event_pool;
};


#endif /* FILESERVER_H_ */
