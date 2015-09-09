/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __OBSS_LIST_H__
#define __OBSS_LIST_H__


class OBSS_Buffer
{
public:
	OBSS_Buffer();
	virtual ~OBSS_Buffer();
public:
	size_t appendData(const char *addData, const size_t addLen);
	size_t getDataLen();
private:
	int __checkData(const int needSize);
public:
	char*	dataStart;		/* start of data */
	char*	dataEnd;		/* end of data */
private:
	char*	__buffStart;	/* start of buffer */
	char*	__buffEnd;		/* end of buffer */
};


class queue_node
{
public:
	queue_node();
	virtual ~queue_node();
public:
	char*				data;
	class queue_node*	next;
};


class OBSS_Queue
{
public:
	OBSS_Queue();
	virtual ~OBSS_Queue();
public:
	int pushBack(const char* data);
	int popFront(char* data, const size_t len);
	const char* getByIndex(uint32 id);
	uint32 getLen();
	queue_node* getStart();
	queue_node* getEnd();
	void destroy();
private:
	queue_node*	__head;
	queue_node*	__tail;
	uint32		__len;
};

#endif /* __OBSS_LIST_H__ */

