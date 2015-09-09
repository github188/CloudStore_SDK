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

#DEBUG = -g

INCLUDE = -I./include -I./instances
		 
CFLAGS = -Wall -O6 $(INCLUDE) -fPIC
CFLAGS += $(shell if $(CC) -march=i686 -S -o /dev/null -xc /dev/null >/dev/null 2>&1; then echo "-march=i686"; fi)
#CFLAGS += -D __OBSS_TRACE -D __OBSS_CHECK

LIBS =

OBJS = 	./src/obss_base64.o \
		./src/obss_hmac-sha1.o \
		./src/obss_md5.o \
		./src/obss_time.o \
		./src/obss_xml.o \
		./src/obss_net.o \
		./src/obss_utils.o \
		./src/obss_data-struct.o \
		./src/obss_auth.o \
		./src/obss_tcptrans.o \
		./src/obss_http.o \
		./src/obss_client.o \
		./src/obss_operation.o \
		./instances/obss_c_style.o

CC = arm-hisiv100nptl-linux-c++


all: obss_sdk

obss_sdk: $(OBJS)
	ar rcs libobsssdk.a $(OBJS) 
	$(CC) -shared -fpic $(OBJS) -o libobsssdk.so 

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f ./src/*.o
	rm -f ./instances/*.o
	rm -f *.o *.a *.so

