/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/


#include <time.h>

#include "obss_time.h"
#include "obss_xml.h"
#include "obss_error.h"
#include "obss_auth.h"
#include "obss_client.h"
#include "obss_operation.h"


OBSS_ListObjRslt::OBSS_ListObjRslt()
{
	BucketName = NULL;
	Prefix = NULL;
	Marker = NULL;
	Delimiter = NULL;
	NextMarker = NULL;
	IsTruncated = false;
	MaxKeys = 0;
}

OBSS_ListObjRslt::~OBSS_ListObjRslt()
{
	try
	{
		releaseInfo();
	}
	catch (...)
	{
	}
}

void OBSS_ListObjRslt::destroy()
{
	releaseInfo();
	ObjectQueue.destroy();
}

void OBSS_ListObjRslt::releaseInfo()
{
	strRelease(BucketName); BucketName = NULL;
	strRelease(Prefix); Prefix = NULL;
	strRelease(Marker); Marker = NULL;
	strRelease(Delimiter); Delimiter = NULL;
	strRelease(NextMarker); NextMarker = NULL;
	IsTruncated = false;
	MaxKeys = 0;
}


OBSS_Operation::OBSS_Operation(OBSS_Client* client)
{
	__Client = client;
	__HttpTrans = NULL;
	RunHandle = true;
}

OBSS_Operation::~OBSS_Operation()
{
	try
	{
		__Client = NULL;
		__releaseTrans();
		RunHandle = true;
	}
	catch (...)
	{
	}
}

void OBSS_Operation::__releaseTrans()
{
	if (__HttpTrans)
	{
		delete __HttpTrans;
		__HttpTrans = NULL;
	}
}

const char* OBSS_Operation::__getAuthType()
{
	switch (__Client->ObssType)
	{
		case OBSS_ALI:
			return "OSS";
		case OBSS_JOVISION:
			return "AWS";
		default:
			return NULL;
	}
}

int OBSS_Operation::__getErrorCode(const int rsltCode, const char* errMsg)
{
	switch (rsltCode)
	{
		// 403 Forbidden
		case OBSS_E_FORBIDDEN:
			if (strcmp("SignatureDoesNotMatch", errMsg) ==0)
				return (int)OBSS_E_SIGNATURE_DOES_NOT_MATCH;
			if (strcmp("RequestTimeTooSkewed", errMsg) ==0)
				return (int)OBSS_E_REQUEST_TIME_TOO_SKEWED;
			return (int)OBSS_E_FORBIDDEN;
			
		// 404 Not Found
		case OBSS_E_NOT_FDOUND:
			if (strcmp("NoSuchBucket", errMsg) ==0)
				return (int)OBSS_E_NO_SUCH_BUCKET;
			if (strcmp("NoSuchKey", errMsg) ==0)
				return (int)OBSS_E_NO_SUCH_KEY;
			return (int)OBSS_E_NOT_FDOUND;

		// 400 Bad Request
		case OBSS_E_BED_REQUEST:
			if (strcmp("InvalidObjectName", errMsg) ==0)
				return (int)OBSS_E_INVALID_OBJECT_NAME;
			if (strcmp("InvalidArgument", errMsg) ==0)
				return (int)OBSS_E_INVALID_ARGUMENT;
			return (int)OBSS_E_BED_REQUEST;

		// 409 Conflict
		case OBSS_E_CONFLICT:
			if (strcmp("PositionNotEqualToLength", errMsg) ==0)
				return (int)OBSS_E_POSITION_NOT_EQUAL_TO_LENGTH;
			if (strcmp("ObjectNotAppendable", errMsg) ==0)
				return (int)OBSS_E_OBJECT_NOT_APPENDABLE;
			return (int)OBSS_E_CONFLICT;
		
		default:
			return rsltCode;
	}
}

int OBSS_Operation::__parseErrorCode(const int rsltCode, const char* xmlBuff, const int xmlLen)
{
	CHECK_RET(xmlBuff != NULL, rsltCode);

	int ret = rsltCode;
	XmlNode* xml = NULL;
	XmlNode* xmlNode = NULL;
	
	xml = xml_load_buffer(xmlBuff, xmlLen);
	CHECK_RET(xml != NULL, rsltCode);

	xmlNode = xml_find(xml,"Code");
	if (xmlNode && xmlNode->child)
	{
		DEBUGMSG("Error Code: %s", *xmlNode->child->attrib);
		ret = __getErrorCode(rsltCode, *xmlNode->child->attrib);
	}
	
	xml_free(xml);	
	return ret;
}



