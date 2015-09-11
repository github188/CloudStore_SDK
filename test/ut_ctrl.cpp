/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#include "gtest/gtest.h"
#include "cloud_control.h"

#define CLOUD_CTL_HOST		"172.16.31.45"
#define CLOUD_CTL_PORT		8080
#define CLOUD_CTL_BASE_RES	"/publicService/httpApi/deviceListData.do"


/**********************************
* Tests of Ali Oss				  *
***********************************/
CLOUD_Controler	g_cloudControler(CLOUD_CTL_HOST, CLOUD_CTL_PORT, CLOUD_CTL_BASE_RES);


TEST(CONTROLER, setCloudSwitch)
{
	int ret = RET_OK;
	ret = g_cloudControler.setCloudSwitch("12345",1);
	EXPECT_EQ(RET_OK, ret);
}

TEST(CONTROLER, getCloudSwitch)
{
	int ret = RET_OK;
	int cloudSw = 0;
	ret = g_cloudControler.getCloudSwitch("12345", cloudSw);
	EXPECT_EQ(RET_OK, ret);
}

