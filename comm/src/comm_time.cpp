/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "comm_time.h"


void time_GetGmt(char *gmtTime, int maxlen)
{
	time_t		cur_time;
	struct tm 	gmt_tm;

	(void)time(&cur_time);
	(void)gmtime_r(&cur_time, &gmt_tm);

	memset(gmtTime, 0, maxlen);

	(void)strftime(gmtTime, maxlen - 1, "%a, %d %b %Y %H:%M:%S GMT", &gmt_tm);

	return;
}

