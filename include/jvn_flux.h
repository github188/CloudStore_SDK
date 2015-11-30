/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __JVN_FLUX_H__
#define __JVN_FLUX_H__


#include "comm_udptrans.h"


class JVN_Flux
{
public:
	JVN_Flux();
	virtual ~JVN_Flux();

public:
	int config(const char* remoteIp, const int remotePort, int fluxBorder, char devGroup[4], int devYSTNO);
	int reportFlux(const int fluxType, const int fluxByte);
private:
	int reconnectServer();
	int sendFluxMsg(const char* msgData, const size_t msgLen);
	unsigned long getCurrTime();
	int genFluxMsg(char* msgData, const size_t msgLen, int fluxType, int fluxKB);
	inline int toKilobyte(int byte);
private:
	Udp_Trans	__UdpTrans;
	int			__CurrFluxByte;	// UOM is BYTE
	int			__FluxBorder;	// UOM is BYTE
	char		__DevGroup[4];	// group id of the device
	int			__DevYSTNO;		// YST NO. of the device
};


#endif /* __JVN_FLUX_H__ */

