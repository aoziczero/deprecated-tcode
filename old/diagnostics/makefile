include ./../makepre

SO_MAJOR_VER=1
SO_MINOR_VER=0

LIB_INCLUDES+= -I./
LIB_NAME=tcode.diagnostics
LIB_SRCS=tcode_error_code.cpp\
	tcode_category_impl.cpp\
	posix_category_impl.cpp\
	last_error.cpp\
	log/record.cpp\
	log/logger.cpp\
	log/writer.cpp\
	log/formatter.cpp\
	log/console_writer.cpp\
	log/file_writer.cpp\


TARGET_NAME=$(LIB_NAME).out

TCODE_COMMON=tcode.common
TCODE_BUFFER=tcode.buffer
TCODE_THREADING=tcode.threading

TARGET_LD_DIR=-L$(OBJS_DIR)/$(TCODE_COMMON) \
	-L$(OBJS_DIR)/$(TCODE_BUFFER) \
	-L$(OBJS_DIR)/$(TCODE_THREADING) \
	-L$(OBJS_DIR)/$(LIB_NAME) \
	
TARGET_LD_LIBS= -lpthread \
	-l$(LIB_NAME) \
	-l$(TCODE_THREADING) \
	-l$(TCODE_BUFFER) \
	-l$(TCODE_COMMON)

TARGET_SRCS=main.cpp\

include ./../makepost