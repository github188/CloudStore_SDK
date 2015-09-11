/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __COMM_DATA_H__
#define __COMM_DATA_H__


class COMM_Buffer
{
public:
	COMM_Buffer();
	virtual ~COMM_Buffer();
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


class COMM_Queue
{
public:
	COMM_Queue();
	virtual ~COMM_Queue();
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

#endif /* __COMM_DATA_H__ */

