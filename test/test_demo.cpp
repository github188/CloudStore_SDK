/******************************************************************************
*
* mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
*
* OBSS_SDK - Restful C++ API for Object-Based Storage System
*
* Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
*
*******************************************************************************/

#include <pthread.h>
#include "test.h"


#define __TEST_ALI

#ifdef __TEST_ALI
	#define TEST_HOST_NAME		"oss-cn-beijing.aliyuncs.com"
	#define TEST_ACCESS_ID		"4fZazqCFmQTbbmcw"
	#define TEST_ACCESS_KEY		"sBT6DgBMdCHnnaYdvK0o6O3zaN06sW"
	#define TEST_BUCKET			"jov-test"
#else
	#define TEST_HOST_NAME		"pan.cloudsee.net"
	#define TEST_ACCESS_ID		"KD3CWKRN96SUYMW9U1HI"
	#define TEST_ACCESS_KEY 	"YaIVDaJPLjTF8Ow88z1SQK3i6GT1wK0LN1Cx6IBH"
	#define TEST_BUCKET 		"jov_test"
#endif


void TEST_HeadObjInfo(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);
	
	OBSS_Test operation_demo(client);
	
	int ret = 0;
	
	ret = operation_demo.headObjInfo(TEST_BUCKET, "test_demo");
	printf("HEAD test_demo\n");
	printf("Content-Type: %s\n", operation_demo.objInfo.contentType);
	printf("Content-Length: %d\n", operation_demo.objInfo.contentLength);
	printf("x-oss-object-type: %s\n", operation_demo.objInfo.ossObjectType);
	printf("x-oss-next-append-position: %d\n", operation_demo.objInfo.ossNextAppendPos);	
	printf("headObjInfo test_demo ret = %d\n", ret);

	ret = operation_demo.headObjInfo(TEST_BUCKET, "test/file/Append_test");
	printf("HEAD test/file/Append_test\n");
	printf("Content-Type: %s\n", operation_demo.objInfo.contentType);
	printf("Content-Length: %d\n", operation_demo.objInfo.contentLength);
	printf("x-oss-object-type: %s\n", operation_demo.objInfo.ossObjectType);
	printf("x-oss-next-append-position: %d\n", operation_demo.objInfo.ossNextAppendPos);
	printf("headObjInfo test/file/Append_test ret = %d\n", ret);
	
	printf("------- %s END \n", __func__);
	return;	
}

void TEST_PutObjectFromFile(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);
	
	OBSS_Operation operation(client);
	
	int ret = 0;
	
	ret = operation.putObjFromFile(TEST_BUCKET, "test_demo", "test_demo");
	printf("putObjFromFile test_demo ret = %d\n", ret);

	ret = operation.putObjFromFile(TEST_BUCKET, "test/file/libobsssdk.a", "libobsssdk.a");
	printf("putObjFromFile libobssc.a ret = %d\n", ret);
	
	printf("------- %s END \n", __func__);
	return;	
}


int __testPutObjectFromStream(OBSS_Operation* operation, const char* objName, 
									const char* dataBuff, const size_t dataLen)
{	
	int ret = RET_OK;
	
	ret = operation->putObjFromStream_Init(TEST_BUCKET, objName, dataLen);
	if (RET_OK != ret) {
		return ret;
	}

	size_t piece_lem = 188;
	size_t send_len = 0;
	while(send_len < dataLen)
	{
		ret = operation->putObjFromStream_Send(dataBuff + send_len, piece_lem);
		if (RET_OK != ret) 
		{
			return ret;
		}
		send_len += piece_lem;
	}

	ret = operation->putObjFromStream_Finish();
	
	return ret;
}

