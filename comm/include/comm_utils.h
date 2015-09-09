/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __OBSS_UTILS_H__
#define __OBSS_UTILS_H__


#include <string.h>
#include <stdlib.h>
#include <stdio.h>


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

#define FALSE 0
#define TRUE 1

#define RET_OK		0
#define RET_ERROR	-1


/*
 * Traceability Defination
*/
#define __OBSS_TRACE
//#define __OBSS_CHECK
//#define __OBSS_DEBUG

#ifdef __OBSS_CHECK
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

#ifdef __OBSS_TRACE
#define TRACE(format,...) fprintf(stdout, __FILE__ ":%d TRACE(%s): " format "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define TRACE(format,...)
#endif

#ifdef __OBSS_DEBUG
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



#endif /* __OBSS_UTILS_H__ */

