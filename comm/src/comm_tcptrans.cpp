/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/


#include "obss_net.h"
#include "obss_tcptrans.h"


#define TCP_TIMEOUT		3

Tcp_Trans::Tcp_Trans(): __Socket(-1),  __RemoteIp(NULL),  __RemotePort(80)
{

}

Tcp_Trans::~Tcp_Trans()
{
	try
	{	
		if(__RemoteIp)
		{
		strRelease(__RemoteIp);
		__RemoteIp = NULL;
		}
		
		close();
		__RemotePort = 0;
	}
	catch (...)
	{
	}

}

int32 Tcp_Trans::config(const char* hostName, uint16 port)
{
	CHECK_RET(hostName != NULL, RET_ERROR);

	char remote_ip[20] = {0};
	int ret = tcp_GetHostIp(hostName, remote_ip, sizeof(remote_ip));
	CHECK_RET(ret == 0, RET_ERROR);
	
	if (__RemoteIp)
		strRelease(__RemoteIp);
	
	__RemoteIp = strDup(remote_ip);
	__RemotePort= port;

	return RET_OK;
}

int32 Tcp_Trans::connect()
{
	int32 socket = 0;
	
	socket = tcp_Connect(__RemoteIp, __RemotePort, TCP_TIMEOUT);
	CHECK_RET(socket > 0, RET_ERROR);

	__Socket = socket;
	return RET_OK;
}

int32 Tcp_Trans::send(const char* data, size_t len)
{
	CHECK_RET(__Socket > 0, RET_ERROR);
	return tcp_Send(__Socket, data, len, TCP_TIMEOUT);
}

int32 Tcp_Trans::recv(char* data, size_t len)
{
	CHECK_RET(__Socket > 0, RET_ERROR);
	return tcp_Recv(__Socket, data, len, TCP_TIMEOUT);
}

void Tcp_Trans::close()
{
	if (__Socket > 0)
	{
		tcp_Close(__Socket);
		__Socket = -1;
	}
	return;
}


