/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/


#include "comm_utils.h"
#include "comm_data.h"


#define OBSS_BUFFER_INIT_SIZE	(1024)

COMM_Buffer::COMM_Buffer()
{
    __buffStart = __buffEnd = dataStart = dataEnd = NULL;
    
}
COMM_Buffer::~COMM_Buffer()
{
	try
	{
		if (__buffStart)
		{
			free(__buffStart);
		}
    
		__buffStart = __buffEnd = dataStart = dataEnd = NULL;
	}
	catch (...)
	{
	}
}

int COMM_Buffer::__checkData(const int needSize) 
{
	int			buf_size = OBSS_BUFFER_INIT_SIZE;
	int			data_len = 0;
	char *		newbuf;

	if (__buffStart == NULL)
	{
		while (buf_size < needSize) {
			buf_size <<= 1;
		}
		
		__buffStart = dataStart = dataEnd = (char *)malloc(buf_size);
		if (NULL == __buffStart) {
			return RET_ERROR;
		}
		memset(__buffStart, 0, buf_size);
		
		__buffEnd = __buffStart + buf_size;
	}
	else if (__buffEnd && dataEnd && dataStart && __buffStart && (__buffEnd - dataEnd) < needSize) 
	{
		data_len = dataEnd - dataStart;
		buf_size = __buffEnd - __buffStart;

		buf_size <<= 1;
		while ((buf_size - data_len) < needSize) {
			buf_size <<= 1;
		}

		newbuf = (char *)malloc(buf_size);
		if (NULL == newbuf) {
			return RET_ERROR;
		}
		memset(newbuf, 0, buf_size);
		
		if (data_len > 0) {
			memcpy(newbuf, dataStart, data_len);
		}
		free(__buffStart);

		__buffStart = newbuf;
		dataStart = __buffStart;
		dataEnd = __buffStart + data_len;
		__buffEnd = __buffStart + buf_size;
	}

	return RET_OK;
}


size_t COMM_Buffer::appendData(const char *addData, const size_t addLen)
{
	if (RET_OK != __checkData((int)addLen)) {
		return 0;
	}

	memcpy(dataEnd, addData, addLen);

	dataEnd += addLen;

	return addLen;
}

size_t COMM_Buffer::getDataLen()
{
	if (dataStart && dataEnd)
		return (dataEnd - dataStart);
	else
		return 0;
}


queue_node::queue_node()
{		
	data = NULL;
	next = NULL;
}
queue_node::~queue_node()
{
	try
	{
		strRelease(data);
		next = NULL;
	}
	catch (...)
	{
	}
}


COMM_Queue::COMM_Queue()
{
	__head = NULL;
	__tail = NULL;
	__len = 0;
}

COMM_Queue::~COMM_Queue()
{
	try
	{
		destroy();
	}
	catch (...)
	{
	}
}

void COMM_Queue::destroy()
{
	queue_node* next;
	queue_node* current = __head;
	
	while (current)
	{
		next = current->next;
		delete current;
		current = next;
	}
	
	__head = NULL;
	__tail = NULL;
	__len = 0;

	return;
}

uint32 COMM_Queue::getLen()
{
	return __len;
}

queue_node* COMM_Queue::getStart()
{
	return __head;
}

queue_node* COMM_Queue::getEnd()
{
	return __tail;
}

int COMM_Queue::pushBack(const char* data)
{
	CHECK_RET(data != NULL, RET_ERROR);
	
	char*	data_str = NULL;
	data_str = strDup(data);
	CHECK_RET(data_str != NULL, RET_ERROR);

	queue_node*	new_node = new queue_node();
	new_node->data = data_str;

	if(__head && __tail)
	{
		__tail->next = new_node;
		__tail = __tail->next;
		
	}
	else if (!__head && !__tail)
	{
		__head = new_node;
		__tail = __head;
	}
	else
	{
		delete new_node;
		return RET_ERROR;
	}
	
	__len++;

	return RET_OK;
}

int COMM_Queue::popFront(char* data, const size_t len)
{
	CHECK_RET(data != NULL, RET_ERROR);
	CHECK_RET(__head != NULL, RET_ERROR);
	CHECK_RET(strlen(__head->data) + 1 <= len, RET_ERROR);

	strcpy(data,__head->data);

	queue_node*	current = __head;
	if(__head == __tail)
		__head = __tail = NULL;
	else
		__head = __head->next;

	__len--;
	delete current;

	return RET_OK;
}

const char* COMM_Queue::getByIndex(uint32 id)
{
	CHECK_RET(__head != NULL, NULL);
	CHECK_RET(id <= __len, NULL);

	queue_node*	current = __head;
	for(uint32 i = 1; i < id; ++i)
	{
		current = current->next;
	}
	
	return current->data;
}

