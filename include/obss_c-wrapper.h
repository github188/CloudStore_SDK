/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __OBSS_C_STYLE_H__
#define __OBSS_C_STYLE_H__


#include "comm_utils.h"


typedef void *OBSS_ClntHandle;
typedef void *OBSS_OptHandle;


/**
* Initialize OBSS client info
* @param [in]  cloudType 	is the cloud type:
							0 --Jovision Obss, 1 --Ali Yun Oss, 2 --unknown type
* @param [in]  accessId 	is the obss access id.
* @param [in]  accessKey	is the obss access key.
* @param [in]  cloudHost	is the hostname of obss server.
* @return	Handler of OBSS Client	--success
			NULL					--failed	
**/
OBSS_ClntHandle OBSS_InitCloudClnt(int cloudType, const char* accessId, const char* accessKey, const char* cloudHost);


/**
* Finalize OBSS client info
* @param [in]  cloudType 	is the pointer of OBSS client handler.
* @return	void	
**/
void OBSS_FinalCloudClnt(OBSS_ClntHandle* pCloudClient);


/**
* Initialize OBSS client info
* @param [in]  	cloudType 	is handler of OBSS client.
* @param [in]	bucket		is the bucket name which is valid on OBSS.
* @param [in]  	devSN		is the combination of Device-Group and YSTNO.
* @return	Handler of OBSS operator	--success
			NULL						--failed	
**/
OBSS_OptHandle OBSS_InitCloudOpt(OBSS_ClntHandle cloudClient, const char* bucket, const char* devSN);


/**
* Finalize OBSS client info
* @param [in]  cloudType 	is the pointer of OBSS operator handler.
* @return	void	
**/
void OBSS_FinalCloudOpt(OBSS_OptHandle* pCloudOpt);


/**
* Upload data from local file to OBSS
* @param [in] cloudOpt			is cloud storage operator handler.
* @param [in] year,month,day	is date of the uploading.
* @param [in] name				is the object name that used on OBSS.
* @param [in] fileName			is the name of local file.
* @param [out] billingSize		is the length of local file to be used for billing.
* @return	 0		success
			-1		internal error
			400 	Bad Request
			4001	Invalid Object Name
			404 	Not Found
			4041	No Such Bucket
			403 	Forbidden
			4031	Signature Does Not Match
			4032	Request Time Too Skewed (over 15mins)	**/
int OBSS_UploadObjFromFile(OBSS_OptHandle cloudOpt, 
								const int year, const int month, const int day, const char* name, const char *localFile, size_t* billingSize);


/** 
* Initialize for uploading data from stream to OBSS
* @param [in] cloudOpt			is cloud storage operator handler.
* @param [in] year,month,day	is date of the uploading.
* @param [in] name				is the object name that used on OBSS.
* @param [in] streamLen 		is the totle length of data stream.
* @return	 0		success
			-1		internal error	
**/
int OBSS_UploadObjFromStream_Init(OBSS_OptHandle cloudOpt,	
										const int year, const int month, const int day, const char* name, const size_t streamLen);


/** 
* Send one part of stream to OBSS
* @param [in] cloudOpt		is cloud storage operator handler.
* @param [in] subStream 	is one part of totle stream.
* @param [in] subStreamLen 	is the length of sub-stream.
* @return	 0		success
			-1		internal error	
**/
int OBSS_UploadObjFromStream_Send(OBSS_OptHandle cloudOpt, const char* subStream, const size_t subStreamLen);


/** 
* Finish for uploading data from stream to OBSS
* @param [in]  cloudOpt		is cloud storage operator handler.
* @return	 0		success
			-1		internal error
			400 	Bad Request
			4001	Invalid Object Name
			404 	Not Found
			4041	No Such Bucket
			403 	Forbidden
			4031	Signature Does Not Match
			4032	Request Time Too Skewed (over 15mins)	
**/
int OBSS_UploadObjFromStream_Finish(OBSS_OptHandle cloudOpt);


/**
* Delete all objects under the specified directory 
* @param [in] cloudOpt			is cloud storage operator handler.
* @param [in] year,month,day	is the date of directory that should be deleted.
* @return	 0		success
			-1		internal error
			400 	Bad Request
			4001	Invalid Object Name
			404 	Not Found
			4041	No Such Bucket
			4042	No Such Key (Object)
			403 	Forbidden
			4031	Signature Does Not Match
			4032	Request Time Too Skewed (over 15mins)	
**/
int OBSS_DeleteDirByDate(OBSS_OptHandle cloudOpt, const int year, const int month, const int day);


/**
* Reset bucket name in OBSS operator
* @param [in]  cloudOpt		is cloud storage operator handler.
* @param [in]  bucket		is the bucket name which is available on OBSS, 
* @return	 0		success
			-1		failed	
**/
int OBSS_ResetBucket(OBSS_OptHandle cloudOpt, const char* bucket);



#endif /* __OBSS_C_STYLE_H__ */