void TEST_PutObjectFromStream(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);

	OBSS_Operation operation(client);
	
	int ret = 0;
	char buff_A[1024] = {0};
	memset(buff_A, 'A', 1024);
	char buff_B[700*1024] = {0};
	memset(buff_B, 'B', 700*1024);
	char buff_C[1800*1024] = {0};
	memset(buff_C, 'C', 1800*1024);
	
	for (int i = 0; i < 1; ++i)
	{
		ret = __testPutObjectFromStream(&operation, "test/stream/A_1024", buff_A, sizeof(buff_A));
		printf("__testPutObjectFromStream A_1024 ret = %d\n", ret);

		ret = __testPutObjectFromStream(&operation, "test/stream/B_700K", buff_B, sizeof(buff_B));
		printf("__testPutObjectFromStream B_700K ret = %d\n", ret);

		ret = __testPutObjectFromStream(&operation, "test/stream/C_1800K", buff_C, sizeof(buff_C));
		printf("__testPutObjectFromStream C_1800K ret = %d\n", ret);
	}
	
	printf("------- %s END \n", __func__);	
	return;
}

void TEST_PutObjectFromBuffer(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);

	OBSS_Operation operation(client);
	
	int ret = 0;

	char buff_A[1024] = {0};
	memset(buff_A, 'A', 1024);
	ret = operation.putObjFromBuffer(TEST_BUCKET, "test/buff/A_1024", buff_A, sizeof(buff_A));
	printf("putObjFromBuffer A_1024 ret = %d\n", ret);

	char buff_B[700*1024] = {0};
	memset(buff_B, 'B', 700*1024);
	ret = operation.putObjFromBuffer(TEST_BUCKET, "test/buff/B_700K", buff_B, sizeof(buff_B));
	printf("putObjFromBuffer B_700K ret = %d\n", ret);

	char buff_C[1800*1024] = {0};
	memset(buff_C, 'C', 1800*1024);
	ret = operation.putObjFromBuffer(TEST_BUCKET, "test/buff/C_1800K", buff_C, sizeof(buff_C));
	printf("putObjFromBuffer C_1800K ret = %d\n", ret);

	printf("------- %s END \n", __func__);	
	return;
}

void TEST_AppendObjFromFile(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);
	
	OBSS_Operation operation(client);
	
	int ret = 0;
	size_t position = 0;
	for (int i=0; i< 1; ++i){
	ret = operation.appendObjFromFile(TEST_BUCKET, "test/file/Append_test", "test_demo", position);
	printf("appendObjFromFile test_demo ret = %d\n", ret);

	ret = operation.appendObjFromFile(TEST_BUCKET, "test/file/Append_test", "libobsssdk.a", position);
	printf("appendObjFromFile libobssc.a ret = %d\n", ret);
	}
	printf("------- %s END \n", __func__);
	return;	
}

void TEST_AppendObjFromBuffer(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);

	OBSS_Operation operation(client);
	
	int ret = 0;
	size_t position = 1024;

	char buff_A[1024] = {0};
	memset(buff_A, 'A', sizeof(buff_A));
	ret = operation.appendObjFromBuffer(TEST_BUCKET, "test/buff/Append_ABC", position, buff_A, sizeof(buff_A));
	printf("appendObjFromBuffer A ret = %d\n", ret);

	char buff_B[1024] = {0};
	memset(buff_B, 'B', sizeof(buff_B));
	ret = operation.appendObjFromBuffer(TEST_BUCKET, "test/buff/Append_ABC", position, buff_B, sizeof(buff_B));
	printf("appendObjFromBuffer B ret = %d\n", ret);

	char buff_C[1024] = {0};
	memset(buff_C, 'C', sizeof(buff_C));
	ret = operation.appendObjFromBuffer(TEST_BUCKET, "test/buff/Append_ABC", position, buff_C, sizeof(buff_C));
	printf("appendObjFromBuffer C ret = %d\n", ret);

	printf("------- %s END \n", __func__);	
	return;
}


void TEST_GenSignUrl(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);
	
	OBSS_Operation operation(client);
	
	int ret = 0;
	char URL[2048] = {0};
	
	ret = operation.genSignUrl(URL, "GET", TEST_BUCKET, "test_demo", 300);

	printf("genSignUrl test_demo ret = %d\n", ret);
	
	printf("[URL]: \n%s\n", URL);

	printf("------- %s END \n", __func__);

	return;
}


void TEST_GetObject2File(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);

	OBSS_Operation operation(client);
	
	int ret = 0;
	ret = operation.getObj2File(TEST_BUCKET, "test/stream/C_1800K", "get.file.10000.C_1800K", 10000, 0);
	printf("getObj2File ret = %d\n", ret);
	
	printf("------- %s END \n", __func__);
	return;
}

