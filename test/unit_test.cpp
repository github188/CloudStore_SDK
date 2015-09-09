/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#include "gtest/gtest.h"
#include "test.h"


#define C_TEST_YSTNO	"S123456789"
#define C_TEST_YEAR		(2015)
#define C_TEST_MONTH	(1)
#define C_TEST_DAY		(1)


/**********************************
* Tests of Ali Oss				  *
***********************************/
OBSS_Client clientAli;

TEST(ALI_TEST, initial)
{
	int ret = RET_OK;
	ret = clientAli.initial(OBSS_ALI, ALI_TEST_ACCESS_ID, ALI_TEST_ACCESS_KEY, ALI_TEST_HOST_NAME, DEFAULT_OBSS_PORT, DEFAULT_AGANT);
	EXPECT_EQ(RET_OK, ret);
	
	EXPECT_EQ(OBSS_ALI, clientAli.ObssType);
	EXPECT_STREQ(ALI_TEST_HOST_NAME, clientAli.RemoteHost);
	EXPECT_EQ(DEFAULT_OBSS_PORT, clientAli.HttpPort);
	EXPECT_STREQ(DEFAULT_AGANT, clientAli.UserAgent);
	EXPECT_STREQ(ALI_TEST_ACCESS_ID, clientAli.AccessId);
	EXPECT_STREQ(ALI_TEST_ACCESS_KEY, clientAli.AccessKey);
}

TEST(ALI_TEST, headObjInfo)
{
	OBSS_Test operation(&clientAli);
	EXPECT_EQ(RET_OK, operation.headObjInfo(ALI_TEST_BUCKET, "Ali_OSS_API.pdf"));
	EXPECT_EQ(404, operation.headObjInfo(ALI_TEST_BUCKET, "Object_Not_Existing"));
	EXPECT_EQ(404, operation.headObjInfo("no-bucket", "Object_Existing"));
	EXPECT_EQ(400, operation.headObjInfo("Invalid_Bucket", "Object_Existing"));
}

TEST(ALI_TEST, putObjFromFile)
{
	OBSS_Test operation(&clientAli);
	size_t billingSize = 0;
	EXPECT_EQ(RET_OK, operation.putObjFromFile(ALI_TEST_BUCKET, "test/file/ut.out", "ut.out", &billingSize));
	EXPECT_EQ(RET_OK, operation.putObjFromFile(ALI_TEST_BUCKET, "test/file/libobsssdk.a", "libobsssdk.a"));
	EXPECT_EQ(4041, operation.putObjFromFile("no-bucket", "test/file/libobsssdk.a", "libobsssdk.a"));
	EXPECT_EQ(400, operation.putObjFromFile("Invalid_Bucket", "test/file/libobsssdk.a", "libobsssdk.a"));
	EXPECT_EQ(RET_ERROR, operation.putObjFromFile(ALI_TEST_BUCKET, "test/file/libobsssdk.a", "NO_libobsssdk.a"));
}

TEST(ALI_TEST, putObjFromStream)
{
	OBSS_Test operation(&clientAli);
	
	char buff_A[1024] = {0};
	memset(buff_A, 'A', 1024);
	char buff_B[700*1024] = {0};
	memset(buff_B, 'B', 700*1024);
	char buff_C[1800*1024] = {0};
	memset(buff_C, 'C', 1800*1024);
	size_t piece_len = 188;
	size_t send_len = 0;

	EXPECT_EQ(RET_OK, operation.putObjFromStream_Init(ALI_TEST_BUCKET, "test/stream/A_1024", sizeof(buff_A)));
	send_len = 0;
	while(send_len < sizeof(buff_A))
	{
		EXPECT_EQ(RET_OK, operation.putObjFromStream_Send(buff_A + send_len, piece_len));
		send_len += piece_len;
	}
	EXPECT_EQ(RET_OK, operation.putObjFromStream_Finish());

	EXPECT_EQ(RET_OK, operation.putObjFromStream_Init(ALI_TEST_BUCKET, "test/stream/B_700K", sizeof(buff_B)));
	send_len = 0;
	while(send_len < sizeof(buff_B))
	{
		EXPECT_EQ(RET_OK, operation.putObjFromStream_Send(buff_B + send_len, piece_len));
		send_len += piece_len;
	}
	EXPECT_EQ(RET_OK, operation.putObjFromStream_Finish());

	EXPECT_EQ(RET_OK, operation.putObjFromStream_Init(ALI_TEST_BUCKET, "test/stream/C_1800K", sizeof(buff_C)));
	send_len = 0;
	while(send_len < sizeof(buff_C))
	{
		EXPECT_EQ(RET_OK, operation.putObjFromStream_Send(buff_C + send_len, piece_len));
		send_len += piece_len;
	}
	EXPECT_EQ(RET_OK, operation.putObjFromStream_Finish());
}

