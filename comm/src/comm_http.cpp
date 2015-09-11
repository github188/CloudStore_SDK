/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#include "comm_net.h"
#include "comm_http.h"


#define HTTP_TCP_BUF_SIZE	1452


Http_PDU::Http_PDU()
{
    httpHead = httpBody = NULL;
    
}
Http_PDU::~Http_PDU()
{	
	httpHead = httpBody = NULL;
}

int Http_PDU::parseData()
{
	CHECK_RET(dataStart != NULL, RET_ERROR);
	CHECK_RET(dataEnd != NULL, RET_ERROR);
		
	httpHead = NULL;
	httpBody = NULL;
	
	char* pos = NULL;
	
	/* find header and body */
	pos = (char* )strstr(dataStart, HTTP_HEADER_END);
	CHECK_RET(pos != NULL, RET_ERROR);

	httpHead = dataStart;
	pos[0] = '\0';
	pos += strlen(HTTP_HEADER_END);
	if (pos < dataEnd)
	{
		httpBody = pos;
	}

	/* format header*/
	char* header = httpHead;
	while(NULL != (pos=strstr(header,HTTP_CRLF)))
	{
		pos[0] = '\0';
		pos[1] = '\0';
		header = pos + 2;
	}

    return RET_OK;
}

const char* Http_PDU::getHeader(const char* key)
{
	CHECK_RET(key != NULL, NULL);
	CHECK_RET(httpHead != NULL, NULL);
	CHECK_RET(dataEnd != NULL, NULL);
	
	char* headerLine = httpHead;
	const char* header_end = httpBody ? httpBody : dataEnd;
	char* header = NULL;

	while(headerLine < header_end)
	{
		header =strstr(headerLine, key);
		if (header)
		{
			header =strstr(header, ":");
			if (!header)
			{
				return NULL;
			}
			do
			{
				header++;
			}
			while (' ' == *header || '\t' == *header || '\"' == *header);
			char* ptr = header;
			do
			{
				ptr++;
				if ( '\"' == *ptr)
				{
					*ptr = '\0';
					break;
				}
			}
			while (*ptr != '\0');

			return header;
		}

		headerLine += (strlen(headerLine) + 1);
	}

    return NULL;
}

int Http_PDU::getResult()
{
	CHECK_RET(httpHead != NULL, RET_ERROR);

	const char* pos = strstr(httpHead, HTTP_VERSION);
	CHECK_RET(pos != NULL, RET_ERROR);

	int seperate = 0;
	char rsltCode[8] = {0};
	char rsltInfo[64] = {0};
	
	seperate = sscanf(pos + strlen(HTTP_VERSION) + 1, "%s %s", rsltCode, rsltInfo);
	CHECK_RET((seperate==1)||(seperate==2), RET_ERROR);
	
	strcpy(rsltInfo, pos + strlen(HTTP_VERSION) + strlen(rsltCode) + 2);
	
	DEBUGMSG("Result: Code-[%s]  Info-[%s]", rsltCode, rsltInfo);
	
	return atoi(rsltCode);
}


Http_Trans::Http_Trans(bool* RunHandler)
{
	__RunHandler = RunHandler;
}

Http_Trans::~Http_Trans()
{
	__RunHandler = NULL;
}

int Http_Trans::sendHttp(const char * __hostName, uint16 __port)
{
	int		tcp_ret = 0;

	tcp_ret = __TcpTrans.config(__hostName, __port);
	CHECK_RET(tcp_ret == RET_OK, RET_ERROR);
	
	tcp_ret = __TcpTrans.connect();
	CHECK_RET(tcp_ret == RET_OK, RET_ERROR);
	
	/* Send HTTP Header */
	tcp_ret = __TcpTrans.send(SendHttp.dataStart, SendHttp.getDataLen());
	if (tcp_ret < 0) 
	{
		TRACE("send http headers error: %d", tcp_ret);
		return RET_ERROR;
	}

	return RET_OK;
}

int Http_Trans::sendContent(FILE* fp)
{
	CHECK_RET(fp != NULL, RET_ERROR);
	
	int		ret = 0;
	char	tcp_buf[HTTP_TCP_BUF_SIZE] = {0};
	
	while((ret = fread(tcp_buf, 1, sizeof(tcp_buf), fp)) >= 0)
	{
		ret = __TcpTrans.send(tcp_buf, ret);
		if (ret < 0) 
		{
			TRACE("send http content file error: %d", ret);
			return RET_ERROR;
		}
		if(feof(fp))
			break;
	}
	
	return RET_OK;
}

