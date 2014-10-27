/******************************************************
 *   FileName: UDPComponent.cpp
 *     Author: triones  2014-9-18
 *Description:
 *******************************************************/

#include "cnet.h"
#include "../pack/tprotocol.h"
#include "../comm/comlog.h"
#include "tbtimeutil.h"
#include "stats.h"

namespace triones
{

UDPComponent::UDPComponent(Transport *owner, Socket *socket, TransProtocol *streamer,
        IServerAdapter *serverAdapter, int type)
		: IOComponent(owner, socket, type)
{
	_streamer = streamer;
	_serverAdapter = serverAdapter;
}

UDPComponent::~UDPComponent()
{

}

bool UDPComponent::init(bool isServer)
{
	if (!isServer)
	{
		if (!_socket->connect())
		{
			return false;
		}
	}

	_isServer = isServer;
	return true;
}

void UDPComponent::close()
{

}

//UDPComponent不处理可写事件，UDPComponent的写操作是同步接口
bool UDPComponent::handleWriteEvent()
{
	return true;
}

bool UDPComponent::handleReadEvent()
{
	__INTO_FUN__
	_lastUseTime = triones::CTimeUtil::getTime();
	bool rc = false;
	if (_state == TRIONES_CONNECTED)
	{
		rc = readData();
	}
	return rc;
}

//提供给已经连接的UDPSocket使用；
bool UDPComponent::readData()
{
	__INTO_FUN__

	//只有TRIONES_UDPCONN才能调用这个接口，
	//TRIONES_UDPACCETOR调用的是readFrom
	//TRIONES_UDPACTCONN的read是由TRIONES_UDPACCETOR
	if(_type != TRIONES_UDPCONN)
		return false;

	//每个包设置为8K的大小，第一次读取出来最大是8K
	_input.ensureFree(READ_WRITE_SIZE);

	//ret表示已经读取到的数据
	int ret = _socket->read(_input.getFree(), _input.getFreeLen());

	int read_cnt = 0;
	bool broken = false;

	//最大能连续读取10次，读任务便切换出来，给其他socket使用
	while (ret > 0 && ++read_cnt <= 10)
	{
		_input.pourData(ret);
		int decode = _streamer->decode(_input.getData(), _input.getDataLen(), &_inputQueue);

		//一定要调用drainData，因为decode时候没有将_input的读位置前移。
		if(decode > 0)
		{
			_input.drainData(decode);
		}

		//如果发生了断开事件，或是_input没有读满（说明缓冲区里面已经没有数据了）
		if (broken || _input.getFreeLen() > 0) break;

		//如果判定读出来的包还没有解析完全说明，可能有未读出来的半包。
		//原先的判断条件为decode > 0, 修改为decode >= 0, decode == 0时可能是一个大包
		//数据包还没有完全接收完毕
		if (decode >= 0 && decode < _input.getDataLen())
		{
			_input.ensureFree(READ_WRITE_SIZE);
		}

		//todo: 如果发送是一个大包，要在encode体现出来，告知上层，那么_input继续扩大自己的范围来适应大包的发送。
		ret = _socket->read(_input.getFree(), _input.getFreeLen());
	}

	//对读到的数据业务回调处理，注意这个地方并不负责packet的释放，而是由外部来释放的
	if (_inputQueue._size > 0)
	{
		if (_serverAdapter->_batchPushPacket)
		{
			_serverAdapter->handleBatchPacket(this, _inputQueue);
		}
		else
		{
			Packet *pack = NULL;
			while ((pack = _inputQueue.pop()) != NULL)
			{
				_serverAdapter->SynHandlePacket(this, pack);
			}
		}
	}

	//将读缓存区回归到初始位置
	_input.shrink();

	/*************
	 * broken事件在最后处理，且是在事件在外层调用的，所以当读取几个包后，读到断开事件时并不影响已经读到的数据的处理。但需要一个前提条件：
	 * （1）断开事件不能采用直接回调的方式，而是跟其它packet数据一样进行处理排队，这个地方要对packet进行处理
	 *************/
	if (!broken)
	{
		if (ret == 0)
		{
			OUT_INFO(NULL, 0, NULL, "%s recv 0, disconnect", _socket->getAddr().c_str());
			broken = true;
		}
		else if (ret < 0)
		{
			int error = Socket::getLastError();
			broken = (error != EAGAIN);
		}
	}

	return !broken;
}

//提供给已经连接的UDPSocket使用；
bool UDPComponent::writeData()
{
	__INTO_FUN__
	// 把 _outputQueue copy到 _myQueue中, 从_myQueue中向外发送
	_output_mutex.lock();
	_outputQueue.moveto(&_myQueue);

	//如果socket中的数据已经全部发送完毕了，置可写事件为false，然后退出来
	if (_myQueue.size() == 0 && _output.getDataLen() == 0)
	{
		this->enableWrite(false);
		_output_mutex.unlock();
		return true;
	}
	_output_mutex.unlock();

	Packet *packet;
	int ret;
	int writeCnt = 0;
	int myQueueSize = _myQueue.size();

	//todo:这块代码需要重写，packet本身就是从_output继承过来的
	//如果write出现ERRORAGAIN的情况，下一个包继续发送；
	do
	{
		while (_output.getDataLen() < READ_WRITE_SIZE)
		{
			// 队列空了就退出
			if (myQueueSize == 0) break;
			packet = _myQueue.pop();

			//为什么将packet放入到_output中发送，如果发送有失败的情况，可以将未发送的数据放入到out_put中；
			//而且是易扩展，如果packet不是继承DataBuffer,可以将packet序列成数据流。
			//缺点是增加了一次内存拷贝。
			_output.writeBytes(packet->getData(), packet->getDataLen());
			myQueueSize--;
			delete packet;
			TBNET_COUNT_PACKET_WRITE(1);
		}

		if (_output.getDataLen() == 0)
		{
			break;
		}

		if(_type == TRIONES_UDPCONN)
		{
			// write data
			ret = _socket->sendto(_output.getData(), _output.getDataLen(), _sock_addr);
		}
		else if(_type == TRIONES_UDPACTCONN)
		{
			ret = _socket->write(_output.getData(), _output.getDataLen());
		}

		if (ret > 0)
		{
			_output.drainData(ret);
		}
		writeCnt++;
		/*******
		 * _output.getDataLen() == 0 说明发送的数据都结束了
		 * 停止发送的条件：
		 * (1)发送的结果ret <= 0, 发送失败，或者写缓冲区已经满了。
		 * （2） _output.getDataLen() > 0 说明一次没有发送完，还有没有发送完的数据。就直接退出来停止发送了。
		 * 	 那么这块数据去了哪里？
		 * (3)最终myqueue和output中未发送完的数据都到哪里去了
		 **********/
	} while (ret > 0 && _output.getDataLen() == 0 && myQueueSize > 0 && writeCnt < 10);

	// 紧缩
	_output.shrink();

	_output_mutex.lock();
	int queueSize = _outputQueue.size() + _myQueue.size() + (_output.getDataLen() > 0 ? 1 : 0);
	if (queueSize == 0 || _writeFinishClose)
	{
		this->enableWrite(false);
	}
	_output_mutex.unlock();

//	if (_writeFinishClose)
//	{
//		OUT_ERROR(NULL, 0, NULL, "主动断开.");
//		return false;
//	}

	return true;
}


} /* namespace triones */
