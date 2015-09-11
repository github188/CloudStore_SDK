/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __CLOUD_CONTROL_H__
#define __CLOUD_CONTROL_H__


#include "comm_http.h"


class CLOUD_Controler
{
public:
	CLOUD_Controler(const char* remoteHost, const int httpPort, const char* baseResource);
	virtual ~CLOUD_Controler();

public:
	int setCloudSwitch(const char* userSid, const int cloudSw);
	int getCloudSwitch(const char* userSid, int& cloudSw);

private:
	Http_Trans*		__HttpTrans;
	char 			__RemoteHost[64];
	unsigned short 	__HttpPort;
	char 			__BaseResource[128];

};


#endif /* __CLOUD_CONTROL_H__ */

