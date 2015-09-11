/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __COMM_UTILS_H__
#define __COMM_UTILS_H__


#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define RET_OK		0
#define RET_ERROR	-1

/*
 * Types Defination
*/
typedef char					int8;
typedef unsigned char           uint8;  
typedef short					int16;
typedef unsigned short  		uint16;
typedef int           			int32;
typedef unsigned int            uint32;
typedef unsigned long           ulong;
typedef long long               int64;
typedef unsigned long long      uint64;


/*
 * Traceability Defination
*/
#define __CS_TRACE_
#define __CS_CHECK_
#define __CS_DEBUG_

#ifdef __CS_CHECK_
#define CHECK(val) { \
    if (!(val)) { \
        fprintf(stdout, __FILE__ ":%d [%s] CHECK(" #val ") failed!\n", __LINE__, __FUNCTION__);	\
        return;	\
    }\
}
#define CHECK_RET(val, ret) { \
    if (!(val)) { \
        fprintf(stdout, __FILE__ ":%d [%s] CHECK_RET(" #val ") failed!\n", __LINE__, __FUNCTION__);	\
        return ret; \
    }\
}
#else
#define CHECK(val) { \
    if (!(val)) { \
        return;	\
    }\
}
#define CHECK_RET(val, ret) { \
    if (!(val)) { \
        return ret; \
    }\
}
#endif

#ifdef __CS_TRACE_
#define TRACE(format,...) fprintf(stdout, __FILE__ ":%d TRACE(%s): " format "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define TRACE(format,...)
#endif

#ifdef __CS_DEBUG_
#define DEBUGMSG(format,...) fprintf(stdout, __FILE__":%d [%s] DEBUG: "format"\n", __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define DEBUGMSG(format, ...)
#endif


/*
 * String Func
*/
char* strDup(const char* str);

void strRelease(char *str);


/*
 * Files operations
*/
size_t fileGetFileSize(const char *fileName);



#endif /* __COMM_UTILS_H__ */

