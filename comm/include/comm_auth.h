/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __COMM_AUTH_H__
#define __COMM_AUTH_H__


int FormAuthLine(char* authLine,
					   const char* accessId, const char* secretKey, 
					   const char* verb, const char* md5, const char* type, const char *date, 
					   const char* obssHeaders, const char* resource);

int FormSignature(char *signature, const char* secretKey, 
						const char* verb, const char* md5, const char* type, const char *date, 
						const char* obssHeaders, const char* resource);

int EncodeUrl(char* encoded_url, char const *url);


void ComputeMd5Digest(char* base64Md5, const unsigned char* pData, int dataLen);

#endif /* __COMM_AUTH_H__ */