int Http_Trans::sendContent(const char* buff, const size_t buffLen)
{
	CHECK_RET(buff != 0, RET_ERROR);
	
	int		tcp_ret = 0;
	char	tcp_buf[HTTP_TCP_BUF_SIZE] = {0};

	const char *stream_pos = buff;
	const char *stream_end = buff + buffLen;
	size_t send_len = 0;
	while(stream_pos < stream_end) 
	{
		send_len = stream_end - stream_pos;
		send_len = send_len >= HTTP_TCP_BUF_SIZE ? HTTP_TCP_BUF_SIZE : send_len;
		memcpy(tcp_buf,stream_pos,send_len);
		tcp_ret = __TcpTrans.send(tcp_buf, send_len);
		if (tcp_ret < 0) 
		{
			TRACE("send http content buff error: %d", tcp_ret);
			return RET_ERROR;
		}
		stream_pos += send_len;
	}

	return RET_OK;
}


int Http_Trans::recvResp()
{
	int		tcp_ret = 0;
	char	tcp_buf[HTTP_TCP_BUF_SIZE] = {0};

	do {
		memset(tcp_buf, 0, sizeof(tcp_buf));

		tcp_ret = __TcpTrans.recv(tcp_buf, 1024);
		if (tcp_ret < 0)
		{
			TRACE("recv http response error: %d", tcp_ret);
			return RET_ERROR;
		}
		
		DEBUGMSG("RECV DATA:\n[%s]\n", tcp_buf);
		
		(void)RecvHttp.appendData(tcp_buf, tcp_ret);
		
	} while(tcp_ret == 1024);
	
	return RET_OK;
}

int Http_Trans::recvHeader()
{
	int		tcp_ret = 0;
	int		ret = 0;
	char	tcp_buf[HTTP_TCP_BUF_SIZE] = {0};

	do {
		memset(tcp_buf, 0, sizeof(tcp_buf));

		tcp_ret = __TcpTrans.recv(tcp_buf, 1024);
		if (tcp_ret < 0)
		{
			TRACE("recv http header error: %d\n", tcp_ret);
			return RET_ERROR;
		}
		
		DEBUGMSG("RECV DATA:\n[%s]\n", tcp_buf);
		
		(void)RecvHttp.appendData(tcp_buf, tcp_ret);
		ret = RecvHttp.parseData();
		
	} while((tcp_ret == 1024) && (ret != RET_OK));
	
	return RET_OK;
}

int Http_Trans::saveContent(FILE* fp)
{
	int		ret = 0;
	char	tcp_buf[HTTP_TCP_BUF_SIZE] = {0};
	size_t	saved_content_len = 0;
	size_t	content_len = 0;

	const char* len_str = RecvHttp.getHeader("Content-Length");
	CHECK_RET(len_str != NULL, RET_ERROR);
	content_len = atoi(len_str);
	CHECK_RET(content_len > 0, RET_ERROR);
	
	if (RecvHttp.httpBody)
	{
		ret = RecvHttp.dataEnd - RecvHttp.httpBody;
		fwrite(RecvHttp.httpBody, ret, 1, fp);
		saved_content_len += ret;
	}

	ret = 1;
	while((saved_content_len < content_len) && (ret > 0)) 
	{
		if (__RunHandler && !(*__RunHandler))
		{
			return RET_ERROR;
		}
		
		memset(tcp_buf, 0, sizeof(tcp_buf));
		ret = __TcpTrans.recv(tcp_buf, 1024);
		if (ret < 0)
		{
			TRACE("recv http content error: %d\n", ret);
			return RET_ERROR;
		}
				
		fwrite(tcp_buf, ret, 1, fp);
		saved_content_len += ret;
	} 
	
	return RET_OK;
}