TEST(ALI_TEST, putObjFromBuffer)
{
	OBSS_Test operation(&clientAli);

	char buff_A[1024] = {0};
	memset(buff_A, 'A', 1024);
	EXPECT_EQ(RET_OK, operation.putObjFromBuffer(ALI_TEST_BUCKET, "test/buff/A_1024", buff_A, sizeof(buff_A)));

	char buff_B[700*1024] = {0};
	memset(buff_B, 'B', 700*1024);
	EXPECT_EQ(RET_OK, operation.putObjFromBuffer(ALI_TEST_BUCKET, "test/buff/B_700K", buff_B, sizeof(buff_B)));

	char buff_C[1800*1024] = {0};
	memset(buff_C, 'C', 1800*1024);
	EXPECT_EQ(RET_OK, operation.putObjFromBuffer(ALI_TEST_BUCKET, "test/buff/C_1800K", buff_C, sizeof(buff_C)));
}

TEST(ALI_TEST, appendObjFromFile)
{
	OBSS_Test operation(&clientAli);

	size_t position = 0;
	EXPECT_EQ(RET_OK, operation.appendObjFromFile(ALI_TEST_BUCKET, "test/file/Append_test", "ut.out", position));
	EXPECT_EQ(RET_OK, operation.appendObjFromFile(ALI_TEST_BUCKET, "test/file/Append_test", "libobsssdk.a", position));
	EXPECT_EQ(position, fileGetFileSize("ut.out") + fileGetFileSize("libobsssdk.a"));

	position = 0;
	EXPECT_EQ(4091, operation.appendObjFromFile(ALI_TEST_BUCKET, "test/file/Append_test", "ut.out", position));
	EXPECT_EQ(4092, operation.appendObjFromFile(ALI_TEST_BUCKET, "test/file/ut.out", "ut.out", position));
}

TEST(ALI_TEST, appendObjFromBuffer)
{
	OBSS_Test operation(&clientAli);
	
	size_t position = 0;

	char buff_A[1024] = {0};
	memset(buff_A, 'A', sizeof(buff_A));
	EXPECT_EQ(RET_OK, operation.appendObjFromBuffer(ALI_TEST_BUCKET, "test/buff/Append_ABC", position, buff_A, sizeof(buff_A)));

	char buff_B[1024] = {0};
	memset(buff_B, 'B', sizeof(buff_B));
	EXPECT_EQ(RET_OK, operation.appendObjFromBuffer(ALI_TEST_BUCKET, "test/buff/Append_ABC", position, buff_B, sizeof(buff_B)));

	char buff_C[1024] = {0};
	memset(buff_C, 'C', sizeof(buff_C));
	EXPECT_EQ(RET_OK, operation.appendObjFromBuffer(ALI_TEST_BUCKET, "test/buff/Append_ABC", position, buff_C, sizeof(buff_C)));
}

TEST(ALI_TEST, genSignUrl)
{
	OBSS_Test operation(&clientAli);
	char URL[2048] = {0};
	EXPECT_EQ(RET_OK, operation.genSignUrl(URL, "GET", ALI_TEST_BUCKET, "Ali_OSS_API.pdf", 300));
	printf("[URL]: %s\n", URL);
}

TEST(ALI_TEST, getObj2File)
{
	OBSS_Test operation(&clientAli);
	
	EXPECT_EQ(RET_OK, operation.getObj2File(ALI_TEST_BUCKET, "test/stream/C_1800K", "get.file.100K.C_1800K_FULL"));
	EXPECT_EQ(RET_OK, operation.getObj2File(ALI_TEST_BUCKET, "test/stream/C_1800K", "get.file.100K.C_1800K_1", 1000000, 0));
	EXPECT_EQ(RET_OK, operation.getObj2File(ALI_TEST_BUCKET, "test/stream/C_1800K", "get.file.100K.C_1800K_2", 800000, 1000000));
	EXPECT_EQ(RET_OK, operation.getObj2File(ALI_TEST_BUCKET, "test/stream/C_1800K", "get.file.100K.C_1800K_1'", 1000000));
	EXPECT_EQ(RET_OK, operation.getObj2File(ALI_TEST_BUCKET, "test/stream/C_1800K", "get.file.1000K.C_1800K_1", 10000000));
	EXPECT_EQ(4042, operation.getObj2File(ALI_TEST_BUCKET, "test/stream/NO_C_1800K", "get.file.100K.C_1800K_FULL_E"));
	EXPECT_EQ(4041, operation.getObj2File("no-bucket", "test/stream/C_1800K", "get.file.100K.C_1800K_FULL_E"));
	EXPECT_EQ(400, operation.getObj2File("Invalid_Bucket", "test/stream/NO_C_1800K", "get.file.100K.C_1800K_FULL_E"));

}

