/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __OBSS_ERROR_H__
#define __OBSS_ERROR_H__


typedef enum _OBSS_ERROR_CODE {
	OBSS_E_OK = 0,	// Request success.
	
	OBSS_E_200_OK = 200,	// Request success with 200 OK.
	OBSS_E_206_PARTIAL_CONTENT = 206,	// Request success for getting range.
	
	OBSS_E_BED_REQUEST = 400, 			// 400 Bad Request.
	OBSS_E_INVALID_OBJECT_NAME = 4001,	// Invalid Object Name, the length of object name is over 1023.
	OBSS_E_INVALID_ARGUMENT = 4002,		// Invalid Argument, the request contains
	
	OBSS_E_FORBIDDEN = 403,					// 403 Forbidden.
	OBSS_E_SIGNATURE_DOES_NOT_MATCH = 4031,	// Signature Does Not Match,
	OBSS_E_REQUEST_TIME_TOO_SKEWED = 4032,	// Request Time Too Skewed, the time warp is over 15 min.
	
	OBSS_E_NOT_FDOUND = 404,		// 404 Not Found.
	OBSS_E_NO_SUCH_BUCKET = 4041,	// No Such Bucket, the bucekt is not existing.
	OBSS_E_NO_SUCH_KEY = 4042,		// No Such Key, the object is not existing.

	OBSS_E_METHOD_NOT_ALLOWED = 405, //405 Method Not Allowed
	
	OBSS_E_CONFLICT = 409,							// 409 Conflict
	OBSS_E_POSITION_NOT_EQUAL_TO_LENGTH = 4091,		// Position is not equal to length of existing object.
	OBSS_E_OBJECT_NOT_APPENDABLE = 4092,			// the object is not Appendable

	OBSS_E_INTERNAL = -1	// internal error.
}OBSS_ERROR_CODE;


#endif /* __OBSS_ERROR_H__ */

