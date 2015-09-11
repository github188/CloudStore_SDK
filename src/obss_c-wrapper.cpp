/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#include "obss_c-wrapper.h"
#include "obss_client.h"
#include "obss_operation.h"


class OBSS_IPC: public OBSS_Operation
{
public:
	OBSS_IPC(OBSS_Client* client, const char* bucket, const char* ystNum):OBSS_Operation(client)
	{
		(void)snprintf(__bucket, sizeof(__bucket), "%s", bucket);
		(void)snprintf(__ystNum, sizeof(__ystNum), "%s", ystNum);
	}
	virtual ~OBSS_IPC()
	{
	}
	
public:
	int IPC_putObjFromFile(const int year, const int month, const int day, const char* name, 
								const char *localFile, size_t* billingSize)
	{
		CHECK_RET(name != NULL, RET_ERROR);
		CHECK_RET(billingSize != NULL, RET_ERROR);

		char object[1024] = {0};
		(void)snprintf(object, sizeof(object), "%s/%d/%d/%d/%s", __ystNum, year, month, day, name);
		return putObjFromFile(__bucket, object, localFile, billingSize);
	}
	
	int IPC_putObjFromStream_Init(const int year, const int month, const int day, const char* name, 
								const size_t streamLen)
	{
		CHECK_RET(name != NULL, RET_ERROR);

		char object[1024] = {0};
		(void)snprintf(object, sizeof(object), "%s/%d/%d/%d/%s", __ystNum, year, month, day, name);
		return putObjFromStream_Init(__bucket, object, streamLen);
	}

	int IPC_putObjFromStream_Send(const char* subStream, const size_t subStreamLen)
	{
		return putObjFromStream_Send(subStream, subStreamLen);
	}
		
	int IPC_putObjFromStream_Finish()
	{
		return putObjFromStream_Finish();
	}
	
	int IPC_deleteDir(const int year, const int month, const int day)
	{
		char prefix[1024] = {0};
		(void)snprintf(prefix, sizeof(prefix), "%s/%d/%d/%d/", __ystNum, year, month, day);
		return deleteDir(__bucket, prefix);
	}

	int IPC_resetBucket(const char* bucket)
	{
		CHECK_RET(bucket != NULL, RET_ERROR);
		
		memset(__bucket, 0 , sizeof(__bucket));
		(void)snprintf(__bucket, sizeof(__bucket), "%s", bucket);

		return RET_OK;
	}
	
private:
	char	__bucket[64];
	char	__ystNum[32];
};


OBSS_ClntHandle OBSS_InitCloudClnt(int cloudType, const char* accessId, const char* accessKey, const char* cloudHost)
{
	int ret = -1;
	
	OBSS_Client* cloudClient = new OBSS_Client();
	ret = cloudClient->initial((Obss_Type)cloudType, accessId, accessKey, cloudHost, DEFAULT_OBSS_PORT, DEFAULT_AGANT);
	if (RET_OK != ret)
	{
		delete cloudClient;
		return NULL;
	}
	return (OBSS_ClntHandle)cloudClient;
}

void OBSS_FinalCloudClnt(OBSS_ClntHandle* pCloudClient)
{
	if (*pCloudClient)
	{
		OBSS_Client* obssClient = (OBSS_Client*)(*pCloudClient);
		delete obssClient;
		*pCloudClient = NULL;
	}
	return;
}

OBSS_OptHandle OBSS_InitCloudOpt(OBSS_ClntHandle cloudClient, const char* bucket, const char* devSN)
{
	if (cloudClient && bucket && devSN)
	{
		OBSS_Client* obssClient = (OBSS_Client*)cloudClient;
		OBSS_IPC* obssOpt = new OBSS_IPC(obssClient, bucket, devSN);
		return (OBSS_OptHandle)obssOpt;
	}
	return NULL;
}

void OBSS_FinalCloudOpt(OBSS_OptHandle* pCloudOpt)
{
	if (*pCloudOpt)
	{
		OBSS_IPC* obssOpt = (OBSS_IPC*)(*pCloudOpt);
		delete obssOpt;
		*pCloudOpt = NULL;
	}
	return;
}

int OBSS_UploadObjFromFile(OBSS_OptHandle cloudOpt, 
								const int year, const int month, const int day, const char* name, 
								const char *localFile, size_t* billingSize)
{
	CHECK_RET(cloudOpt != NULL, RET_ERROR);
	
	OBSS_IPC* obssOpt = (OBSS_IPC*)cloudOpt;
	return obssOpt->IPC_putObjFromFile(year, month, day, name, localFile, billingSize);
}

int OBSS_UploadObjFromStream_Init(OBSS_OptHandle cloudOpt,	
										const int year, const int month, const int day, const char* name, const size_t streamLen)
{
	CHECK_RET(cloudOpt != NULL, RET_ERROR);

	OBSS_IPC* obssOpt = (OBSS_IPC*)cloudOpt;
	return obssOpt->IPC_putObjFromStream_Init(year, month, day, name, streamLen);
}

int OBSS_UploadObjFromStream_Send(OBSS_OptHandle cloudOpt,	
										const char* subStream, const size_t subStreamLen)
{
	CHECK_RET(cloudOpt != NULL, RET_ERROR);

	OBSS_IPC* obssOpt = (OBSS_IPC*)cloudOpt;
	return obssOpt->IPC_putObjFromStream_Send(subStream, subStreamLen);
}

int OBSS_UploadObjFromStream_Finish(OBSS_OptHandle cloudOpt)
{
	CHECK_RET(cloudOpt != NULL, RET_ERROR);

	OBSS_IPC* obssOpt = (OBSS_IPC*)cloudOpt;
	return obssOpt->IPC_putObjFromStream_Finish();
}

int OBSS_DeleteDirByDate(OBSS_OptHandle cloudOpt, const int year, const int month, const int day)
{
	CHECK_RET(cloudOpt != NULL, RET_ERROR);

	OBSS_IPC* obssOpt = (OBSS_IPC*)cloudOpt;
	return obssOpt->IPC_deleteDir(year, month, day);
}

int OBSS_ResetBucket(OBSS_OptHandle cloudOpt, const char* bucket)
{
	CHECK_RET(cloudOpt != NULL, RET_ERROR);

	OBSS_IPC* obssOpt = (OBSS_IPC*)cloudOpt;
	return obssOpt->IPC_resetBucket(bucket);
}