TEST(ALI_TEST, getObj2Buff)
{
	OBSS_Test operation(&clientAli);
	
	char buff[100000] = {0};
	
	memset(buff, 0, sizeof(buff));
	EXPECT_EQ(RET_OK, operation.getObj2Buff(ALI_TEST_BUCKET, "test/stream/C_1800K", buff, sizeof(buff), 0));
	EXPECT_EQ('C', buff[100000-1]);
	
	memset(buff, 0, sizeof(buff));
	EXPECT_EQ(RET_OK, operation.getObj2Buff(ALI_TEST_BUCKET, "test/stream/C_1800K", buff, sizeof(buff)));
	EXPECT_EQ('C', buff[100000-1]);
}

TEST(ALI_TEST, listObject)
{
	OBSS_Test operation(&clientAli);
	OBSS_ListObjRslt list_rucket_rslt;

	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, ALI_TEST_BUCKET, NULL, NULL, 1000, "test/"));
	EXPECT_EQ(10, (int)list_rucket_rslt.ObjectQueue.getLen());

	list_rucket_rslt.destroy();
	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, ALI_TEST_BUCKET, NULL, NULL, 1000, "test/stream/"));
	EXPECT_EQ(3, (int)list_rucket_rslt.ObjectQueue.getLen());

	list_rucket_rslt.destroy();
	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, ALI_TEST_BUCKET, NULL, NULL, 8, "test/"));
	EXPECT_EQ(8, (int)list_rucket_rslt.ObjectQueue.getLen());

	list_rucket_rslt.destroy();
	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, ALI_TEST_BUCKET, "/", NULL, 10, "test/"));
	EXPECT_EQ(3, (int)list_rucket_rslt.ObjectQueue.getLen());

	list_rucket_rslt.destroy();
	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, ALI_TEST_BUCKET, NULL, "test/file/libobsssdk.a", 10, "test/"));
	EXPECT_EQ(4, (int)list_rucket_rslt.ObjectQueue.getLen());

	list_rucket_rslt.destroy();
	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, ALI_TEST_BUCKET, NULL, NULL, 1000, "NOT_EXIST"));
	EXPECT_EQ(0, (int)list_rucket_rslt.ObjectQueue.getLen());

}

TEST(ALI_TEST, listAllObject)
{
	OBSS_Test operation(&clientAli);
	OBSS_ListObjRslt list_rucket_rslt;

	EXPECT_EQ(RET_OK, operation.listAllObject(&list_rucket_rslt, ALI_TEST_BUCKET, NULL, NULL));
	EXPECT_EQ(11, (int)list_rucket_rslt.ObjectQueue.getLen());

}

TEST(ALI_TEST, deleteObject)
{
	OBSS_Test operation(&clientAli);

	EXPECT_EQ(RET_OK, operation.deleteObject(ALI_TEST_BUCKET, "test/file/libobsssdk.a"));
	EXPECT_EQ(RET_OK, operation.deleteObject(ALI_TEST_BUCKET, "test/file/NOT_EXIST"));
	EXPECT_EQ(4041, operation.deleteObject("no-bucket", "test/file/libobsssdk.a"));
}

TEST(ALI_TEST, deleteMultiObjects)
{
	OBSS_Test operation(&clientAli);
	OBSS_Queue objectQueue;

	(void)objectQueue.pushBack("test/stream/A_1024");
	(void)objectQueue.pushBack("test/stream/B_700K");
	(void)objectQueue.pushBack("test/stream/C_1800K");

	EXPECT_EQ(RET_OK, operation.deleteMultiObjects(ALI_TEST_BUCKET, &objectQueue, true));
}

TEST(ALI_TEST, deleteDir)
{
	OBSS_Test operation(&clientAli);

	EXPECT_EQ(RET_OK, operation.deleteDir(ALI_TEST_BUCKET, "test/"));
}

