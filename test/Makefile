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


DEBUG = -g

INCLUDE = -I../include -I../instances

GCOVR = -fprofile-arcs -ftest-coverage

CFLAGS = -Wall -O0 $(DEBUG) $(INCLUDE) -fPIC
#CFLAGS += -D __OBSS_TRACE -D __OBSS_CHECK
#CFLAGS += -D __OBSS_DEBUG

LIBS = 

OBJS = 	../src/obss_base64.o \
		../src/obss_hmac-sha1.o \
		../src/obss_md5.o \
		../src/obss_time.o \
		../src/obss_xml.o \
		../src/obss_net.o \
		../src/obss_utils.o \
		../src/obss_data-struct.o \
		../src/obss_auth.o \
		../src/obss_tcptrans.o \
		../src/obss_http.o \
		../src/obss_client.o \
		../src/obss_operation.o \
		../instances/obss_c_style.o

CPPS = $(OBJS:%.o=%.cpp)

CC = g++
#CC = arm-hisiv100nptl-linux-c++

all: demo.out ut.out

demo.out: OBSS_SDK
	$(CC) $(CFLAGS) -o $@  test_demo.cpp libobsssdk.a -lpthread

ut.out: OBSS_SDK
	$(CC) $(CFLAGS) $(GCOVR) -o $@ unit_test.cpp $(CPPS) -I./ -I/opt/gtest/include -L/opt/gtest/lib -lgtest -lgtest_main -lpthread
#	$(CC) $(CFLAGS) -o $@ unit_test.cpp libobsssdk.a -I./ -I/opt/gtest/include -L/opt/gtest/lib -larm-gtest -lpthread

OBSS_SDK: $(OBJS)
	ar rcs libobsssdk.a $(OBJS) 
	$(CC) -shared -fpic $(OBJS) -o libobsssdk.so 

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f ../src/*.o ../src/*.gcno ../src/*.gcda
	rm -f ../instances/*.o ../instances/*.gcno ../instances/*.gcda
	rm -f *.o *.a *.so *.out *.gcno *.gcda

