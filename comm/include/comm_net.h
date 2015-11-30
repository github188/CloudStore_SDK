/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __COMM_NET_H__
#define __COMM_NET_H__


/*****************
* TCP Socket Api *
*****************/
int tcp_GetHostIp(const char *hostName,  char *ipAddr, int ipLen);

int tcp_Connect(const char *host, unsigned short port, int timeout);

int tcp_Send(int fd, const char* pData,int len, int timeout);

int tcp_Recv(int fd, char* pData, int len, int timeout);

void tcp_Close(int fd);


/*****************
* UDP Socket Api *
*****************/
int udp_Connect(const char* hostip, const int hostport);

int udp_Send(int sock, const char* pData, int len, int timeout);

int udp_Recv(int sock, char* pData, int len, int timeout);

void udp_Close(int sock);


#endif /* __COMM_NET_H__ */