TEST(ALI_TEST, listAllObject2)
{
	OBSS_Test operation(&clientAli);
	OBSS_ListObjRslt list_rucket_rslt;

	EXPECT_EQ(RET_OK, operation.listAllObject(&list_rucket_rslt, ALI_TEST_BUCKET, NULL, "test/"));
	EXPECT_EQ(0, (int)list_rucket_rslt.ObjectQueue.getLen());
}

TEST(ALI_TEST, getObj2FileThreadSUCCESS)
{
	OBSS_Test operation(&clientAli);
	snprintf(operation.bucketName, sizeof(operation.bucketName), ALI_TEST_BUCKET);
	snprintf(operation.objectName, sizeof(operation.objectName), "Ali_OSS_API.pdf");
	snprintf(operation.fileName, sizeof(operation.fileName), "Ali_OSS_API.pdf");
	int ret = 0;

	void* pth_join1 = NULL;
	pthread_t	thread_id1;
	ret = pthread_create(&thread_id1, NULL, operation.downloadFile, (void*)&operation);
	EXPECT_EQ(0, ret);
	pthread_join(thread_id1, &pth_join1);
	EXPECT_STREQ("SUCCESS", (char*)pth_join1);
}

TEST(ALI_TEST, getObj2FileThreadFAILED)
{
	OBSS_Test operation(&clientAli);
	snprintf(operation.bucketName, sizeof(operation.bucketName), ALI_TEST_BUCKET);
	snprintf(operation.objectName, sizeof(operation.objectName), "Ali_OSS_API.pdf");
	snprintf(operation.fileName, sizeof(operation.fileName), "Ali_OSS_API.pdf");
	int ret = 0;
	
	void* pth_join2 = NULL;
	pthread_t	thread_id2;
	ret = pthread_create(&thread_id2, NULL, operation.downloadFile, (void*)&operation);
	EXPECT_EQ(0, ret);
	operation.RunHandle = false;
	pthread_join(thread_id2, &pth_join2);
	EXPECT_STREQ("FAILED", (char*)pth_join2);
}


/**********************************
* Tests of Jov Cloud			  *
***********************************/
OBSS_Client clientJov;

TEST(JOV_TEST, initial)
{
	int ret = RET_OK;
	ret = clientJov.initial(OBSS_JOVISION, JOV_TEST_ACCESS_ID, JOV_TEST_ACCESS_KEY, JOV_TEST_HOST_NAME, DEFAULT_OBSS_PORT, DEFAULT_AGANT);
	EXPECT_EQ(RET_OK, ret);
	
	EXPECT_EQ(OBSS_JOVISION, clientJov.ObssType);
	EXPECT_STREQ(JOV_TEST_HOST_NAME, clientJov.RemoteHost);
	EXPECT_EQ(DEFAULT_OBSS_PORT, clientJov.HttpPort);
	EXPECT_STREQ(DEFAULT_AGANT, clientJov.UserAgent);
	EXPECT_STREQ(JOV_TEST_ACCESS_ID, clientJov.AccessId);
	EXPECT_STREQ(JOV_TEST_ACCESS_KEY, clientJov.AccessKey);
}

TEST(JOV_TEST, headObjInfo)
{
	OBSS_Test operation(&clientJov);
	EXPECT_EQ(RET_OK, operation.headObjInfo(JOV_TEST_BUCKET, "Ali_OSS_API.pdf"));
	EXPECT_EQ(404, operation.headObjInfo(JOV_TEST_BUCKET, "Object_Not_Existing"));
	EXPECT_EQ(404, operation.headObjInfo("no-bucket", "Object_Existing"));
}

TEST(JOV_TEST, putObjFromFile)
{
	OBSS_Test operation(&clientJov);
	size_t billingSize = 0;
	EXPECT_EQ(RET_OK, operation.putObjFromFile(JOV_TEST_BUCKET, "test/file/ut.out", "ut.out", &billingSize));
	EXPECT_EQ(RET_OK, operation.putObjFromFile(JOV_TEST_BUCKET, "test/file/libobsssdk.a", "libobsssdk.a"));
	EXPECT_EQ(4041, operation.putObjFromFile("no-bucket", "test/file/libobsssdk.a", "libobsssdk.a"));
	EXPECT_EQ(RET_ERROR, operation.putObjFromFile(JOV_TEST_BUCKET, "test/file/libobsssdk.a", "NO_libobsssdk.a"));
}