/*
<ListBucketResult>
  <Name>albert-test</Name>
  <Prefix></Prefix>
  <Marker></Marker>
  <MaxKeys>10</MaxKeys>
  <Delimiter></Delimiter>
  <IsTruncated>true</IsTruncated>
  <NextMarker>A363/2015/6/12/A363-6.ts</NextMarker>
  <Contents>
    <Key>S123456/2015/6/10/buff_1452_1</Key>
    <LastModified>2015-06-17T00:51:51.000Z</LastModified>
    <ETag>"9819BD726628A3A0DF1305E697B13232"</ETag>
    <Type>Normal</Type>
    <Size>1452</Size>
    <StorageClass>Standard</StorageClass>
    <Owner>
	  <ID>1359231587069645</ID>
	  <DisplayName>1359231587069645</DisplayName>
    </Owner>
  </Contents>
</ListBucketResult>
*/
int OBSS_Operation::__parseListObjRslt(OBSS_ListObjRslt* listObjRslt, const char* xmlBuff, const int xmlLen)
{	
	CHECK_RET(listObjRslt != NULL, RET_ERROR);
	CHECK_RET(xmlBuff != NULL, RET_ERROR);
	CHECK_RET(xmlLen != 0, RET_ERROR);

	XmlNode* xml = NULL;
	XmlNode* xmlNode = NULL;
	
	xml = xml_load_buffer(xmlBuff, xmlLen);
	CHECK_RET(xml != NULL, RET_ERROR);

	listObjRslt->releaseInfo();
	
	xmlNode = xml_find(xml,"Name");
	if (xmlNode)
		listObjRslt->BucketName = strDup(*xmlNode->child->attrib);

	xmlNode = xml_find(xml,"Prefix");
	if (xmlNode && xmlNode->child)
		listObjRslt->Prefix = strDup(*xmlNode->child->attrib);

	xmlNode = xml_find(xml,"Marker");
	if (xmlNode && xmlNode->child)
		listObjRslt->Marker = strDup(*xmlNode->child->attrib);

	xmlNode = xml_find(xml,"Delimiter");
	if (xmlNode && xmlNode->child)
		listObjRslt->Delimiter = strDup(*xmlNode->child->attrib);

	xmlNode = xml_find(xml,"MaxKeys");
	if (xmlNode && xmlNode->child)
		listObjRslt->MaxKeys = atoi(*xmlNode->child->attrib);

	xmlNode = xml_find(xml,"IsTruncated");
	if (xmlNode && xmlNode->child)
	{
		if (strcmp(*xmlNode->child->attrib, "false") == 0)
			listObjRslt->IsTruncated = false;
		else
			listObjRslt->IsTruncated = true;
	}

	xmlNode = xml_find(xml,"NextMarker");
	if (xmlNode && xmlNode->child)
		listObjRslt->NextMarker = strDup(*xmlNode->child->attrib);

	int count = 0;
	int ret = RET_OK;
	
	xmlNode = xml_find(xml,"Contents");
	while (xmlNode)
	{
		ret = listObjRslt->ObjectQueue.pushBack(*xmlNode->child->child->attrib);
		if (ret != RET_OK)
		{
			xml_free(xml);
			return RET_ERROR;
		}
		xmlNode = xmlNode->next;
		count++;
	}

	xmlNode = xml_find(xml, "CommonPrefixes");
	while (xmlNode)
	{
		ret = listObjRslt->ObjectQueue.pushBack(*xmlNode->child->child->attrib);
		if (ret != RET_OK)
		{
			xml_free(xml);
			return RET_ERROR;
		}
		xmlNode = xmlNode->next;
		count++;
	}

	if ((NULL == listObjRslt->NextMarker) && (listObjRslt->IsTruncated) && (count == listObjRslt->MaxKeys) && (count != 0))
	{
		queue_node* qTail = listObjRslt->ObjectQueue.getEnd();
		if (qTail)
			listObjRslt->NextMarker = strDup(qTail->data);
	}
	
	xml_free(xml);
	return RET_OK;
}


int OBSS_Operation::__parseDeleteMultiObjsRslt(OBSS_Queue* objectQueue, const char* xmlBuff, const int xmlLen)
{
	return RET_OK;
}



int OBSS_Operation::CallBack_parseHeadObjInfo(Http_PDU* recvHead)
{
	return RET_OK;
}