void TEST_GetObject2Buff(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);

	OBSS_Operation operation(client);
	
	char buff[10000] = {0};
	int ret = 0;
	ret = operation.getObj2Buff(TEST_BUCKET, "test/stream/C_1800K", buff, sizeof(buff), 0);
	printf("getObj2Buff ret = %d\n", ret);

	FILE *fp;
	fp = fopen("get.buff.10000.C_1800K", "wb");
	CHECK(fp != NULL);

	fwrite(buff, sizeof(buff), 1, fp);
	fclose(fp);
	
	printf("------- %s END \n", __func__);	
	return;
}


void TEST_ListObject(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);

	OBSS_Operation operation(client);
	OBSS_ListObjRslt list_rucket_rslt;
	
	int ret = 0;
	ret = operation.listObject(&list_rucket_rslt, TEST_BUCKET, "/", NULL, 1000, "test/");
	printf("listObject test/ ret = %d\n\n", ret);

	printf("BucketName = %s\n",list_rucket_rslt.BucketName);
	printf("Prefix = %s\n",list_rucket_rslt.Prefix);
	printf("Marker = %s\n",list_rucket_rslt.Marker);
	printf("MaxKeys = %d\n",list_rucket_rslt.MaxKeys);
	printf("IsTruncated = %d\n",list_rucket_rslt.IsTruncated);
	printf("NextMarker = %s\n",list_rucket_rslt.NextMarker);
	
	char object[1024] = {0};
	uint32 queue_len = list_rucket_rslt.ObjectQueue.getLen();
	printf("objectQueue Lenth = %u\n", queue_len);
	for (uint32 i = 1; i <= queue_len; ++i)
	{
		memset(object, 0, sizeof(object));
		if (RET_OK == list_rucket_rslt.ObjectQueue.popFront(object,sizeof(object)))
		{
			printf("[%4u]%s\n", i, object);
		}
		else
		{
			printf("ERROR len = %u\n", list_rucket_rslt.ObjectQueue.getLen());
			break;
		}
	}
	
	printf("------- %s END \n", __func__);	
	return;
}


void __testListAllObject(OBSS_Client* client)
{
	OBSS_Operation operation(client);
	OBSS_ListObjRslt list_rucket_rslt;
	
	int ret = 0;
	ret = operation.listAllObject(&list_rucket_rslt, TEST_BUCKET, NULL, NULL);
	printf("listAllObject ret = %d\n", ret);
	
	char object[1024] = {0};
	uint32 queue_len = list_rucket_rslt.ObjectQueue.getLen();
	printf("objectQueue Lenth = %u\n", queue_len);
	for (uint32 i = 1; i <= queue_len; ++i)
	{
		memset(object, 0, sizeof(object));
		if (RET_OK == list_rucket_rslt.ObjectQueue.popFront(object,sizeof(object)))
		{
			printf("[%4u]%s\n", i, object);
		}
		else
		{
			printf("ERROR len = %u\n", list_rucket_rslt.ObjectQueue.getLen());
			break;
		}
	}
	
	return;
}


void TEST_ListAllObject(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);
	
	__testListAllObject(client);
	
	printf("------- %s END \n", __func__);	
	return;
}


void TEST_DeleteObject(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);

	OBSS_Operation operation(client);
	
	int ret = 0;
	ret = operation.deleteObject(TEST_BUCKET, "test/file/libobsssdk.a");
	printf("deleteObject libobssc.a ret = %d\n", ret);

	__testListAllObject(client);
	
	printf("------- %s END \n", __func__);
	return;
}


void TEST_DeleteMultiObjects(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);

	OBSS_Operation operation(client);
	OBSS_Queue objectQueue;
	int ret = 0;

	(void)objectQueue.pushBack("A363/2015/6/12/A363-1.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-10.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-11.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-12.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-13.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-2.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-3.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-4.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-5.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-6.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-7.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-8.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363-9.ts");
	(void)objectQueue.pushBack("A363/2015/6/12/A363.m3u8");

	ret = operation.deleteMultiObjects(TEST_BUCKET, &objectQueue, true);
	printf("deleteMultiObjects ret = %d\n", ret);
	
	__testListAllObject(client);

	printf("------- %s END \n", __func__);	
	return;
}


