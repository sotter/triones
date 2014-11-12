/******************************************************
 *   FileName: ServerSocket.h
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

namespace triones
{

class ServerSocket: public Socket
{
public:

	ServerSocket();

	virtual ~ServerSocket();

	Socket *accept();

	bool listen();

private:

	int _back_log;
};

} /* namespace triones */

#endif /* SERVERSOCKET_H_ */
