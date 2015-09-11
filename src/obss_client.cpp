/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#include "comm_utils.h"
#include "obss_client.h"


OBSS_Client::OBSS_Client()
{
	memset(RemoteHost, 0, sizeof(RemoteHost));
	memset(UserAgent, 0, sizeof(UserAgent));
	memset(AccessId, 0, sizeof(AccessId));
	memset(AccessKey, 0, sizeof(AccessKey));
	HttpPort = 0;
	ObssType = OBSS_UNKNOWN;
}

OBSS_Client::~OBSS_Client()
{
}

int OBSS_Client::initial(Obss_Type obssType, const char* accessId, const char* accessKey,
						const char* remoteHost, const int httpPort, const char* userAgent)
{
	CHECK_RET(remoteHost != NULL, RET_ERROR);
	CHECK_RET(userAgent != NULL, RET_ERROR);
	CHECK_RET(accessId != NULL, RET_ERROR);
	CHECK_RET(accessKey != NULL, RET_ERROR);
	CHECK_RET(httpPort > 0, RET_ERROR);
	CHECK_RET(obssType < OBSS_UNKNOWN, RET_ERROR);

	(void)snprintf(RemoteHost, sizeof(RemoteHost), "%s", remoteHost);
	(void)snprintf(UserAgent, sizeof(UserAgent), "%s", userAgent);
	(void)snprintf(AccessId, sizeof(AccessId), "%s", accessId);
	(void)snprintf(AccessKey, sizeof(AccessKey), "%s", accessKey);
	HttpPort = httpPort;
	ObssType = obssType;

	return RET_OK;
}

