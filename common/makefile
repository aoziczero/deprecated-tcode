include ./../makepre

SO_MAJOR_VER=1
SO_MINOR_VER=0

LIB_NAME=tcode.common
LIB_SRCS=lib_allocator.cpp\
	time_span.cpp\
	time_stamp.cpp\
	time_util.cpp\
	rtti.cpp\
	string.cpp\
	big_integer.cpp

TARGET_NAME=$(LIB_NAME).out
TARGET_LD_DIR=-L$(OBJS_DIR)/$(LIB_NAME)
TARGET_LD_LIBS= -lpthread -l$(LIB_NAME)
TARGET_SRCS=main.cpp

include ./../makepost