TEST(JOV_TEST, putObjFromStream)
{
	OBSS_Test operation(&clientJov);
	
	char buff_A[1024] = {0};
	memset(buff_A, 'A', 1024);
	char buff_B[700*1024] = {0};
	memset(buff_B, 'B', 700*1024);
	char buff_C[1800*1024] = {0};
	memset(buff_C, 'C', 1800*1024);
	size_t piece_lem = 188;
	size_t send_len = 0;

	EXPECT_EQ(RET_OK, operation.putObjFromStream_Init(JOV_TEST_BUCKET, "test/stream/A_1024", sizeof(buff_A)));
	send_len = 0;
	while(send_len < sizeof(buff_A))
	{
		EXPECT_EQ(RET_OK, operation.putObjFromStream_Send(buff_A + send_len, piece_lem));
		send_len += piece_lem;
	}
	EXPECT_EQ(RET_OK, operation.putObjFromStream_Finish());

	EXPECT_EQ(RET_OK, operation.putObjFromStream_Init(JOV_TEST_BUCKET, "test/stream/B_700K", sizeof(buff_B)));
	send_len = 0;
	while(send_len < sizeof(buff_B))
	{
		EXPECT_EQ(RET_OK, operation.putObjFromStream_Send(buff_B + send_len, piece_lem));
		send_len += piece_lem;
	}
	EXPECT_EQ(RET_OK, operation.putObjFromStream_Finish());

	EXPECT_EQ(RET_OK, operation.putObjFromStream_Init(JOV_TEST_BUCKET, "test/stream/C_1800K", sizeof(buff_C)));
	send_len = 0;
	while(send_len < sizeof(buff_C))
	{
		EXPECT_EQ(RET_OK, operation.putObjFromStream_Send(buff_C + send_len, piece_lem));
		send_len += piece_lem;
	}
	EXPECT_EQ(RET_OK, operation.putObjFromStream_Finish());
}

TEST(JOV_TEST, putObjFromBuffer)
{
	OBSS_Test operation(&clientJov);

	char buff_A[1024] = {0};
	memset(buff_A, 'A', 1024);
	EXPECT_EQ(RET_OK, operation.putObjFromBuffer(JOV_TEST_BUCKET, "test/buff/A_1024", buff_A, sizeof(buff_A)));

	char buff_B[700*1024] = {0};
	memset(buff_B, 'B', 700*1024);
	EXPECT_EQ(RET_OK, operation.putObjFromBuffer(JOV_TEST_BUCKET, "test/buff/B_700K", buff_B, sizeof(buff_B)));

	char buff_C[1800*1024] = {0};
	memset(buff_C, 'C', 1800*1024);
	EXPECT_EQ(RET_OK, operation.putObjFromBuffer(JOV_TEST_BUCKET, "test/buff/C_1800K", buff_C, sizeof(buff_C)));
}

TEST(JOV_TEST, genSignUrl)
{
	OBSS_Test operation(&clientJov);
	char URL[2048] = {0};
	EXPECT_EQ(RET_OK, operation.genSignUrl(URL, "GET", JOV_TEST_BUCKET, "Ali_OSS_API.pdf", 300));
	printf("[URL]: %s\n", URL);
}

TEST(JOV_TEST, getObj2File)
{
	OBSS_Test operation(&clientJov);
	
	EXPECT_EQ(RET_OK, operation.getObj2File(JOV_TEST_BUCKET, "test/stream/C_1800K", "get.file.100K.C_1800K_FULL"));
	EXPECT_EQ(RET_OK, operation.getObj2File(JOV_TEST_BUCKET, "test/stream/C_1800K", "get.file.100K.C_1800K_1", 1000000, 0));
	EXPECT_EQ(RET_OK, operation.getObj2File(JOV_TEST_BUCKET, "test/stream/C_1800K", "get.file.100K.C_1800K_2", 800000, 1000000));
	EXPECT_EQ(RET_OK, operation.getObj2File(JOV_TEST_BUCKET, "test/stream/C_1800K", "get.file.100K.C_1800K_1'", 1000000));
	EXPECT_EQ(RET_OK, operation.getObj2File(JOV_TEST_BUCKET, "test/stream/C_1800K", "get.file.1000K.C_1800K_1", 10000000));
	EXPECT_EQ(4042, operation.getObj2File(JOV_TEST_BUCKET, "test/stream/NO_C_1800K", "get.file.100K.C_1800K_FULL_E"));
	EXPECT_EQ(4041, operation.getObj2File("no-bucket", "test/stream/C_1800K", "get.file.100K.C_1800K_FULL_E"));
}

