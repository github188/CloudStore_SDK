/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __OBSS_CLIENT_H__
#define __OBSS_CLIENT_H__


#define DEFAULT_OSS_HOST	"oss.aliyuncs.com"
#define DEFAULT_JOS_HOST	"pan.cloudsee.com"
#define DEFAULT_OBSS_PORT	80
#define DEFAULT_AGANT		"OBSS-Client"


typedef enum
{
	OBSS_JOVISION = 0,	// Jovision Obss
	OBSS_ALI = 1,		// Ali Yun Oss
	OBSS_UNKNOWN = 2,	// unknown type
} Obss_Type;


class OBSS_Client
{
public:
	OBSS_Client();
	virtual ~OBSS_Client();
	
public:
	/**
	* Initialize client info
	* @param [in]  remoteHost	is the hostname of obss server.
	* @param [in]  httpPort 	is the http port of obss server, default is 80.
	* @param [in]  userAgent 	is the client name.
	* @param [in]  accessId		is the obss access id.
	* @param [in]  accessKey	is the obss access key.
	* @param [in]  obssType		it the OBSS type.
	* @return	 0		success
				-1		failed	**/
	int initial(Obss_Type obssType, const char* accessId, const char* accessKey,
				const char* remoteHost, const int httpPort = DEFAULT_OBSS_PORT, const char* userAgent = DEFAULT_AGANT);

	
public:
	char 			RemoteHost[128];
	char 			UserAgent[64];
	char 			AccessId[64];
	char 			AccessKey[64];
	unsigned short 	HttpPort;
	Obss_Type		ObssType;
};


#endif /* __OBSS_CLIENT_H__ */

