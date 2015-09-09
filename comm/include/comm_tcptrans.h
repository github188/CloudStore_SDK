/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __OBSS_TRANSE_H__
#define __OBSS_TRANSE_H__


#include "obss_utils.h"


class Tcp_Trans
{
public:
	Tcp_Trans();
	virtual ~Tcp_Trans();
public:
	int32 config(const char* hostName, uint16 port);
	int32 connect();
	int32 send(const char* data, size_t len);
	int32 recv(char* data, size_t len);
	void close();

private:
	int32	__Socket;
	char*	__RemoteIp;
	uint16	__RemotePort;
};

#endif /* __OBSS_TRANSE_H__ */