TEST(JOV_TEST, getObj2Buff)
{
	OBSS_Test operation(&clientJov);
	
	char buff[100000] = {0};
	
	memset(buff, 0, sizeof(buff));
	EXPECT_EQ(RET_OK, operation.getObj2Buff(JOV_TEST_BUCKET, "test/stream/C_1800K", buff, sizeof(buff), 0));
	EXPECT_EQ('C', buff[100000-1]);
	
	memset(buff, 0, sizeof(buff));
	EXPECT_EQ(RET_OK, operation.getObj2Buff(JOV_TEST_BUCKET, "test/stream/C_1800K", buff, sizeof(buff)));
	EXPECT_EQ('C', buff[100000-1]);
}

TEST(JOV_TEST, listObject)
{
	OBSS_Test operation(&clientJov);
	OBSS_ListObjRslt list_rucket_rslt;

	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, JOV_TEST_BUCKET, NULL, NULL, 1000, "test/"));
	EXPECT_EQ(8, (int)list_rucket_rslt.ObjectQueue.getLen());

	list_rucket_rslt.destroy();
	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, JOV_TEST_BUCKET, NULL, NULL, 1000, "test/stream/"));
	EXPECT_EQ(3, (int)list_rucket_rslt.ObjectQueue.getLen());

	list_rucket_rslt.destroy();
	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, JOV_TEST_BUCKET, NULL, NULL, 8, "test/"));
	EXPECT_EQ(8, (int)list_rucket_rslt.ObjectQueue.getLen());

	list_rucket_rslt.destroy();
	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, JOV_TEST_BUCKET, "/", NULL, 10, "test/"));
	EXPECT_EQ(3, (int)list_rucket_rslt.ObjectQueue.getLen());

	list_rucket_rslt.destroy();
	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, JOV_TEST_BUCKET, NULL, "test/file/libobsssdk.a", 10, "test/"));
	EXPECT_EQ(4, (int)list_rucket_rslt.ObjectQueue.getLen());

	list_rucket_rslt.destroy();
	EXPECT_EQ(RET_OK, operation.listObject(&list_rucket_rslt, JOV_TEST_BUCKET, NULL, NULL, 1000, "NOT_EXIST"));
	EXPECT_EQ(0, (int)list_rucket_rslt.ObjectQueue.getLen());

}

TEST(JOV_TEST, listAllObject)
{
	OBSS_Test operation(&clientJov);
	OBSS_ListObjRslt list_rucket_rslt;

	EXPECT_EQ(RET_OK, operation.listAllObject(&list_rucket_rslt, JOV_TEST_BUCKET, NULL, NULL));
	EXPECT_EQ(9, (int)list_rucket_rslt.ObjectQueue.getLen());

}

TEST(JOV_TEST, deleteObject)
{
	OBSS_Test operation(&clientJov);

	EXPECT_EQ(RET_OK, operation.deleteObject(JOV_TEST_BUCKET, "test/file/libobsssdk.a"));
	EXPECT_EQ(RET_OK, operation.deleteObject(JOV_TEST_BUCKET, "test/file/NOT_EXIST"));
	EXPECT_EQ(4041, operation.deleteObject("no-bucket", "test/file/libobsssdk.a"));
}

TEST(JOV_TEST, deleteMultiObjects)
{
	OBSS_Test operation(&clientJov);
	OBSS_Queue objectQueue;

	(void)objectQueue.pushBack("test/stream/A_1024");
	(void)objectQueue.pushBack("test/stream/B_700K");
	(void)objectQueue.pushBack("test/stream/C_1800K");

	EXPECT_EQ(RET_OK, operation.deleteMultiObjects(JOV_TEST_BUCKET, &objectQueue, true));
}

TEST(JOV_TEST, deleteDir)
{
	OBSS_Test operation(&clientJov);

	EXPECT_EQ(RET_OK, operation.deleteDir(JOV_TEST_BUCKET, "test/"));
}

TEST(JOV_TEST, listAllObject2)
{
	OBSS_Test operation(&clientJov);
	OBSS_ListObjRslt list_rucket_rslt;

	EXPECT_EQ(RET_OK, operation.listAllObject(&list_rucket_rslt, JOV_TEST_BUCKET, NULL, "test/"));
	EXPECT_EQ(0, (int)list_rucket_rslt.ObjectQueue.getLen());
}

TEST(JOV_TEST, getObj2FileThreadSUCCESS)
{
	OBSS_Test operation(&clientJov);
	snprintf(operation.bucketName, sizeof(operation.bucketName), JOV_TEST_BUCKET);
	snprintf(operation.objectName, sizeof(operation.objectName), "Ali_OSS_API.pdf");
	snprintf(operation.fileName, sizeof(operation.fileName), "Ali_OSS_API.pdf");
	int ret = 0;

	void* pth_join1 = NULL;
	pthread_t	thread_id1;
	ret = pthread_create(&thread_id1, NULL, operation.downloadFile, (void*)&operation);
	EXPECT_EQ(0, ret);
	pthread_join(thread_id1, &pth_join1);
	EXPECT_STREQ("SUCCESS", (char*)pth_join1);
}

