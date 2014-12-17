/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */
#ifndef TBNET_ISERVERADAPTER_H
#define TBNET_ISERVERADAPTER_H

namespace triones
{

class IServerAdapter
{
	friend class TCPComponent;
	friend class UDPComponent;

public:
    enum {
    	CMD_DATA_PACKET = 0,
        CMD_BAD_PACKET,
        CMD_TIMEOUT_PACKET,
        CMD_DISCONN_PACKET
    };

public:
	// 单个packet回调， 直接从整个网络层，直接回调上去的
	virtual bool syn_handle_packet(IOComponent *connection, Packet *packet) = 0;

	// 批量packet回调, 可以不用实现
	virtual bool handle_batch_packet(IOComponent *connection, PacketQueue &packetQueue)
	{
		UNUSED(packetQueue);
		UNUSED(connection);
		return false;
	}

	// 处理连接结果
	virtual bool handle_connected(IOComponent *connection, bool succ)
	{
		printf("---call handle_connected -----\n");
		UNUSED(connection);
		if (succ) {
			printf("(^_^) (^_^) (^_^) (^_^) (^_^)　connect succ!\n");
		} else {
			printf("(>_<) (>_<) (>_<)　(>_<)　(>_<)　connect failed!\n");
		}
		return false;
	}

	// 构造函数
	IServerAdapter()
	{
		_batch_push_packet = false;
	}

	virtual ~IServerAdapter()
	{

	}

	void set_batch_push_packet(bool value)
	{
		_batch_push_packet = value;
	}

private:
	bool _batch_push_packet;          // 批量post packet
};
}

#endif /*ISERVERADAPTER_H*/