void TEST_DeleteDir(OBSS_Client* client)
{
	printf("\n\n------- %s START\n", __func__);

	OBSS_Operation operation(client);
	
	int ret = 0;
	ret = operation.deleteDir(TEST_BUCKET, "test/");
	printf("deleteDir test ret = %d\n", ret);
	
	__testListAllObject(client);
	
	printf("------- %s END \n", __func__);	
	return;
}


#define C_TEST_YSTNO	"S123456789"
#define C_TEST_YEAR		(2015)
#define C_TEST_MONTH	(1)
#define C_TEST_DAY		(1)
#define C_TEST_ARC		"_x86_"

char g_buff_A[1024] = {0};
char g_buff_B[700*1024] = {0};
char g_buff_C[1800*1024] = {0};

int C_UploadFromStream(OBSS_OptHandle cloudOpt, const char* name, 
							const char* dataBuff, const size_t totleLen, FILE* fpout)
{	
	int ret = RET_OK;
	size_t piece_len = 188;
	size_t send_len = 0;

	ret = OBSS_UploadObjFromStream_Init(cloudOpt, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY, name, totleLen);
	if (ret != RET_OK)
	{
		fprintf(fpout, "%s Init Failed!\n", name);
		return RET_ERROR;
	}

	while(send_len < totleLen)
	{
		ret = OBSS_UploadObjFromStream_Send(cloudOpt, dataBuff + send_len, piece_len);
		if (ret != RET_OK)
		{
			fprintf(fpout, "%s Send Failed!\n", name);
			return RET_ERROR;
		}
		send_len += piece_len;
	}
	
	ret = OBSS_UploadObjFromStream_Finish(cloudOpt);
	if (ret != RET_OK)
	{
		fprintf(fpout, "%s Finish Failed! ret=%d\n", name, ret);
		return RET_ERROR;
	}

	return RET_OK;
}


void* Thread_C_UploadFromStream(void* param)
{
	int ret = RET_OK;
	CHECK_RET(param != NULL, NULL);
	
	FILE* LogFile = NULL;
	LogFile = fopen((char*)param, "a+");
	CHECK_RET(LogFile != NULL, NULL);
	
	OBSS_ClntHandle client_C = NULL;
	OBSS_OptHandle operator_C = NULL;
	
	client_C = OBSS_InitCloudClnt(OBSS_ALI, TEST_ACCESS_ID, TEST_ACCESS_KEY, TEST_HOST_NAME);
	if (!client_C)
	{
		fprintf(LogFile, "OBSS_InitCloudClnt Failed!\n");
		fclose(LogFile);
		return NULL;
	}
	fprintf(LogFile, "OBSS_InitCloudClnt OK!\n");	

	operator_C = OBSS_InitCloudOpt(client_C, TEST_BUCKET, C_TEST_YSTNO);
	if (!operator_C)
	{
		fprintf(LogFile, "OBSS_InitCloudOpt Failed!\n");

		OBSS_FinalCloudClnt(&client_C);
		if (client_C)
			fprintf(LogFile, "OBSS_FinalCloudClnt client_C != NULL!\n");

		fclose(LogFile);
		return NULL;
	}
	fprintf(LogFile, "OBSS_InitCloudOpt OK!\n");

	int count_A = 0;
	int count_B = 0;
	int count_C = 0;
	char name_A[32] = {0};
	char name_B[32] = {0};
	char name_C[32] = {0};
	char name_std[32] = {0};
	(void)snprintf(name_A, sizeof(name_A), "A_1024"C_TEST_ARC"%s", (char*)param);
	(void)snprintf(name_B, sizeof(name_B), "B_700K"C_TEST_ARC"%s", (char*)param);
	(void)snprintf(name_C, sizeof(name_C), "C_1800K"C_TEST_ARC"%s", (char*)param);
	(void)snprintf(name_std, sizeof(name_std), C_TEST_ARC"std_%s", (char*)param);
	
	for (int i = 1; i <= 100000; ++i)
	{
		
		ret = C_UploadFromStream(operator_C, name_A, g_buff_A, sizeof(g_buff_A), LogFile);
		if (ret == RET_OK) count_A++;
		
		ret = C_UploadFromStream(operator_C, name_B, g_buff_B, sizeof(g_buff_B), LogFile);
		if (ret == RET_OK) count_B++;

		ret = C_UploadFromStream(operator_C, name_C, g_buff_C, sizeof(g_buff_C), LogFile);
		if (ret == RET_OK) count_C++;

		fprintf(LogFile, "i = %d, A = %d, B = %d, C = %d\n", i, count_A, count_B, count_C);
		(void)fflush(LogFile);
		(void)fflush(stdout);

		(void)OBSS_UploadObjFromFile(operator_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY,
				(const char*)param, (const char*)param, NULL);
		(void)OBSS_UploadObjFromFile(operator_C, C_TEST_YEAR, C_TEST_MONTH, C_TEST_DAY,
				name_std, "stdout.log", NULL);
	}


	OBSS_FinalCloudOpt(&operator_C);
	if (operator_C)
		fprintf(LogFile, "OBSS_FinalCloudOpt operator_C != NULL!\n");
	

	OBSS_FinalCloudClnt(&client_C);
	if (client_C)
		fprintf(LogFile, "OBSS_FinalCloudClnt client_C != NULL!\n");

	fclose(LogFile);
	return NULL;
}

