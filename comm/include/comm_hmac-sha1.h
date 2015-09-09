/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#ifndef __OBSS_HAMC_SHA1_H__
#define __OBSS_HAMC_SHA1_H__


/*
 * Compute HMAC-SHA-1 with key [key] and message [message], storing result in [hmac]
 */	
void HMAC_SHA1(unsigned char hmac[20], const unsigned char *key, unsigned int key_len,
				   const unsigned char *message, unsigned int message_len);


#endif /* __OBSS_HAMC_SHA1_H__ */
