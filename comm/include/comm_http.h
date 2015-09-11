/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __COMM_HTTP_H__
#define __COMM_HTTP_H__


#include "comm_tcptrans.h"
#include "comm_data.h"


#define HTTP_VERSION		"HTTP/1.1"

#define HTTP_VERB_GET		"GET"
#define HTTP_VERB_PUT		"PUT"
#define HTTP_VERB_HEAD		"HEAD"
#define HTTP_VERB_POST		"POST"
#define HTTP_VERB_DELETE	"DELETE"

#define HTTP_HEADER_END		"\r\n\r\n"
#define HTTP_CRLF			"\r\n"


class Http_PDU: public COMM_Buffer
{
public:
	Http_PDU();
	virtual ~Http_PDU();
public:
	int parseData();
	const char* getHeader(const char* key);
	int getResult();
public:
	char*			httpHead;	/* start of http header */
	char*			httpBody;	/* start of http body */
};


class Http_Trans
{
public:
    Http_Trans(bool* RunHandler);
    virtual ~Http_Trans();
public:
	int sendHttp(const char * __hostName, uint16 __port);
	int sendContent(FILE* fp);
	int sendContent(const char* buff, const size_t buffLen);
	int recvResp();
	int recvHeader();
	int saveContent(FILE* fp);
	int saveContent(char* dataBuff, const size_t rangeLen);
	int saveBody(COMM_Buffer* obssBuff);
private:
	int __parseChunk(COMM_Buffer* obssBuff, const COMM_Buffer* chunkBuff);
public:
    Http_PDU    SendHttp;
    Http_PDU    RecvHttp;
private:
    Tcp_Trans   __TcpTrans;
	bool*		__RunHandler;
};

#endif /* __COMM_HTTP_H__ */

