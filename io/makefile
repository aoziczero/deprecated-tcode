include ./../makepre

SO_MAJOR_VER=1
SO_MINOR_VER=0

LIB_INCLUDES+= -I./
LIB_NAME=tcode.io
LIB_SRCS=completion_handler.cpp\
epoll.cpp\
io.cpp\
ip/address.cpp\
ip/basic_socket.cpp\
ip/resolver.cpp\

TARGET_NAME=$(LIB_NAME).out

TCODE_COMMON=tcode.common
TCODE_BUFFER=tcode.buffer
TCODE_THREADING=tcode.threading
TCODE_DIAGNOSTICS=tcode.diagnostics

TARGET_LD_DIR=-L$(OBJS_DIR)/$(TCODE_COMMON) \
	-L$(OBJS_DIR)/$(TCODE_BUFFER) \
	-L$(OBJS_DIR)/$(TCODE_THREADING) \	
	-L$(OBJS_DIR)/$(TCODE_DIAGNOSTICS) \
	-L$(OBJS_DIR)/$(LIB_NAME) \
	
TARGET_LD_LIBS= -lpthread \
	-l$(LIB_NAME) \
	-l$(TCODE_DIAGNOSTICS) \
	-l$(TCODE_THREADING) \
	-l$(TCODE_BUFFER) \
	-l$(TCODE_COMMON)
	
TARGET_SRCS=main.cpp\

include ./../makepost