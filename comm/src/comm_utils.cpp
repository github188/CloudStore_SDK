/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/


#include <sys/stat.h>
#include "comm_utils.h"


char* strDup(const char* str)
{
	CHECK_RET(str != NULL, NULL);
	
	char* copy = NULL;
    size_t strLen = strlen(str);

    copy = (char* )malloc(sizeof(char) * strLen + 1);
	CHECK_RET(copy != NULL, NULL);

    memcpy(copy, str, strLen);
    copy[strLen] = '\0';

    return copy;
}

void strRelease(char *str)
{
	if(str != NULL)
		free(str);
	return;
}

size_t fileGetFileSize(const char *fileName) 
{ 
	struct stat buf; 
	if(stat(fileName, &buf)<0) 
	{ 
		return 0; 
	} 
	return (size_t)buf.st_size; 
}


