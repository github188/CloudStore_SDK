/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#include <sys/time.h>
#include "jvn_flux.h"

//default flux border is 10M 
#define DEFAULT_FLUX_BORDER		(10*1024*1024)

//Max flux border is 1GB
#define MAX_FLUX_CACHE			(1024*1024*1024)

//Max flux for each msg is 64MB
#define MAX_FLUX_4_MSG			(64*1024*1024)

JVN_Flux::JVN_Flux():__CurrFluxByte(0), __FluxBorder(DEFAULT_FLUX_BORDER), __DevYSTNO(0)
{
	memset(__DevGroup, 0, sizeof(__DevGroup[4]));
}

JVN_Flux::~JVN_Flux()
{
}

int JVN_Flux::config(const char* remoteIp, const int remotePort, int fluxBorder, char devGroup[4], int devYSTNO)
{
	__FluxBorder = fluxBorder;
	memcpy(__DevGroup, devGroup, sizeof(__DevGroup[4]));
	__DevYSTNO = devYSTNO;

	int ret = RET_OK;
	ret = __UdpTrans.config(remoteIp,remotePort);
	CHECK_RET(ret == RET_OK, RET_ERROR);
	__UdpTrans.close();
	ret = __UdpTrans.connect();
	CHECK_RET(ret == RET_OK, RET_ERROR);

	return RET_OK;
}


int JVN_Flux::reportFlux(const int fluxType, const int fluxByte)
{
	if (__CurrFluxByte > MAX_FLUX_CACHE)
	{
		TRACE("CurrFluxByte was over MAX_FLUX_CACHE(1G)!");
		return RET_ERROR;
	}

	int ret = RET_OK;
	int report_fluxKB = 0;
	char msg[64] = {0};
	
	__CurrFluxByte += fluxByte;
	if (__CurrFluxByte >= fluxByte)
	{
		report_fluxKB = (__CurrFluxByte < MAX_FLUX_4_MSG) ? __CurrFluxByte : MAX_FLUX_4_MSG;
		DEBUGMSG("report_fluxKB = %d", report_fluxKB);
		
		ret = genFluxMsg(msg, sizeof(msg), fluxType, toKilobyte(report_fluxKB));
		CHECK_RET(ret == RET_OK, RET_ERROR);
	}
	
	for (int i=0; i<34; ++i)
		printf("%x ", msg[i]);
	printf("\n");
	
	ret = sendFluxMsg(msg, 34);
	CHECK_RET(ret == RET_OK, RET_ERROR);
	
	return RET_OK;
}

int JVN_Flux::reconnectServer()
{
	int ret = RET_OK;

	__UdpTrans.close();
	ret = __UdpTrans.connect();
	CHECK_RET(ret == RET_OK, RET_ERROR);

	return RET_OK;
}

int JVN_Flux::sendFluxMsg(const char* msgData, const size_t msgLen)
{
	int ret = RET_OK;
	
	ret = __UdpTrans.send(msgData, msgLen);
	if (ret < 0)
	{
		ret = reconnectServer();
		CHECK_RET(ret == RET_OK, RET_ERROR);

		//send again
		ret = __UdpTrans.send(msgData, msgLen);
		CHECK_RET(ret == RET_OK, RET_ERROR);
	}

	DEBUGMSG("send ok");
	
	char recvData[128] = {0};
	memset(recvData, 0, sizeof(recvData));
	ret = __UdpTrans.recv(recvData, sizeof(recvData));
	CHECK_RET(ret == RET_OK, RET_ERROR);
	DEBUGMSG("recv ok");

	return RET_OK;
}

unsigned long JVN_Flux::getCurrTime()
{
	unsigned long curr_ms = 0;

	//struct timespec ts;
    //clock_gettime(CLOCK_MONOTONIC, &ts);
    //curr_ms = (unsigned long)(ts.tv_sec*1000 + ts.tv_nsec/1000000);
	struct timeval tv;
	gettimeofday(&tv,NULL);
	curr_ms = (unsigned long)(tv.tv_sec*1000 + tv.tv_usec/1000);

	return curr_ms;
}

int JVN_Flux::genFluxMsg(char* msgData, const size_t msgLen, int fluxType, int fluxKB)
{
	CHECK_RET(msgData != NULL, RET_ERROR);
	CHECK_RET(msgLen != 0, RET_ERROR);

	memset(msgData, 0, msgLen);
	
	//The Msq length is 34 bytes
	int nType = 0xFC;
	int nLen = 26;
	memcpy(&msgData[0], &nType, 4);
	memcpy(&msgData[4], &nLen, 4);
	memcpy(&msgData[8], __DevGroup, 4);
	memcpy(&msgData[12], &__DevYSTNO, 4);
	//[16] == 0

	int ID = getCurrTime();
	memcpy(&msgData[18], &ID,4);
	memcpy(&msgData[22], &fluxType, 2);
	//[24] == 0 //Account Info
	//int dataSize = __CurrFluxByte / 1024; // UOM is KB
	memcpy(&msgData[26], &fluxKB, 2);
	//[28]	//Port
	//[30]	//IP

	return RET_OK;
}

inline int JVN_Flux::toKilobyte(int byte)
{
	return (byte/1024);
}

