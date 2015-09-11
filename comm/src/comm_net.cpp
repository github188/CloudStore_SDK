/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "comm_net.h"
#include "comm_utils.h"


int tcp_GetHostIp(const char *hostName,  char *ipAddr, int ipLen)
{
	struct addrinfo hints;
	struct addrinfo *rslt, *curr;
	int ret;
	struct sockaddr_in *addr;
	char ipbuf[16] = {0};

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;		/* Allow IPv4 */
	hints.ai_flags = AI_PASSIVE;	/* For wildcard IP address */
	hints.ai_protocol = 0;			/* Any protocol */
	hints.ai_socktype = (int)SOCK_STREAM;

 	/* 
 	* int getaddrinfo( const char *hostname, const char *service, 
 	*				const struct addrinfo *hints, struct addrinfo **result );
 	*/
	ret = getaddrinfo(hostName, NULL,&hints,&rslt);
	if (ret != 0) {
		TRACE("ret = %d, error: [%d] %s", ret, errno, gai_strerror(errno));
		return -1;
	}
	
	for (curr = rslt; curr != NULL; curr = curr->ai_next) {
		addr = (struct sockaddr_in *)curr->ai_addr;
		memset(ipbuf, 0, sizeof(ipbuf));
		(void)snprintf(ipAddr, ipLen, "%s", inet_ntop(AF_INET, &addr->sin_addr, ipbuf, sizeof(ipbuf)));
	}

	freeaddrinfo(rslt);
	return 0;
}

int tcp_Connect(const char *hostip, unsigned short port, int timeout)
{
	struct sockaddr_in	addr;
	int					fd = -1;
	int					ret = 0;
	struct timeval		tv;
	fd_set				set;
	int					flags;

	if (!hostip || port == 0) 
	{
		return -1;
	}
	
	fd = socket(AF_INET, (int)SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(hostip);
	addr.sin_port = htons(port);
	
	flags = fcntl(fd, F_GETFL, 0);
	(void)fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	ret = connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)); 
	if (ret == 0) 
	{
		return fd;
	}

	if (errno != EINPROGRESS) 
	{
		TRACE("hostip=%s, port=%d, ret=%d, error: [%d] %s", hostip, port, ret, errno, strerror(errno));
		close(fd);
		return -1;
	}

	FD_ZERO(&set);
	FD_SET(fd, &set);

	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	ret = select(fd + 1, NULL, &set, NULL, &tv);
	if (ret == -1)
	{
		TRACE("hostip=%s, port=%d, fd=%d, ret=%d, error: [%d] %s", hostip, port, fd, ret, errno, strerror(errno));
		close(fd);
		return -1;
	} 
	else if(ret == 0)
	{
		TRACE("hostip=%s, port=%d, fd=%d, ret=%d, error: [%d] %s", hostip, port, fd, ret, errno, strerror(errno));
		close(fd);
		return -1;
	}

	return fd;
}

int tcp_Send(int fd, const char* pData, int len, int timeout)
{
		    int				ret = -1;
	int				sendlen = 0;
	struct timeval	tv;
	fd_set			writefd;
	int				retry = 0;
	char			*ptr = (char*) pData;

	while( sendlen < len ) 
	{
		FD_ZERO(&writefd);

		if(fd <= 0) 
		{
			TRACE("fd=%d", fd);
			return -1;
		}

		FD_SET(fd, &writefd);

		tv.tv_sec = timeout;
		tv.tv_usec = 0;

		ret = select(fd + 1, NULL, &writefd, NULL, &tv);
		if (ret <= 0)
		{
			TRACE("fd=%d, ret=%d, error: [%d] %s", fd, ret, errno, strerror(errno));
			return -1;
		}

		ret = send(fd, ptr+sendlen, len-sendlen, 0);
		if (ret > 0)
		{
			sendlen += ret;
			retry = 0;
		}
		else 
		{
			if (retry++ > 5) 
			{
				TRACE("fd=%d, ret=%d, error: [%d] %s", fd, ret, errno, strerror(errno));
				return -1;
			}
		}
	}

	return sendlen;
}

int tcp_Recv(int fd, char* pData, int len, int timeout)
{
	int				ret = 0;
	struct timeval	tv;
	fd_set			readfd;

	FD_ZERO(&readfd);
	FD_SET(fd, &readfd);

	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	ret = select(fd + 1, &readfd, NULL, NULL, &tv);
	if(ret < 0) {
		TRACE("fd=%d, ret=%d, error: [%d] %s", fd, ret, errno, strerror(errno));
		return -1;
	} 

	ret = recv(fd, pData, len, 0);
	if(ret < 0) {
		TRACE("fd=%d, ret=%d, error: [%d] %s", fd, ret, errno, strerror(errno));
	} 
	
	return ret;
}

void tcp_Close(int fd)
{
	close(fd);
	return;
}


