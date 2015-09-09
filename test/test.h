/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#include "obss_client.h"
#include "obss_operation.h"
#include "obss_c_style.h"


#if 1
#define ALI_TEST_HOST_NAME		"oss-cn-beijing.aliyuncs.com"
#define ALI_TEST_ACCESS_ID		"4fZazqCFmQTbbmcw"
#define ALI_TEST_ACCESS_KEY 	"sBT6DgBMdCHnnaYdvK0o6O3zaN06sW"
#define ALI_TEST_BUCKET 		"jov-test"

#define JOV_TEST_HOST_NAME		"pan.cloudsee.net"
#define JOV_TEST_ACCESS_ID		"KD3CWKRN96SUYMW9U1HI"
#define JOV_TEST_ACCESS_KEY 	"YaIVDaJPLjTF8Ow88z1SQK3i6GT1wK0LN1Cx6IBH"
#define JOV_TEST_BUCKET 		"jov_test"
#else
#define ALI_TEST_HOST_NAME		"oss-cn-hangzhou.aliyuncs.com"
#define ALI_TEST_ACCESS_ID		"4fZazqCFmQTbbmcw"
#define ALI_TEST_ACCESS_KEY 	"sBT6DgBMdCHnnaYdvK0o6O3zaN06sW"
#define ALI_TEST_BUCKET 		"jovetech"

#define JOV_TEST_HOST_NAME		"pan.cloudsee.net"
#define JOV_TEST_ACCESS_ID		"G3ZIKD1O99AYJ3YLNFKP"
#define JOV_TEST_ACCESS_KEY 	"elXOR16F9gvftVo7UQpFMg9Z6hwrFYV42GGA8YxD"
#define JOV_TEST_BUCKET 		"S288907438"
#endif


int mapStdout2LogFile(const char* fileName)
{
	CHECK_RET(fileName, RET_ERROR);
	
	FILE* LogFile = NULL;
	LogFile = freopen(fileName, "a+", stdout);
	if (!LogFile)
	{
		return RET_ERROR;
	}
	return RET_OK;
}

int mapStdoutBack()
{
	FILE* stream = NULL;
	stream = freopen("/dev/tty", "w", stdout);
	if (!stream)
	{
		return RET_ERROR;
	}
	return RET_OK;
}


/***************************************
* Example for using OBSS SDK				   		*
****************************************/
class object_info
{
public:
	char 	contentType[64];
	int 	contentLength;
	char 	ossObjectType[64];
	int 	ossNextAppendPos;
};

class OBSS_Test: public OBSS_Operation
{
public:
	OBSS_Test(OBSS_Client* client):OBSS_Operation(client) 
	{
		memset(&objInfo, 0, sizeof(object_info));
	}
	~OBSS_Test() {}
	
public:
	int CallBack_parseHeadObjInfo(Http_PDU* recvHead)
	{
		const char* header = NULL;
		
		header = recvHead->getHeader("Content-Type");
		if (header)
			(void)snprintf(objInfo.contentType, sizeof(objInfo.contentType), "%s", header);

		header = recvHead->getHeader("Content-Length");
		if (header)
			objInfo.contentLength = atoi(header);

		header = recvHead->getHeader("x-oss-object-type");
		if (header)
			(void)snprintf(objInfo.ossObjectType, sizeof(objInfo.ossObjectType), "%s", header);

		header = recvHead->getHeader("x-oss-next-append-position");
		if (header)
			objInfo.ossNextAppendPos = atoi(header);

		return RET_OK;
	}
	
	static void* downloadFile(void* param)
	{
		OBSS_Test* OptTest = (OBSS_Test*)param;
		int ret = 0;
		
		ret = OptTest->getObj2File(OptTest->bucketName, OptTest->objectName, OptTest->fileName);
		if (0 == ret)
			pthread_exit((void*)"SUCCESS");
		else
			pthread_exit((void*)"FAILED");
		//return NULL;
	}

public:
	object_info objInfo;
	char bucketName[64];
	char objectName[1024];
	char fileName[1024];
};


