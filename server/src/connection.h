#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#pragma once

#include "packet_manager.h"

class Connection
{
public:
	Connection(int id, SOCKET socket);
	void Kill();

public:
	int m_id;
	SOCKET m_socket;
	CPacketManager m_packetManager;
	bool m_isActive;
	HANDLE m_hThread;
};

#endif