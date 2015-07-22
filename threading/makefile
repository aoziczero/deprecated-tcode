include ./../makepre

SO_MAJOR_VER=1
SO_MINOR_VER=0

LIB_NAME=tcode.threading
LIB_SRCS=spin_lock.cpp\
	tls.cpp\

TARGET_NAME=$(LIB_NAME).out

TCODE_COMMON=tcode.common

TARGET_LD_DIR=-L$(OBJS_DIR)/$(TCODE_COMMON) \
	-L$(OBJS_DIR)/$(LIB_NAME) \
	
TARGET_LD_LIBS= -lpthread \
	-l$(LIB_NAME) \
	-l$(TCODE_COMMON)
	
TARGET_SRCS=main.cpp\

include ./../makepost