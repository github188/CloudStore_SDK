/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __OBSS_OPERATION_H__
#define __OBSS_OPERATION_H__


#include "comm_http.h"


class OBSS_ListObjRslt
{
public:
	OBSS_ListObjRslt();
	virtual ~OBSS_ListObjRslt();
public:
	void destroy();
	void releaseInfo();
public:
	char*		BucketName;
	char*		Prefix;
	char*		Marker;
	char*		Delimiter;
	int			MaxKeys;
	bool		IsTruncated;
	char*		NextMarker;
	COMM_Queue	ObjectQueue;
};


class OBSS_Operation
{
public:
	OBSS_Operation(OBSS_Client* client);
	virtual ~OBSS_Operation();

public:
	
	/**
	* Call Back Func for parsing head info
	* @param [in] bucket 		is the bucket name which is valid on OBSS, 
								and please ensure it has read permission.
	* @param [in] object 		is the object name on OBSS.
	* @return	 0		success
				-1		internal error **/
	virtual int CallBack_parseHeadObjInfo(Http_PDU* recvHead);

	/**
	* Get head info of object on OBSS
	* @param [in] bucket 		is the bucket name which is valid on OBSS, 
								and please ensure it has read permission.
	* @param [in] object 		is the object name on OBSS.
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int headObjInfo(const char* bucket, const char* object);
	
	/**
	* Upload data from local file to OBSS
	* @param [in] bucket 		is the bucket name which is valid on OBSS, 
								and please ensure it has wirte permission.
	* @param [in] object 		is the object name that used on OBSS.
	* @param [in] fileName 		is the name of local file.
	* @param [out] billingSize 	is the length of local file to be used for billing.
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int putObjFromFile(const char* bucket, const char* object, const char* fileName, size_t* billingSize = NULL);


	/** 
	* Upload data from memory space to OBSS
	* @param [in] bucket 	is the bucket name which is valid on OBSS, 
							and please ensure it has wirte permission.
	* @param [in] object 	is the object name that used on OBSS.
	* @param [in] buff 		is the data buff.
	* @param [in] buffLen 	is the length of data buff.
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int putObjFromBuffer(const char* bucket, const char* object, const char* buff, const size_t buffLen);

	/** 
	* Initialize for uploading data from stream to OBSS
	* @param [in] bucket 	is the bucket name which is valid on OBSS, 
							and please ensure it has wirte permission.
	* @param [in] object 	is the object name that used on OBSS.
	* @param [in] streamLen is the totle length of data stream.
	* @return	 0		success
				-1		internal error	**/
	int putObjFromStream_Init(const char* bucket, const char* object, const size_t streamLen);

	/** 
	* Send one part of totle stream to OBSS
	* @param [in] subStream is one part of totle stream.
	* @param [in] subStreamLen is the length of sub-stream.
	* @return	 0		success
				-1		internal error	**/
	int putObjFromStream_Send(const char* subStream, const size_t subStreamLen);

