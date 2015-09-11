/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/


#include "cloud_billing.h"
#include "comm_json.h"


#define PARA_CLOUD_FLUX_SET		"setCldFlux"
#define PARA_BLANCE_GET			"getBlance"
#define PARA_SID				"sid"


CLOUD_Billing::CLOUD_Billing(const char* remoteHost, const int httpPort, const char* baseResource)

{
	__HttpTrans = NULL;
	
	if (remoteHost)
		(void)snprintf(__RemoteHost, sizeof(__RemoteHost), "%s", remoteHost);

	if (baseResource)
		(void)snprintf(__BaseResource, sizeof(__RemoteHost), "%s", baseResource);
	
	__HttpPort = httpPort;
}

CLOUD_Billing::~CLOUD_Billing()
{
	try
	{
		if (__HttpTrans)
		{
			delete __HttpTrans;
			__HttpTrans = NULL;
		}
		memset(__RemoteHost, 0, sizeof(__RemoteHost));
		memset(__BaseResource, 0, sizeof(__BaseResource));
		__HttpPort = 0;
	}
	catch (...)
	{
	}
}

int CLOUD_Billing::reportFlux(const char* userSid, const size_t byteFlux)
{
	CHECK_RET(userSid != NULL, RET_ERROR);

	Http_Trans httpTrans(NULL);

	char 		resource[1024] = {0};
	char 		header_line[1024] = {0};
	size_t 		line_len = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "%s?" PARA_SID "=%s&" PARA_CLOUD_FLUX_SET "=%zu", 
					__BaseResource, userSid, byteFlux);
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_GET, resource);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __RemoteHost);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	int ret = 0;
	ret = httpTrans.sendHttp(__RemoteHost, __HttpPort);
	if (ret != RET_OK)
	{
		TRACE("sent http header failed: %d", ret);
		return RET_ERROR;
	}
	
	/* Receive Response */
	ret = httpTrans.recvHeader();
	if (ret != RET_OK)
	{
		TRACE("receive http header failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.getResult();
	if (ret == RET_ERROR)
	{
		TRACE("parse http header result failed: %d", ret);
		return RET_ERROR;
	}	
	if(ret != 200)
	{
		TRACE("parse http response result: %d", ret);
		return RET_ERROR;
	}
	
	/* Receive & Parse Body */
	COMM_Buffer	jsonBuff;
	ret = httpTrans.saveBody(&jsonBuff);
	CHECK_RET(ret == RET_OK, RET_ERROR);
	
	DEBUGMSG("Body:\n%s\n", jsonBuff.dataStart);

	cJSON* json = cJSON_Parse(jsonBuff.dataStart);
	if (json)
	{
		TRACE("json type = %d", json->type);
		cJSON* root = cJSON_GetObjectItem(json,"root");
		if (root)
		{
			TRACE("root type = %d", root->type);
			cJSON* data = cJSON_GetObjectItem(root,"data");
			if (data)
			{
				TRACE("data type = %d", data->type);
				int array_size = cJSON_GetArraySize(data);
				TRACE("data array_size = %d", array_size);

				cJSON* item =NULL;
				cJSON* deviceGuid = NULL;
				for (int i = 0; i < array_size; ++i)
				{
					item = cJSON_GetArrayItem(data,i);
					if (item)
					{
						deviceGuid = cJSON_GetObjectItem(item,"deviceGuid");
						if (deviceGuid)
						{
							TRACE("deviceGuid[%d] = %s", i, deviceGuid->valuestring);
						}
					}
				}
			}
		}
		cJSON_Delete(json);
	}
	else
	{
		TRACE("parse JSON failed!");
	}

	return RET_OK;

}