int Http_Trans::saveContent(char* dataBuff, const size_t rangeLen)
{
	int		ret = 0;
	char	tcp_buf[HTTP_TCP_BUF_SIZE] = {0};
	size_t	saved_content_len = 0;
	size_t	content_len = 0;

	const char* len_str = RecvHttp.getHeader("Content-Length");
	CHECK_RET(len_str != NULL, RET_ERROR);
	content_len = atoi(len_str);
	CHECK_RET(content_len > 0, RET_ERROR);
	CHECK_RET(content_len <= rangeLen, RET_ERROR);
	
	if (RecvHttp.httpBody)
	{
		ret = RecvHttp.dataEnd - RecvHttp.httpBody;
		memcpy(dataBuff + saved_content_len, RecvHttp.httpBody, ret);
		saved_content_len += ret;
	}

	ret = 1;
	while((saved_content_len < content_len) && (ret > 0))
	{
		if (__RunHandler && !(*__RunHandler))
		{
			return RET_ERROR;
		}

		memset(tcp_buf, 0, sizeof(tcp_buf));
		ret = __TcpTrans.recv(tcp_buf, 1024);
		if (ret < 0)
		{
			TRACE("recv http content error: %d\n", ret);
			return RET_ERROR;
		}

		if ((saved_content_len + ret) <= rangeLen)
		{
			memcpy(dataBuff + saved_content_len, tcp_buf, ret);
			saved_content_len += ret;
		}
		else
		{
			TRACE("recv content length over buffer length: saved_content_len=%zu,ret=%d,rangeLen=%zu\n", 
				saved_content_len, ret, rangeLen);
			return RET_ERROR;
		}		
	}
	
	return RET_OK;
}


int Http_Trans::__parseChunk(COMM_Buffer* obssBuff, const COMM_Buffer* chunkBuff)
{
	CHECK_RET(obssBuff != NULL, RET_ERROR);
	CHECK_RET(chunkBuff != NULL, RET_ERROR);

	const char* chunk_end = NULL;
	chunk_end = strstr(chunkBuff->dataStart, "0\r\n\r\n");
	CHECK_RET(chunk_end != NULL, RET_ERROR);
	
	uint32 	chunk_len = 0;
	const char* pos = NULL;
	const char* chunk_start = chunkBuff->dataStart;
	
	while (chunk_start < chunk_end)
	{
		pos = strstr(chunk_start, HTTP_CRLF);
		CHECK_RET(pos != NULL, RET_ERROR);
		
		chunk_len = strtoul(chunk_start, NULL, 16);
		pos += 2;
		if (chunk_len > 0)
		{
			(void)obssBuff->appendData(pos, chunk_len);
		}
		chunk_start = pos + chunk_len + 1;
	}

	return RET_OK;
}


int Http_Trans::saveBody(COMM_Buffer* obssBuff)
{
	int		ret = 0;
	char	tcp_buf[HTTP_TCP_BUF_SIZE] = {0};
	size_t	saved_content_len = 0;
	size_t	content_len = 0;
	const char* http_header = NULL;

	// Content-Length: xxx
	http_header = RecvHttp.getHeader("Content-Length");
	if (http_header)
	{
		content_len = atoi(http_header);
		CHECK_RET(content_len > 0, RET_ERROR);

		if (RecvHttp.httpBody)
		{
			ret = RecvHttp.dataEnd - RecvHttp.httpBody;
			(void)obssBuff->appendData(RecvHttp.httpBody, ret);
			saved_content_len += ret;
		}
		
		DEBUGMSG("saved_content_len = %zu, content_len = %zu", saved_content_len, content_len);
		
		ret = 1;
		while((saved_content_len < content_len) && (ret > 0))
		{
			memset(tcp_buf, 0, sizeof(tcp_buf));
			ret = __TcpTrans.recv(tcp_buf, 1024);
			if (ret < 0)
			{
				TRACE("recv http content error: %d\n", ret);
				return RET_ERROR;
			}

			(void)obssBuff->appendData(tcp_buf, ret);
			saved_content_len += ret;
		}

		return RET_OK;
	}
	
	//Transfer-Encoding: chunked
	http_header = RecvHttp.getHeader("Transfer-Encoding");	
	CHECK_RET(http_header != NULL, RET_ERROR);
	CHECK_RET(strcmp(http_header, "chunked") == 0, RET_ERROR);
	
	COMM_Buffer chunkBuff;
	if (RecvHttp.httpBody)
	{
		ret = RecvHttp.dataEnd - RecvHttp.httpBody;
		(void)chunkBuff.appendData(RecvHttp.httpBody, ret);
	}
	
	ret = 1;
	while ((strstr(chunkBuff.dataStart, "0\r\n\r\n") == NULL) && (ret > 0))
	{
			memset(tcp_buf, 0, sizeof(tcp_buf));
			ret = __TcpTrans.recv(tcp_buf, 1024);
			if (ret < 0)
			{
				TRACE("recv http content error: %d, break while.\n", ret);
				break;
			}
	
			(void)chunkBuff.appendData(tcp_buf, ret);
	}
	
	ret = __parseChunk(obssBuff, &chunkBuff);
	CHECK_RET(ret == RET_OK,RET_ERROR);
	
	return RET_OK;
}