int OBSS_Operation::headObjInfo(const char* bucket, const char* object)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(object != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL,RET_ERROR);
	
	Http_Trans httpTrans(&RunHandle);

	char resource[1024] = {0};
	char date[64] = {0};
	char header_authline[1024] = {0};
	char header_line[1024] = {0};
	size_t line_len = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "/%s/%s", bucket, object);
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_HEAD, resource);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(header_line, sizeof(header_line), "Date: %s" HTTP_CRLF, date);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_HEAD, "", "", date, "", resource);
	line_len = snprintf(header_line, sizeof(header_line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	int ret = 0;
	ret = httpTrans.sendHttp(__Client->RemoteHost, __Client->HttpPort);
	if (ret != RET_OK)
	{
		TRACE("sent http header failed: %d", ret);
		return RET_ERROR;
	}
	
	/* Receive Response */
	ret = httpTrans.recvResp();
	if (ret != RET_OK)
	{
		TRACE("receive http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.parseData();
	if (ret != RET_OK)
	{
		TRACE("parse http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.getResult();
	if (ret == RET_ERROR)
	{
		TRACE("parse http response result failed: %d", ret);
		return RET_ERROR;
	}

	DEBUGMSG("Head: \n[%s]\n", httpTrans.RecvHttp.httpHead);
	
	CHECK_RET(ret == 200,ret);

	ret = CallBack_parseHeadObjInfo(&httpTrans.RecvHttp);
	CHECK_RET(ret == RET_OK, RET_ERROR);
	
	return RET_OK;
}


int OBSS_Operation::putObjFromFile(const char* bucket, const char* object, const char* fileName, 
									size_t* billingSize)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(object != NULL, RET_ERROR);
	CHECK_RET(fileName != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL, RET_ERROR);

	Http_Trans httpTrans(&RunHandle);

	/* Open File */
	FILE *fp;
	fp = fopen(fileName, "rb");
	CHECK_RET(fp != NULL, RET_ERROR);
	
	size_t fileSize = fileGetFileSize(fileName);
	if (fileSize == 0)
	{
		fclose(fp);
		TRACE("file size of %s is %zu", fileName, fileSize);
		return RET_ERROR;
	}
	if (billingSize) 
		*billingSize = fileSize;

	char resource[1024] = {0};
	char date[64] = {0};
	char header_authline[1024] = {0};
	char header_line[1024] = {0};
	size_t line_len = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "/%s/%s", bucket, object);
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_PUT, resource);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Content-Length: %zu" HTTP_CRLF, fileSize);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Content-Type: application/octet-stream" HTTP_CRLF);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(header_line, sizeof(header_line), "Date: %s" HTTP_CRLF, date);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_PUT, "", 
						"application/octet-stream", date, "", resource);
	line_len = snprintf(header_line, sizeof(header_line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	int ret = 0;
	ret = httpTrans.sendHttp(__Client->RemoteHost, __Client->HttpPort);
	if (ret != RET_OK)
	{
		TRACE("sent http header failed: %d", ret);
		fclose(fp);
		return RET_ERROR;
	}
	ret = httpTrans.sendContent(fp);
	if (ret != RET_OK)
	{
		TRACE("sent buff failed: %d", ret);
		fclose(fp);
		return RET_ERROR;
	}

	fclose(fp);

	/* Receive Response */
	ret = httpTrans.recvResp();
	if (ret != RET_OK)
	{
		TRACE("receive http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.parseData();
	if (ret != RET_OK)
	{
		TRACE("parse http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.getResult();
	if (ret == RET_ERROR)
	{
		TRACE("parse http response result failed: %d", ret);
		return RET_ERROR;
	}
	DEBUGMSG("Body: \n[%s]\n",httpTrans.RecvHttp.httpBody);

	if (ret != 200)
		return __parseErrorCode(ret, httpTrans.RecvHttp.httpBody, httpTrans.RecvHttp.dataEnd - httpTrans.RecvHttp.httpBody);

	return RET_OK;
}


int OBSS_Operation::putObjFromBuffer(const char* bucket, const char* object, 
							const char* buff, const size_t buffLen)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(object != NULL, RET_ERROR);
	CHECK_RET(buff != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL,RET_ERROR);
	
	Http_Trans httpTrans(&RunHandle);

	char resource[1024] = {0};
	char date[64] = {0};
	char header_authline[1024] = {0};
	char header_line[1024] = {0};
	size_t line_len = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "/%s/%s", bucket, object);
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_PUT, resource);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Content-Length: %zu" HTTP_CRLF, buffLen);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Content-Type: application/octet-stream" HTTP_CRLF);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(header_line, sizeof(header_line), "Date: %s" HTTP_CRLF, date);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_PUT, "", 
						"application/octet-stream", date, "", resource);
	line_len = snprintf(header_line, sizeof(header_line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	int ret = 0;
	ret = httpTrans.sendHttp(__Client->RemoteHost, __Client->HttpPort);
	if (ret != RET_OK)
	{
		TRACE("sent http header failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.sendContent(buff, buffLen);
	if (ret != RET_OK)
	{
		TRACE("sent buff failed: %d", ret);
		return RET_ERROR;
	}
	
	/* Receive Response */
	ret = httpTrans.recvResp();
	if (ret != RET_OK)
	{
		TRACE("receive http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.parseData();
	if (ret != RET_OK)
	{
		TRACE("parse http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.getResult();
	if (ret == RET_ERROR)
	{
		TRACE("parse http response result failed: %d", ret);
		return RET_ERROR;
	}
	
	DEBUGMSG("Body: \n[%s]\n",httpTrans.RecvHttp.httpBody);
	
	if (ret != 200)
		return __parseErrorCode(ret, httpTrans.RecvHttp.httpBody, httpTrans.RecvHttp.dataEnd - httpTrans.RecvHttp.httpBody);
	
	return RET_OK;
}


int OBSS_Operation::putObjFromStream_Init(const char* bucket, const char* object, const size_t streamLen)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(object != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL,RET_ERROR);
	
	__releaseTrans();
	__HttpTrans =  new Http_Trans(&RunHandle);

	char resource[1024] = {0};
	char date[64] = {0};
	char header_authline[1024] = {0};
	char header_line[1024] = {0};
	size_t line_len = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "/%s/%s", bucket, object);
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_PUT, resource);
	(void)__HttpTrans->SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)__HttpTrans->SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Content-Length: %zu" HTTP_CRLF, streamLen);
	(void)__HttpTrans->SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Content-Type: application/octet-stream" HTTP_CRLF);
	(void)__HttpTrans->SendHttp.appendData(header_line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(header_line, sizeof(header_line), "Date: %s" HTTP_CRLF, date);
	(void)__HttpTrans->SendHttp.appendData(header_line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_PUT, "", 
					"application/octet-stream", date, "", resource);
	line_len = snprintf(header_line, sizeof(header_line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)__HttpTrans->SendHttp.appendData(header_line, line_len);
	
	line_len = snprintf(header_line, sizeof(header_line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)__HttpTrans->SendHttp.appendData(header_line, line_len);

	(void)__HttpTrans->SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", __HttpTrans->SendHttp.dataStart);

	/* Send Http */
	int ret = 0;
	ret = __HttpTrans->sendHttp(__Client->RemoteHost, __Client->HttpPort);
	if (ret != RET_OK)
	{
		TRACE("sent http header failed: %d", ret);
		__releaseTrans();
		return RET_ERROR;
	}

	return RET_OK;
}

int OBSS_Operation::putObjFromStream_Send(const char* subStream, const size_t subStreamLen)
{
	CHECK_RET(subStream != NULL, RET_ERROR);
	CHECK_RET(__HttpTrans != NULL, RET_ERROR);
	
	int ret = 0;
	
	ret = __HttpTrans->sendContent(subStream, subStreamLen);
	if (ret != RET_OK)
	{
		TRACE("sent buff failed: %d", ret);
		__releaseTrans();
		return RET_ERROR;
	}

	return RET_OK;
}

int OBSS_Operation::putObjFromStream_Finish()
{	
	CHECK_RET(__HttpTrans != NULL, RET_ERROR);
	
	/* Receive Response */
	int ret = RET_OK;

	ret = __HttpTrans->recvResp();
	if (ret != RET_OK)
	{
		TRACE("receive http responce failed: %d", ret);
		__releaseTrans();
		return RET_ERROR;
	}
	ret = __HttpTrans->RecvHttp.parseData();
	if (ret != RET_OK)
	{
		TRACE("parse http responce failed: %d", ret);
		__releaseTrans();
		return RET_ERROR;
	}
	ret = __HttpTrans->RecvHttp.getResult();
	if (ret == RET_ERROR)
	{
		TRACE("parse http response result failed: %d", ret);
		__releaseTrans();
		return RET_ERROR;
	}
	
	DEBUGMSG("Body: \n[%s]\n",__HttpTrans->RecvHttp.httpBody);
		
	if (ret != 200)
	{
		ret = __parseErrorCode(ret, __HttpTrans->RecvHttp.httpBody, 
							__HttpTrans->RecvHttp.dataEnd - __HttpTrans->RecvHttp.httpBody);
		__releaseTrans();
		return ret;
	}
	
	__releaseTrans();
	return RET_OK;
}

/*
POST /oss_test_append_object_from_file?append&position=0 HTTP/1.1
User-Agent: aliyun-sdk-c/libaos_0.0.3(Compatible oss_test)
Host: albert-test.oss-cn-beijing.aliyuncs.com
Content-Length: 363923
Content-Type: application/x-www-form-urlencoded
Date: Tue, 14 Jul 2015 02:55:30 GMT
Authorization: OSS YTouKEOrlY7blSHn:inKc0exScouWtUUiMjgwQj5HQRs=
*/
int OBSS_Operation::appendObjFromFile(const char* bucket, const char* object, const char* fileName, 
									size_t& position, size_t* billingSize)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(object != NULL, RET_ERROR);
	CHECK_RET(fileName != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL, RET_ERROR);

	Http_Trans httpTrans(&RunHandle);

	/* Open File */
	FILE *fp;
	fp = fopen(fileName, "rb");
	CHECK_RET(fp != NULL, RET_ERROR);
	
	size_t fileSize = fileGetFileSize(fileName);
	if (fileSize == 0)
	{
		fclose(fp);
		TRACE("file size of %s is %zu", fileName, fileSize);
		return RET_ERROR;
	}
	if (billingSize) 
		*billingSize = fileSize;

	char resource[1024] = {0};
	char date[64] = {0};
	char header_authline[1024] = {0};
	char header_line[1024] = {0};
	size_t line_len = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "/%s/%s?append&position=%zu", bucket, object, position);
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_POST, resource);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Content-Length: %zu" HTTP_CRLF, fileSize);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Content-Type: application/octet-stream" HTTP_CRLF);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(header_line, sizeof(header_line), "Date: %s" HTTP_CRLF, date);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_POST, "", 
					"application/octet-stream", date, "", resource);
	line_len = snprintf(header_line, sizeof(header_line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	int ret = 0;
	ret = httpTrans.sendHttp(__Client->RemoteHost, __Client->HttpPort);
	if (ret != RET_OK)
	{
		TRACE("sent http header failed: %d", ret);
		fclose(fp);
		return RET_ERROR;
	}
	ret = httpTrans.sendContent(fp);
	if (ret != RET_OK)
	{
		TRACE("sent buff failed: %d", ret);
		fclose(fp);
		return RET_ERROR;
	}

	fclose(fp);

	/* Receive Response */
	ret = httpTrans.recvResp();
	if (ret != RET_OK)
	{
		TRACE("receive http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.parseData();
	if (ret != RET_OK)
	{
		TRACE("parse http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.getResult();
	if (ret == RET_ERROR)
	{
		TRACE("parse http response result failed: %d", ret);
		return RET_ERROR;
	}
	
	DEBUGMSG("Body: \n[%s]\n", httpTrans.RecvHttp.httpBody);

	const char* position_str = NULL;
	position_str = httpTrans.RecvHttp.getHeader("x-oss-next-append-position");
	if (position_str)
		position = atoi(position_str);

	if (ret != 200)
		return __parseErrorCode(ret, httpTrans.RecvHttp.httpBody, httpTrans.RecvHttp.dataEnd - httpTrans.RecvHttp.httpBody);

	return RET_OK;
}


int OBSS_Operation::appendObjFromBuffer(const char* bucket, const char* object, size_t& position,
							const char* buff, const size_t buffLen)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(object != NULL, RET_ERROR);
	CHECK_RET(buff != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL,RET_ERROR);
	
	Http_Trans httpTrans(&RunHandle);

	char resource[1024] = {0};
	char date[64] = {0};
	char header_authline[1024] = {0};
	char header_line[1024] = {0};
	size_t line_len = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "/%s/%s?append&position=%zu", bucket, object, position);
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_POST, resource);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Content-Length: %zu" HTTP_CRLF, buffLen);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Content-Type: application/octet-stream" HTTP_CRLF);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(header_line, sizeof(header_line), "Date: %s" HTTP_CRLF, date);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_POST, "", 
						"application/octet-stream", date, "", resource);
	line_len = snprintf(header_line, sizeof(header_line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	int ret = 0;
	ret = httpTrans.sendHttp(__Client->RemoteHost, __Client->HttpPort);
	if (ret != RET_OK)
	{
		TRACE("sent http header failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.sendContent(buff, buffLen);
	if (ret != RET_OK)
	{
		TRACE("sent buff failed: %d", ret);
		return RET_ERROR;
	}
	
	/* Receive Response */
	ret = httpTrans.recvResp();
	if (ret != RET_OK)
	{
		TRACE("receive http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.parseData();
	if (ret != RET_OK)
	{
		TRACE("parse http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.getResult();
	if (ret == RET_ERROR)
	{
		TRACE("parse http response result failed: %d", ret);
		return RET_ERROR;
	}
	
	DEBUGMSG("Body: \n[%s]\n",httpTrans.RecvHttp.httpBody);

	const char* position_str = NULL;
	position_str = httpTrans.RecvHttp.getHeader("x-oss-next-append-position");
	if (position_str)
		position = atoi(position_str);
	
	if (ret != 200)
		return __parseErrorCode(ret, httpTrans.RecvHttp.httpBody, httpTrans.RecvHttp.dataEnd - httpTrans.RecvHttp.httpBody);
	
	return RET_OK;
}


int OBSS_Operation::getObj2File(const char* bucket, const char* object, const char* fileName, 
					const size_t rangeLen, const size_t rangeStart)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(object != NULL, RET_ERROR);
	CHECK_RET(fileName != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL,RET_ERROR);

	Http_Trans httpTrans(&RunHandle);

	/* Open File */
	FILE *fp;
	fp = fopen(fileName, "wb");
	CHECK_RET(fp != NULL,RET_ERROR);

	char resource[1024] = {0};
	char date[64] = {0};
	char header_authline[1024] = {0};
	char header_line[1024] = {0};
	size_t line_len = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "/%s/%s", bucket, object);
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_GET, resource);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	line_len = snprintf(header_line, sizeof(header_line), "Content-Type: application/octet-stream" HTTP_CRLF);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(header_line, sizeof(header_line), "Date: %s" HTTP_CRLF, date);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_GET, "", 
			"application/octet-stream", date, "", resource);
	line_len = snprintf(header_line, sizeof(header_line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	if (rangeLen)
	{
		line_len = snprintf(header_line, sizeof(header_line), "Range: bytes=%zu-%zu" HTTP_CRLF, rangeStart, rangeStart + rangeLen - 1);
		(void)httpTrans.SendHttp.appendData(header_line, line_len);
	}
	
	line_len = snprintf(header_line, sizeof(header_line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	int ret = 0;
	ret = httpTrans.sendHttp(__Client->RemoteHost, __Client->HttpPort);
	if (ret != RET_OK)
	{
		TRACE("sent http header failed: %d", ret);
		fclose(fp);
		(void)remove(fileName);
		return RET_ERROR;
	}
	
	/* Receive Response */
	ret = httpTrans.recvHeader();
	if (ret != RET_OK)
	{
		TRACE("receive http header failed: %d", ret);
		fclose(fp);
		(void)remove(fileName);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.getResult();
	if (ret == RET_ERROR)
	{
		TRACE("parse http header result failed: %d", ret);
		fclose(fp);
		(void)remove(fileName);
		return RET_ERROR;
	}
		
	if (ret != 200 && ret != 206)
	{
		TRACE("parse http response result: %d", ret);
		fclose(fp);
		(void)remove(fileName);
		return __parseErrorCode(ret, httpTrans.RecvHttp.httpBody, httpTrans.RecvHttp.dataEnd - httpTrans.RecvHttp.httpBody);
	}

	ret = httpTrans.saveContent(fp);
	fclose(fp);
	if (ret != RET_OK)
	{
		(void)remove(fileName);
		return RET_ERROR;
	}
	
	return RET_OK;
}


int OBSS_Operation::getObj2Buff(const char* bucket, const char* object, char* dataBuff,
								const size_t rangeLen, const size_t rangeStart)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(object != NULL, RET_ERROR);
	CHECK_RET(dataBuff != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL,RET_ERROR);

	Http_Trans httpTrans(&RunHandle);

	char resource[1024] = {0};
	char date[64] = {0};
	char header_authline[1024] = {0};
	char header_line[1024] = {0};
	size_t line_len = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "/%s/%s", bucket, object);
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_GET, resource);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	line_len = snprintf(header_line, sizeof(header_line), "Content-Type: application/octet-stream" HTTP_CRLF);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(header_line, sizeof(header_line), "Date: %s" HTTP_CRLF, date);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_GET, "", 
			"application/octet-stream", date, "", resource);
	line_len = snprintf(header_line, sizeof(header_line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	if (rangeLen)
	{
		line_len = snprintf(header_line, sizeof(header_line), "Range: bytes=%zu-%zu" HTTP_CRLF, rangeStart, rangeStart + rangeLen - 1);
		(void)httpTrans.SendHttp.appendData(header_line, line_len);
	}
	
	line_len = snprintf(header_line, sizeof(header_line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	int ret = 0;
	ret = httpTrans.sendHttp(__Client->RemoteHost, __Client->HttpPort);
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
	if(ret != 200 && ret != 206)
	{
		TRACE("parse http response result: %d", ret);
		return __parseErrorCode(ret, httpTrans.RecvHttp.httpBody, httpTrans.RecvHttp.dataEnd - httpTrans.RecvHttp.httpBody);
	}

	ret = httpTrans.saveContent(dataBuff, rangeLen);
	CHECK_RET(ret == RET_OK, RET_ERROR);
	
	return RET_OK;
}

/*
DELETE /ObjectName HTTP/1.1
Host: BucketName.oss-cn-hangzhou.aliyuncs.com
Date: GMT Date
Authorization: SignatureValue
*/
int OBSS_Operation::deleteObject(const char* bucket, const char* object)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(object != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL, RET_ERROR);

	Http_Trans httpTrans(&RunHandle);

	char resource[1024] = {0};
	char date[64] = {0};
	char header_authline[1024] = {0};
	char header_line[1024] = {0};
	size_t line_len = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "/%s/%s", bucket, object);
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_DELETE, resource);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(header_line, sizeof(header_line), "Date: %s" HTTP_CRLF, date);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_DELETE, "", "", date, "", resource);
	line_len = snprintf(header_line, sizeof(header_line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	int ret = 0;
	ret = httpTrans.sendHttp(__Client->RemoteHost, __Client->HttpPort);
	if (ret != RET_OK)
	{
		TRACE("sent http header failed: %d", ret);
		return RET_ERROR;
	}
	
	/* Receive Response */
	ret = httpTrans.recvResp();
	if (ret != RET_OK)
	{
		TRACE("receive http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.parseData();
	if (ret != RET_OK)
	{
		TRACE("parse http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.getResult();
	if (ret == RET_ERROR)
	{
		TRACE("parse http response result failed: %d", ret);
		return RET_ERROR;
	}
	DEBUGMSG("Body: \n[%s]\n",httpTrans.RecvHttp.httpBody);

	if (ret != 204)
		return __parseErrorCode(ret, httpTrans.RecvHttp.httpBody, httpTrans.RecvHttp.dataEnd - httpTrans.RecvHttp.httpBody);

	return RET_OK;
}


/*
POST /?delete HTTP/1.1
Host: BucketName.oss-cn-hangzhou.aliyuncs.com
Date: GMT Date
Content-Length: ContentLength
Content-MD5: MD5Value
Authorization: SignatureValue

<?xml version="1.0" encoding="UTF-8"?>
<Delete>
<Quiet>true</Quiet>
<Object>
<Key>key</Key>
</Object>
...
</Delete>
*/
int OBSS_Operation::deleteMultiObjects(const char* bucket, OBSS_Queue* objectQueue, const bool isQuiet)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(objectQueue != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL, RET_ERROR);

	char line[1024] = {0};
	size_t line_len = 0;
	int ret = 0;

	/* From HTTP xml Body */
	OBSS_Buffer xml_body;

	(void)xml_body.appendData("<Delete>", strlen("<Delete>"));
	
	if (isQuiet == true)
		(void)xml_body.appendData("<Quiet>true</Quiet>", strlen("<Quiet>true</Quiet>"));
	else 
		(void)xml_body.appendData("<Quiet>false</Quiet>", strlen("<Quiet>false</Quiet>"));

	uint32 xml_len = objectQueue->getLen();
	char obj_key[1024] = {0};
	for (uint32 i = 1; i <= xml_len; ++i)
	{
		ret = objectQueue->popFront(obj_key,sizeof(obj_key));
		CHECK_RET(ret == RET_OK, RET_ERROR);
		line_len = snprintf(line, sizeof(line), "<Object><Key>%s</Key></Object>", obj_key);
		(void)xml_body.appendData(line, line_len);
	}
	
	(void)xml_body.appendData("</Delete>", strlen("</Delete>"));
	
	xml_len = xml_body.getDataLen();


	/* From HTTP Header */
	char resource[1024] = {0};
	char date[64] = {0};
	char header_authline[1024] = {0};
	Http_Trans httpTrans(&RunHandle);
	
	(void)snprintf(resource, sizeof(resource), "/%s/?delete", bucket);
	
	line_len = snprintf(line, sizeof(line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_POST, resource);
	(void)httpTrans.SendHttp.appendData(line, line_len);

	line_len = snprintf(line, sizeof(line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)httpTrans.SendHttp.appendData(line, line_len);

	line_len = snprintf(line, sizeof(line), "Content-Length: %u" HTTP_CRLF, xml_len);
	(void)httpTrans.SendHttp.appendData(line, line_len);

	char base64Md5[64] = {0};
	ComputeMd5Digest(base64Md5, (unsigned char*)xml_body.dataStart, xml_len);
	line_len = snprintf(line, sizeof(line), "Content-MD5: %s" HTTP_CRLF, base64Md5);
	(void)httpTrans.SendHttp.appendData(line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(line, sizeof(line), "Date: %s" HTTP_CRLF, date);
	(void)httpTrans.SendHttp.appendData(line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_POST, base64Md5, "", date, "", resource);
	line_len = snprintf(line, sizeof(line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)httpTrans.SendHttp.appendData(line, line_len);

	line_len = snprintf(line, sizeof(line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)httpTrans.SendHttp.appendData(line, line_len);

	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));
	
	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	ret = httpTrans.sendHttp(__Client->RemoteHost, __Client->HttpPort);
	if (ret != RET_OK)
	{
		TRACE("sent http header failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.sendContent(xml_body.dataStart, xml_len);
	if (ret != RET_OK)
	{
		TRACE("sent xml body failed: %d", ret);
		return RET_ERROR;
	}
	
	/* Receive Response */
	ret = httpTrans.recvResp();
	if (ret != RET_OK)
	{
		TRACE("receive http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.parseData();
	if (ret != RET_OK)
	{
		TRACE("parse http responce failed: %d", ret);
		return RET_ERROR;
	}
	ret = httpTrans.RecvHttp.getResult();
	if (ret == RET_ERROR)
	{
		TRACE("parse http response result failed: %d", ret);
		return RET_ERROR;
	}
	DEBUGMSG("Body: \n[%s]\n", httpTrans.RecvHttp.httpBody);

	if (ret != 200)
		return __parseErrorCode(ret, httpTrans.RecvHttp.httpBody, httpTrans.RecvHttp.dataEnd - httpTrans.RecvHttp.httpBody);

	return RET_OK;
}


int OBSS_Operation::deleteDir(const char* bucket, const char* prefix)
{
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(prefix != NULL, RET_ERROR);
	
	int ret = 0;
	OBSS_ListObjRslt listObjRslt;

	do
	{
		ret = listObject(&listObjRslt, bucket, NULL, NULL, 100, prefix);
		CHECK_RET(ret == RET_OK, ret);
		
		ret = deleteMultiObjects(bucket, &(listObjRslt.ObjectQueue), true);
		CHECK_RET(ret == RET_OK, ret);
		
	}while (listObjRslt.NextMarker);

	return RET_OK;
}


/*
GET /?prefix=fun/&delimiter=/ HTTP/1.1
Host: oss-example.oss-cn-hangzhou.aliyuncs.com
Date: Fri, 24 Feb 2012 08:43:27 GMT
Authorization: OSS qn6qrrqxo2oawuk53otfjbyc:DNrnx7xHk3sgysx7I8U9I9IY1vY=
*/
int OBSS_Operation::listObject(OBSS_ListObjRslt* listObjRslt, const char* bucket,  
							const char* delimiter, const char* marker, const int maxKeys, const char* prefix)
{
	CHECK_RET(listObjRslt != NULL, RET_ERROR);
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET( maxKeys >= 0 && maxKeys <= 1000, RET_ERROR);
	CHECK_RET(__Client != NULL, RET_ERROR);

	Http_Trans httpTrans(&RunHandle);

	char 		resource[1024] = {0};
	char 		tmp_resrc[1024] = {0};
	char 		date[64] = {0};
	char 		header_authline[1024] = {0};
	char 		header_line[1024] = {0};
	size_t 		line_len = 0;
	int 		exist_param = 0;

	/* From HTTP Header */
	(void)snprintf(resource, sizeof(resource), "/%s", bucket);
	if (delimiter) {
		strcpy(tmp_resrc, resource);
		(void)snprintf(resource, sizeof(resource), "%s%sdelimiter=%s", tmp_resrc, (exist_param++ == 0) ? "/?": "&", delimiter);
	}
	if (marker) {
		strcpy(tmp_resrc, resource);
		(void)snprintf(resource, sizeof(resource), "%s%smarker=%s", tmp_resrc, (exist_param++ == 0) ? "/?": "&", marker);
	}
	if (maxKeys) {
		strcpy(tmp_resrc, resource);
		(void)snprintf(resource, sizeof(resource), "%s%smax-keys=%d", tmp_resrc, (exist_param++ == 0) ? "/?": "&", maxKeys);
	}
	if (prefix) {
		strcpy(tmp_resrc, resource);
		(void)snprintf(resource, sizeof(resource), "%s%sprefix=%s", tmp_resrc, (exist_param++ == 0) ? "/?": "&", prefix);
	}
	
	line_len = snprintf(header_line, sizeof(header_line), "%s %s " HTTP_VERSION HTTP_CRLF, HTTP_VERB_GET, resource);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "Host: %s" HTTP_CRLF, __Client->RemoteHost);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	time_GetGmt(date, sizeof(date));
	line_len = snprintf(header_line, sizeof(header_line), "Date: %s" HTTP_CRLF, date);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);
	
	(void)FormAuthLine(header_authline, __Client->AccessId, __Client->AccessKey, HTTP_VERB_GET, "", "", date, "", resource);
	line_len = snprintf(header_line, sizeof(header_line), "Authorization: %s %s" HTTP_CRLF, __getAuthType(), header_authline);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	line_len = snprintf(header_line, sizeof(header_line), "User-Agent: %s" HTTP_CRLF, __Client->UserAgent);
	(void)httpTrans.SendHttp.appendData(header_line, line_len);

	(void)httpTrans.SendHttp.appendData(HTTP_CRLF, strlen(HTTP_CRLF));

	DEBUGMSG("HTTP REQ HEADER:\n[%s]\n", httpTrans.SendHttp.dataStart);

	/* Send Data */
	int ret = 0;
	ret = httpTrans.sendHttp(__Client->RemoteHost, __Client->HttpPort);
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
		return __parseErrorCode(ret, httpTrans.RecvHttp.httpBody, httpTrans.RecvHttp.dataEnd - httpTrans.RecvHttp.httpBody);
	}

	/* Receive & Parse Body */
	OBSS_Buffer	obssBuff;
	ret = httpTrans.saveBody(&obssBuff);
	CHECK_RET(ret == RET_OK, RET_ERROR);
	
	DEBUGMSG("Body:\n%s\n", obssBuff.dataStart);

	ret = __parseListObjRslt(listObjRslt, obssBuff.dataStart, obssBuff.getDataLen());
	if (ret != RET_OK)
	{
		TRACE("parse result of list object failed: %d", ret);
		return RET_ERROR;
	}

	return RET_OK;
}


int OBSS_Operation::listAllObject(OBSS_ListObjRslt* listObjRslt, const char* bucket,  
							const char* delimiter, const char* prefix)
{	
	int	ret = 0;

	do
	{
		ret = listObject(listObjRslt, bucket, delimiter, listObjRslt->NextMarker, 100, prefix);
		CHECK_RET(ret == RET_OK, ret);
		
	}while (listObjRslt->NextMarker);

	return RET_OK;
}


int OBSS_Operation::genSignUrl(char* signUrl,
					 const char* method, const char* bucket, const char* object, time_t expires)
{
	CHECK_RET(signUrl!= NULL, RET_ERROR);
	CHECK_RET(method != NULL, RET_ERROR);
	CHECK_RET(bucket != NULL, RET_ERROR);
	CHECK_RET(object != NULL, RET_ERROR);
	CHECK_RET(__Client != NULL,RET_ERROR);
	
	char resource[1024] = {0};
	char sign[1024] = {0};
	char encoded_sign[1024] = {0};
	char nowStr[64] = {0};
	time_t nowt;
	int ret = 0;
	
	(void)time(&nowt);
	nowt += expires;
	(void)snprintf(nowStr, sizeof(nowStr), "%ld",nowt);
	(void)snprintf(resource, sizeof(resource), "/%s/%s", bucket, object);

	ret = FormSignature(sign, __Client->AccessKey, method, "", "", nowStr, "", resource);
	if (ret != RET_OK)
	{
		TRACE("form signature failed!");
		return RET_ERROR;
	}
	ret = EncodeUrl(encoded_sign, sign);
	if (ret != RET_OK)
	{
		TRACE("url encoded failed!");
		return RET_ERROR;
	}
	
	memset(resource, 0, sizeof(resource));

	switch (__Client->ObssType)
	{
		case OBSS_ALI:
			(void)snprintf(resource, sizeof(resource), "http://%s.%s/%s?Expires=%s&OSSAccessKeyId=%s&Signature=%s", 
							bucket, __Client->RemoteHost, object, nowStr, __Client->AccessId, encoded_sign);
			break;
		case OBSS_JOVISION:
			(void)snprintf(resource, sizeof(resource), "http://%s/%s/%s?AWSAccessKeyId=%s&Expires=%s&Signature=%s", 
							__Client->RemoteHost, bucket, object, __Client->AccessId, nowStr, encoded_sign);
			break;
		default:
			return RET_ERROR;
	}
	strcpy(signUrl, resource);
	
	return RET_OK;
}

