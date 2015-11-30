###############################################################################
#
# mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*- 
#
# OBSS_SDK - Restful C++ API for Object-Based Storage System
#
# Copyright (C) 2015-2020 Albert AN <heroanxiaobo@163.com>
#
###############################################################################

.EXPORT_ALL_VARIABLES:

CC = g++
#CC = arm-hisiv100nptl-linux-c++

DEBUG = -g

INCLUDE = -I../include -I../comm/include

GCOVR = -fprofile-arcs -ftest-coverage

CFLAGS = -Wall -O0 $(DEBUG) $(INCLUDE) -fPIC
#CFLAGS += -D __OBSS_TRACE -D __OBSS_CHECK
#CFLAGS += -D __OBSS_DEBUG

LIBS = 

IPC_LIB_NAME = libcldipc
IPC_OBJS = 	../comm/src/comm_base64.o \
			../comm/src/comm_hmac-sha1.o \
			../comm/src/comm_md5.o \
			../comm/src/comm_time.o \
			../comm/src/comm_xml.o \
			../comm/src/comm_net.o \
			../comm/src/comm_utils.o \
			../comm/src/comm_data.o \
			../comm/src/comm_auth.o \
			../comm/src/comm_tcptrans.o \
			../comm/src/comm_udptrans.o \
			../comm/src/comm_http.o \
			../src/obss_client.o \
			../src/obss_operation.o \
			../src/obss_c-wrapper.o	\
			../src/jvn_flux.o
IPC_CPPS = $(IPC_OBJS:%.o=%.cpp)

APP_LIB_NAME = libcldapp
APP_OBJS = 	../comm/src/comm_base64.o \
			../comm/src/comm_hmac-sha1.o \
			../comm/src/comm_md5.o \
			../comm/src/comm_time.o \
			../comm/src/comm_xml.o \
			../comm/src/comm_json.o \
			../comm/src/comm_net.o \
			../comm/src/comm_utils.o \
			../comm/src/comm_data.o \
			../comm/src/comm_auth.o \
			../comm/src/comm_tcptrans.o \
			../comm/src/comm_udptrans.o \
			../comm/src/comm_http.o \
			../src/obss_client.o \
			../src/obss_operation.o \
			../src/obss_c-wrapper.o	\
			../src/jvn_flux.o	\
			../src/cloud_control.o	\
			../src/cloud_billing.o

APP_CPPS = $(APP_OBJS:%.o=%.cpp)

all: demo_obss unitest

unitest: ut_obss ut_ctrl

ut_obss: ipc_sdk
	$(CC) $(CFLAGS) $(GCOVR) -o $@.out ut_obss.cpp $(IPC_CPPS) -I./ -I/opt/gtest/include -L/opt/gtest/lib -lgtest -lgtest_main -lpthread

ut_ctrl: app_sdk
	$(CC) $(CFLAGS) $(GCOVR) -o $@.out ut_ctrl.cpp $(APP_CPPS) -I./ -I/opt/gtest/include -L/opt/gtest/lib -lgtest -lgtest_main -lpthread

demo_obss: app_sdk
	$(CC) $(CFLAGS) -o $@.out  demo_obss.cpp $(APP_LIB_NAME).a -lpthread

ipc_sdk: $(IPC_OBJS)
	ar rcs $(IPC_LIB_NAME).a $(IPC_OBJS) 
	$(CC) -shared -fPIC $(IPC_OBJS) -o $(IPC_LIB_NAME).so 

app_sdk: $(APP_OBJS)
	ar rcs $(APP_LIB_NAME).a $(APP_OBJS) 
	$(CC) -shared -fPIC $(APP_OBJS) -o $(APP_LIB_NAME).so 
	
%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f ../src/*.o ../src/*.gcno ../src/*.gcda ../comm/src/*.o ../comm/src/*.gcno ../comm/src/*.gcda
	rm -f *.o *.a *.so *.out *.gcno *.gcda

