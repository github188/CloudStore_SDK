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

INCLUDE = -I./include -I./comm/include
		 
CFLAGS = -Wall -O6 $(INCLUDE)
CFLAGS += $(shell if $(CC) -march=i686 -S -o /dev/null -xc /dev/null >/dev/null 2>&1; then echo "-march=i686"; fi)
#CFLAGS += -D __OBSS_TRACE -D __OBSS_CHECK

LIBS =

OBJS = 	./comm/src/comm_base64.o \
		./comm/src/comm_hmac-sha1.o \
		./comm/src/comm_md5.o \
		./comm/src/comm_time.o \
		./comm/src/comm_xml.o \
		./comm/src/comm_net.o \
		./comm/src/comm_utils.o \
		./comm/src/comm_data.o \
		./comm/src/comm_auth.o \
		./comm/src/comm_tcptrans.o \
		./comm/src/comm_http.o \
		./src/obss_client.o \
		./src/obss_operation.o \
		./src/obss_c-wrapper.o

CC = arm-hisiv100nptl-linux-c++


all: obss_sdk 

obss_sdk: $(OBJS)
	ar rcs libobsssdk.a $(OBJS) 
	$(CC) -shared -fpic $(OBJS) -o libobsssdk.so 

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f ./src/*.o ./comm/src/*.o
	rm -f *.o *.a *.so