TEST(JOV_TEST, getObj2FileThreadFAILED)
{
	OBSS_Test operation(&clientJov);
	snprintf(operation.bucketName, sizeof(operation.bucketName), JOV_TEST_BUCKET);
	snprintf(operation.objectName, sizeof(operation.objectName), "Ali_OSS_API.pdf");
	snprintf(operation.fileName, sizeof(operation.fileName), "Ali_OSS_API.pdf");
	int ret = 0;
	
	void* pth_join2 = NULL;
	pthread_t	thread_id2;
	ret = pthread_create(&thread_id2, NULL, operation.downloadFile, (void*)&operation);
	EXPECT_EQ(0, ret);
	operation.RunHandle = false;
	pthread_join(thread_id2, &pth_join2);
	EXPECT_STREQ("FAILED", (char*)pth_join2);
}

/**********************************
* Tests of Ali C style api	      *
***********************************/
OBSS_ClntHandle clientAli_C = NULL;
OBSS_OptHandle operatorAli_C = NULL;

TEST(ALI_C_TEST, InitCloudClnt)
{
	clientAli_C = OBSS_InitCloudClnt(OBSS_ALI, ALI_TEST_ACCESS_ID, ALI_TEST_ACCESS_KEY, ALI_TEST_HOST_NAME);
	if (!clientAli_C)
	{
		FAIL();
	}

	OBSS_Client* obssClient = (OBSS_Client*)clientAli_C;
	EXPECT_EQ(OBSS_ALI, obssClient->ObssType);
	EXPECT_STREQ(ALI_TEST_ACCESS_ID, obssClient->AccessId);
	EXPECT_STREQ(ALI_TEST_ACCESS_KEY, obssClient->AccessKey);
	EXPECT_STREQ(ALI_TEST_HOST_NAME, obssClient->RemoteHost);
}

TEST(ALI_C_TEST, InitCloudOpt)
{
	operatorAli_C = OBSS_InitCloudOpt(clientAli_C, ALI_TEST_BUCKET, C_TEST_YSTNO);
	if (!operatorAli_C)
	{
		FAIL();
	}
}

TEST(ALI_C_TEST, UploadObjFromFile)
{
	size_t billingSize = 0;
	EXPECT_EQ(RET_OK, OBSS_UploadObjFromFile(operatorAli_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "ut.out", "ut.out", &billingSize));
	EXPECT_EQ(RET_OK, OBSS_UploadObjFromFile(operatorAli_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "libobsssdk.a", "libobsssdk.a", &billingSize));
	EXPECT_EQ(RET_ERROR, OBSS_UploadObjFromFile(operatorAli_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "NO_libobsssdk.a", "NO_libobsssdk.a", &billingSize));
}

TEST(ALI_C_TEST, UploadObjFromStream)
{
	char buff_A[1024] = {0};
	memset(buff_A, 'A', 1024);
	char buff_B[700*1024] = {0};
	memset(buff_B, 'B', 700*1024);
	char buff_C[1800*1024] = {0};
	memset(buff_C, 'C', 1800*1024);
	size_t piece_len = 188;
	size_t send_len = 0;

	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Init(operatorAli_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "A_1024", sizeof(buff_A)));
	send_len = 0;
	while(send_len < sizeof(buff_A))
	{
		EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Send(operatorAli_C, buff_A + send_len, piece_len));
		send_len += piece_len;
	}
	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Finish(operatorAli_C));

	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Init(operatorAli_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "B_700K", sizeof(buff_B)));
	send_len = 0;
	while(send_len < sizeof(buff_B))
	{
		EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Send(operatorAli_C, buff_B + send_len, piece_len));
		send_len += piece_len;
	}
	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Finish(operatorAli_C));

	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Init(operatorAli_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "C_1800K", sizeof(buff_C)));
	send_len = 0;
	while(send_len < sizeof(buff_C))
	{
		EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Send(operatorAli_C, buff_C + send_len, piece_len));
		send_len += piece_len;
	}
	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Finish(operatorAli_C));
}

TEST(ALI_C_TEST, DeleteDirByDate)
{
	EXPECT_EQ(RET_OK, OBSS_DeleteDirByDate(operatorAli_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY));
}