	/** 
	* Finish for uploading data from stream to OBSS
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int putObjFromStream_Finish();

	/**
	* Download object from OBSS to local file
	* @param [in]  bucket 		is the bucket name which is available on OBSS, 
								and please ensure it has read permission.
	* @param [in]  object 		is the object name which is available on OBSS.
	* @param [out] fileName 	is the name of local file to save the object.
	* @param [in]  rangeLen		is the length of specified range bytes default is 0.
								If set it t to onzero, the specified range bytes of an object will be downloaded. 
	* @param [in]  rangeStart	is the start bytes of specified range.
								Default is 0, means the range start from origin of the object. 
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				4042	No Such Key (Object)
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int getObj2File(const char* bucket, const char* object, const char* fileName, const size_t rangeLen = 0, const size_t rangeStart = 0);


	/**
	* Download object from OBSS to memory space
	* @param [in]  bucket 		is the bucket name which is available on OBSS, 
								and please ensure it has read permission.
	* @param [in]  object 		is the object name which is available on OBSS.
	* @param [out] dataBuff 	is the memory buffer to save the object.
	* @param [in]  rangeLen		is the length of specified range bytes, default is 0.
								If set it t to onzero, the specified range bytes of an object will be downloaded. 
	* @param [in]  rangeStart	is the start bytes of specified range. 
								Default is 0, means the range start from origin of the object. 
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				4042	No Such Key (Object)
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int getObj2Buff(const char* bucket, const char* object, char* dataBuff, const size_t rangeLen, const size_t rangeStart = 0);


	/**
	* Delete specified object on OBSS
	* @param [in]  bucket 		is the bucket name which is available on OBSS, 
								and please ensure it has write permission.
	* @param [in]  object 		is the object name.
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int deleteObject(const char* bucket, const char* object);

	/**
	* Delete multi-objects on OBSS
	* @param [in] bucket 		is the bucket name which is available on OBSS, 
								and please ensure it has write permission.
	* @param [in] objectQueue 	is a queue of objects which whould be deleted.
	* @param [in] isQuiet 		is option the enable/disable quiet mode for the request.
								true - enable quiet mode, only return deleteResult of the object that deleting failed.
								false - disable quiet mode, return deleteResult of all objects.
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int deleteMultiObjects(const char* bucket, COMM_Queue* objectQueue, const bool isQuiet = true);


	/**
	* Delete all objects under the specified directory 
	* @param [in]  bucket 		is the bucket name which is available on OBSS, 
								and please ensure it has write permission.
	* @param [in]  prefix 		is the name of the directory
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				4042	No Such Key (Object)
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int deleteDir(const char* bucket, const char* prefix);


	/**
	* List objects under the specified bucket (at most 1000 objects once)
	* @param [out]  listObjRslt saves the result of listing object.
	* @param [in]  	bucket 		is the bucket name which is available on OBSS, 
								and please ensure it has read permission.
	* @param [in]  	delimiter 	is a string you use to group keys.
	* @param [in]  	marker		is the specified key to start with when listing objects in a bucket. 
	* @param [in]  	maxKeys		is the maximum number of keys returned in the response. Max of it is 1000.
	* @param [in]  	prefix		is the specified prefix to limits the response to keys that begin with.
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				4042	No Such Key (Object)
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int listObject(OBSS_ListObjRslt* listObjRslt, const char* bucket, 
					const char* delimiter = NULL, const char* marker = NULL, const int maxKeys = 0, const char* prefix = NULL);


	/**
	* List all objects under the specified bucket 
	* @param [out]  listObjRslt saves the result of listing object.
	* @param [in]  	bucket 		is the bucket name which is available on OBSS, 
								and please ensure it has read permission.
	* @param [in]  	delimiter 	is a string you use to group keys.
	* @param [in]  	prefix		is the specified prefix to limits the response to keys that begin with.
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				4042	No Such Key (Object)
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	**/
	int listAllObject(OBSS_ListObjRslt* listObjRslt, const char* bucket, 
					const char* delimiter = NULL, const char* prefix = NULL);

	
	/**
	* Generate a sign URL for specified object on OBSS
	* @param [out]  signUrl 	is the sign URL of the specified object.
	* @param [in]  	method 		is the http method.
	* @param [in]  	bucket 		is the bucket name which is available on OBSS, 
								and please ensure it has read permission.
	* @param [in] 	object 		is the available object on OBSS.
	* @param [in]  	expires		is the seconds of the sign URL timeout.
	* @return	 0		success
				-1		internal error	**/
	int genSignUrl(char* signUrl, const char* method, const char* bucket, const char* object, time_t expires);


/****************************************************************************************************
*  Following interfaces should be used on Ali OSS only.
*****************************************************************************************************/

	/** 
	* Append data from local file to an appendable object on OBSS (Only for Ali OSS)
	* @param [in] bucket 		is the bucket name which is appendable on OBSS, 
								and please ensure it has wirte permission.
	* @param [in] object 		is the object name that will be available on OBSS.
	* @param [in] fileName 		is the name of local file.
	* @param [in/out] position 	is the length of the existing object. The buff will be appended after the position, 
								and it will return the length of appended object for next appending. 
	* @param [out] billingSize 	is the length of local file to be used for billing.
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	
				409		Conflict
				4091	Position is not equal to length	of object
				4092	the object is not appendable	**/
	int appendObjFromFile(const char* bucket, const char* object, const char* fileName, 
						size_t& position, size_t* billingSize = NULL);

	/** 
	* Append data from memory to an appendable object on OBSS (Only for Ali OSS)
	* @param [in] bucket 		is the bucket name which is appendable on OBSS, 
								and please ensure it has wirte permission.
	* @param [in] object 		is the object name that available on OBSS.
	* @param [in/out] position 	is the length of the existing object. The buff will be appended after the position, 
								and it will return the length of appended object for next appending. 
	* @param [in] buff 			is the data buff .
	* @param [in] buffLen 		is the length of data buff.
	* @return	 0		success
				-1		internal error
				400 	Bad Request
				4001	Invalid Object Name
				404 	Not Found
				4041 	No Such Bucket
				403 	Forbidden
				4031	Signature Does Not Match
				4032	Request Time Too Skewed (over 15mins)	
				409		Conflict
				4091	Position is not equal to length	of object
				4092	the object is not appendable	**/
	int appendObjFromBuffer(const char* bucket, const char* object, size_t& position, const char* buff, const size_t buffLen);


private:
	void __releaseTrans();
	const char* __getAuthType();
	int __getErrorCode(const int rsltCode, const char* errMsg);
	int __parseErrorCode(const int rsltCode, const char* xmlBuff, const int xmlLen);
	int __parseListObjRslt(OBSS_ListObjRslt* listObjRslt, const char* xmlBuff, const int xmlLen);

public:
	bool			RunHandle;

private:
	OBSS_Client* 	__Client;
	Http_Trans*		__HttpTrans;
};


#endif /* __OBSS_OPERATION_H__ */

