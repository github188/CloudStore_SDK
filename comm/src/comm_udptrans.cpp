/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/


#include "comm_net.h"
#include "comm_udptrans.h"


#define UDP_TIMEOUT		3

Udp_Trans::Udp_Trans(): __Socket(-1),  __RemoteIp(NULL),  __RemotePort(80)
{

}

Udp_Trans::~Udp_Trans()
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

int32 Udp_Trans::config(const char* remoteIp, const int remotePort)
{
	CHECK_RET(remoteIp != NULL, RET_ERROR);
	
	if (__RemoteIp)
		strRelease(__RemoteIp);
	
	__RemoteIp = strDup(remoteIp);
	__RemotePort= remotePort;

	return RET_OK;
}

int32 Udp_Trans::connect()
{
	int32 socket = 0;
	
	socket = udp_Connect(__RemoteIp, __RemotePort);
	CHECK_RET(socket != -1, RET_ERROR);

	__Socket = socket;
	return RET_OK;
}

int32 Udp_Trans::send(const char* data, size_t len)
{
	CHECK_RET(__Socket > 0, RET_ERROR);
	return udp_Send(__Socket, data, len, UDP_TIMEOUT);
}

int32 Udp_Trans::recv(char* data, size_t len)
{
	CHECK_RET(__Socket > 0, RET_ERROR);
	return udp_Recv(__Socket, data, len, UDP_TIMEOUT);
}

void Udp_Trans::close()
{
	if (__Socket > 0)
	{
		udp_Close(__Socket);
		__Socket = -1;
	}
	return;
}

bool Udp_Trans::checkSocket()
{
	return (__Socket == -1) ? false : true;
}