TEST(ALI_C_TEST, ResetBucket)
{
	EXPECT_EQ(RET_OK, OBSS_ResetBucket(operatorAli_C, "testbucket"));
}

TEST(ALI_C_TEST, FinalCloudOpt)
{
	OBSS_FinalCloudOpt(&operatorAli_C);
	if (operatorAli_C)
	{
		FAIL();
	}
}

TEST(ALI_C_TEST, FinalCloudClnt)
{
	OBSS_FinalCloudClnt(&clientAli_C);
	if (clientAli_C)
	{
		FAIL();
	}
}


/**********************************
* Tests of Jov C style api	      *
***********************************/
OBSS_ClntHandle clientJov_C = NULL;
OBSS_OptHandle operatorJov_C = NULL;

TEST(JOV_C_TEST, InitCloudClnt)
{
	clientJov_C = OBSS_InitCloudClnt(OBSS_JOVISION, JOV_TEST_ACCESS_ID, JOV_TEST_ACCESS_KEY, JOV_TEST_HOST_NAME);
	if (!clientJov_C)
	{
		FAIL();
	}

	OBSS_Client* obssClient = (OBSS_Client*)clientJov_C;
	EXPECT_EQ(OBSS_JOVISION, obssClient->ObssType);
	EXPECT_STREQ(JOV_TEST_ACCESS_ID, obssClient->AccessId);
	EXPECT_STREQ(JOV_TEST_ACCESS_KEY, obssClient->AccessKey);
	EXPECT_STREQ(JOV_TEST_HOST_NAME, obssClient->RemoteHost);
}

TEST(JOV_C_TEST, InitCloudOpt)
{
	operatorJov_C = OBSS_InitCloudOpt(clientJov_C, JOV_TEST_BUCKET, C_TEST_YSTNO);
	if (!operatorJov_C)
	{
		FAIL();
	}
}

TEST(JOV_C_TEST, UploadObjFromFile)
{
	size_t billingSize = 0;
	EXPECT_EQ(RET_OK, OBSS_UploadObjFromFile(operatorJov_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "ut.out", "ut.out", &billingSize));
	EXPECT_EQ(RET_OK, OBSS_UploadObjFromFile(operatorJov_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "libobsssdk.a", "libobsssdk.a", &billingSize));
	EXPECT_EQ(RET_ERROR, OBSS_UploadObjFromFile(operatorJov_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "NO_libobsssdk.a", "NO_libobsssdk.a", &billingSize));
}

TEST(JOV_C_TEST, UploadObjFromStream)
{
	char buff_A[1024] = {0};
	memset(buff_A, 'A', 1024);
	char buff_B[700*1024] = {0};
	memset(buff_B, 'B', 700*1024);
	char buff_C[1800*1024] = {0};
	memset(buff_C, 'C', 1800*1024);
	size_t piece_len = 188;
	size_t send_len = 0;

	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Init(operatorJov_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "A_1024", sizeof(buff_A)));
	send_len = 0;
	while(send_len < sizeof(buff_A))
	{
		EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Send(operatorJov_C, buff_A + send_len, piece_len));
		send_len += piece_len;
	}
	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Finish(operatorJov_C));

	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Init(operatorJov_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "B_700K", sizeof(buff_B)));
	send_len = 0;
	while(send_len < sizeof(buff_B))
	{
		EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Send(operatorJov_C, buff_B + send_len, piece_len));
		send_len += piece_len;
	}
	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Finish(operatorJov_C));

	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Init(operatorJov_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, "C_1800K", sizeof(buff_C)));
	send_len = 0;
	while(send_len < sizeof(buff_C))
	{
		EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Send(operatorJov_C, buff_C + send_len, piece_len));
		send_len += piece_len;
	}
	EXPECT_EQ(RET_OK, OBSS_UploadObjFromStream_Finish(operatorJov_C));
}

TEST(JOV_C_TEST, DeleteDirByDate)
{
	EXPECT_EQ(RET_OK, OBSS_DeleteDirByDate(operatorJov_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY));
}

TEST(JOV_C_TEST, ResetBucket)
{
	EXPECT_EQ(RET_OK, OBSS_ResetBucket(operatorJov_C, "testbucket"));
}

TEST(JOV_C_TEST, FinalCloudOpt)
{
	OBSS_FinalCloudOpt(&operatorJov_C);
	if (operatorJov_C)
	{
		FAIL();
	}
}

TEST(JOV_C_TEST, FinalCloudClnt)
{
	OBSS_FinalCloudClnt(&clientJov_C);
	if (clientJov_C)
	{
		FAIL();
	}
}