void TEST_Thread_C_UploadFromStream()
{
	int ret = 0;
	
	memset(g_buff_A, 'A', 1024);
	memset(g_buff_B, 'B', 700*1024);
	memset(g_buff_C, 'C', 1800*1024);
	
	(void)mapStdout2LogFile("stdout.log");
		
	pthread_t	thread_id1;
	ret = pthread_create(&thread_id1, NULL, Thread_C_UploadFromStream, (void*)"1.log");
	printf("Create thread 1 ret = %d\n", ret);

	pthread_t	thread_id2;
	ret = pthread_create(&thread_id2, NULL, Thread_C_UploadFromStream, (void*)"2.log");
	printf("Create thread 2 ret = %d\n", ret);

	pthread_t	thread_id3;
	ret = pthread_create(&thread_id3, NULL, Thread_C_UploadFromStream, (void*)"3.log");
	printf("Create thread 3 ret = %d\n", ret);
	
	(void)pthread_join(thread_id1, NULL);
	(void)pthread_join(thread_id2, NULL);
	(void)pthread_join(thread_id3, NULL);

	(void)mapStdoutBack();
}

int main(int argc, char* argv[])
{
	OBSS_Client client;
	int ret = RET_OK;
#ifdef __TEST_ALI
	ret = client.initial(OBSS_ALI, TEST_ACCESS_ID, TEST_ACCESS_KEY, TEST_HOST_NAME, DEFAULT_OBSS_PORT, DEFAULT_AGANT);
#else
	ret = client.initial(OBSS_JOVISION, TEST_ACCESS_ID, TEST_ACCESS_KEY, TEST_HOST_NAME, DEFAULT_OBSS_PORT, DEFAULT_AGANT);
#endif
	CHECK_RET(ret == RET_OK,RET_ERROR);

	printf("Obss Type: %d\n",client.ObssType);
	printf("Remote Host: %s\n",client.RemoteHost);
	printf("Http Port: %d\n",client.HttpPort);
	printf("User Agent: %s\n",client.UserAgent);
	printf("Access Id: %s\n",client.AccessId);
	printf("Access Key: %s\n",client.AccessKey);

	//TEST_HeadObjInfo(&client);
	//TEST_PutObjectFromFile(&client);
	//TEST_PutObjectFromBuffer(&client);
	//TEST_PutObjectFromStream(&client);
	//TEST_AppendObjFromFile(&client);
	//TEST_AppendObjFromBuffer(&client);
	//TEST_GenSignUrl(&client);
	//TEST_GetObject2File(&client);
	//TEST_GetObject2Buff(&client);
	//TEST_ListObject(&client);
	//TEST_ListAllObject(&client);
	//TEST_DeleteObject(&client);
	//TEST_DeleteMultiObjects(&client);
	//TEST_DeleteDir(&client);
	TEST_Thread_C_UploadFromStream();
	//(void)Thread_C_UploadFromStream((void*)"demo_main.log");
	
	return 0;
}

