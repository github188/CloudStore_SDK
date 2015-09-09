/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/


#include "obss_utils.h"
#include "obss_data-struct.h"


#define OBSS_BUFFER_INIT_SIZE	(1024)

OBSS_Buffer::OBSS_Buffer()
{
    __buffStart = __buffEnd = dataStart = dataEnd = NULL;
    
}
OBSS_Buffer::~OBSS_Buffer()
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

int OBSS_Buffer::__checkData(const int needSize) 
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


size_t OBSS_Buffer::appendData(const char *addData, const size_t addLen)
{
	if (RET_OK != __checkData((int)addLen)) {
		return 0;
	}

	memcpy(dataEnd, addData, addLen);

	dataEnd += addLen;

	return addLen;
}

size_t OBSS_Buffer::getDataLen()
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


OBSS_Queue::OBSS_Queue()
{
	__head = NULL;
	__tail = NULL;
	__len = 0;
}

OBSS_Queue::~OBSS_Queue()
{
	try
	{
		destroy();
	}
	catch (...)
	{
	}
}

void OBSS_Queue::destroy()
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

uint32 OBSS_Queue::getLen()
{
	return __len;
}

queue_node* OBSS_Queue::getStart()
{
	return __head;
}

queue_node* OBSS_Queue::getEnd()
{
	return __tail;
}

int OBSS_Queue::pushBack(const char* data)
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

int OBSS_Queue::popFront(char* data, const size_t len)
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

const char* OBSS_Queue::getByIndex(uint32 id)
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

