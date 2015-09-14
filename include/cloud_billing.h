/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __CLOUD_BILLING_H__
#define __CLOUD_BILLING_H__


#include "comm_http.h"


typedef struct CLOUD_PackageInfo
{
	
}CLOUD_PackageInfo;


class CLOUD_Billing
{
public:
	CLOUD_Billing(const char* remoteHost, const int httpPort, const char* baseResource);
	virtual ~CLOUD_Billing();

public:
	int reportFlux(const char* userSid, const size_t byteFlux);
	int getBalance(const char* userSid, size_t& currBalance);
	int getPackageInfo(const char* userSid, CLOUD_PackageInfo& packageInfo);
private:
	Http_Trans*		__HttpTrans;
	char 			__RemoteHost[64];
	unsigned short 	__HttpPort;
	char 			__BaseResource[128];

};


#endif /* __CLOUD_BILLING_H__ */

