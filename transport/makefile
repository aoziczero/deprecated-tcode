include ./../makepre

SO_MAJOR_VER=1
SO_MINOR_VER=0

LIB_INCLUDES+= -I./
LIB_NAME=tcode.transport
LIB_SRCS=event_loop.cpp\
	event_timer.cpp\
	event_timer.cpp\
	tcp/acceptor.cpp\
	tcp/acceptor_handler.cpp\
	tcp/channel.cpp\
	tcp/channel_config.cpp\
	tcp/connector.cpp\
	tcp/connector_handler.cpp\
	tcp/filter.cpp\
	tcp/packet_buffer.cpp\
	tcp/pipeline.cpp\
	tcp/filters/size_filter.cpp\
	tcp/filters/timeout_filter.cpp\
	udp/bootstrap.cpp\
	udp/channel.cpp\
	udp/pipeline.cpp\
	udp/filter.cpp\

TARGET_NAME=$(LIB_NAME).out

TCODE_COMMON=tcode.common
TCODE_BUFFER=tcode.buffer
TCODE_THREADING=tcode.threading
TCODE_DIAGNOSTICS=tcode.diagnostics
TCODE_IO=tcode.io

TARGET_LD_DIR=-L$(OBJS_DIR)/$(TCODE_COMMON) \
	-L$(OBJS_DIR)/$(TCODE_BUFFER) \
	-L$(OBJS_DIR)/$(TCODE_THREADING) \
	-L$(OBJS_DIR)/$(TCODE_DIAGNOSTICS) \
	-L$(OBJS_DIR)/$(TCODE_IO) \
	-L$(OBJS_DIR)/$(LIB_NAME) \
	
TARGET_LD_LIBS= -lpthread \
	-l$(LIB_NAME) \
	-l$(TCODE_IO) \
	-l$(TCODE_DIAGNOSTICS) \
	-l$(TCODE_THREADING) \
	-l$(TCODE_BUFFER) \
	-l$(TCODE_COMMON)
	
TARGET_SRCS=main.cpp\


include ./../makepost