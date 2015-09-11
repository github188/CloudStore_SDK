/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/


#include "comm_base64.h"
#include "comm_hmac-sha1.h"
#include "comm_md5.h"
#include "comm_utils.h"
#include "comm_auth.h"


int FormSignature(char *signature, const char* secretKey, 
							  const char* verb, const char* md5, const char* type, const char *date, 
							  const char* obssHeaders, const char* resource)
{
    unsigned char outHmacSha1[20] = {0};
    char signStr[2048] = {0};

    size_t len = strlen(verb) + strlen(md5) + strlen(type) + strlen(date) + strlen(obssHeaders) + strlen(resource);
	CHECK_RET(sizeof(signStr) > len, RET_ERROR);
	
    (void)snprintf(signStr, sizeof(signStr),"%s\n%s\n%s\n%s\n%s%s", verb, md5, type, date, obssHeaders, resource);
    DEBUGMSG("Auth Str:\n[%s]\n", signStr);
	
    HMAC_SHA1(outHmacSha1, (unsigned char*)secretKey, strlen(secretKey), (unsigned char*)signStr, strlen(signStr));    
    (void)base64Encode(outHmacSha1, sizeof(outHmacSha1), signature);
	
	return RET_OK;
}

int FormAuthLine(char* authLine, 
						const char* accessId, const char* secretKey, 
						const char* verb, const char* md5, const char* type, const char *date, 
						const char* obssHeaders, const char* resource)
{
	char formed_res[1024] = {0};
	char sign[64] = {0};
    char* pos;

	CHECK_RET(sizeof(formed_res) > strlen(resource), RET_ERROR);

	memset(formed_res, 0, sizeof(formed_res));
	pos = (char* )strstr(resource, "?");
	if (pos)
	{
		if(strstr(pos, "?delete")
			|| strstr(pos, "?acl")
			|| strstr(pos, "?uploads")
			|| strstr(pos, "?uploadId")
			|| strstr(pos, "?partNumber")
			|| strstr(pos, "?group")
			|| strstr(pos, "?append"))
		{
			strcpy(formed_res, resource);
		}
		else
		{
			memcpy(formed_res, resource, (pos - resource));
		}
	}
	else
	{
		strcpy(formed_res, resource);
	}

	if (!FormSignature(sign, secretKey, verb, md5, type, date, obssHeaders, formed_res))
	{
		memset(formed_res, 0, sizeof(formed_res));
		(void)snprintf(formed_res, sizeof(formed_res), "%s:%s", accessId, sign);
		strcpy(authLine, formed_res);
		return RET_OK;
	}
	
	return RET_ERROR;
}


#define ENCODE_HEX_CHARS	"0123456789ABCDEF"
int EncodeUrl(char* encoded_url, char const *url)
{
	CHECK_RET(encoded_url != NULL, RET_ERROR);
	CHECK_RET(url != NULL, RET_ERROR);
	
	unsigned char hexchars[] = ENCODE_HEX_CHARS;
	register unsigned char c = 0;
    unsigned char *to = NULL;
    unsigned char const *from = NULL;
	unsigned char const *end = NULL;
	size_t url_len = strlen(url);
	
    from = (unsigned char *)url;
    end  = (unsigned char *)url + url_len;
    to = (unsigned char *)encoded_url;

    while (from < end)
    {
        c = *from++;

        if (c == ' ')
        {
            *to++ = '+';
        }
        else if ((c < '0' && c != '-' && c != '.') ||
                 (c < 'A' && c > '9') ||
                 (c > 'Z' && c < 'a' && c != '_') ||
                 (c > 'z'))
        {
            to[0] = '%';
            to[1] = hexchars[c >> 4];
            to[2] = hexchars[c & 15];
            to += 3;
        }
        else
        {
            *to++ = c;
        }
    }
    *to = 0;
	
	return RET_OK;
}


void ComputeMd5Digest(char* base64Md5, const unsigned char* pData, int dataLen)
{
	char md5_digest[17];
	md5_state_t md5_state;

	memset(md5_digest, 0, sizeof(md5_digest));

	obss_md5_init(&md5_state);
	obss_md5_append(&md5_state, pData, dataLen);
	obss_md5_finish(&md5_state, (md5_byte_t *)md5_digest);

	(void)base64Encode((unsigned char* )md5_digest, 16, base64Md5);

	return;
}


