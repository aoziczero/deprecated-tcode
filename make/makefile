# arm build -> make TARGET_ARM=1
# release -> make RELEASE=1
# openssl disable -> make OPENSSL=0
# poll mux -> make MUX=POLL 
#
#
RM=rm
MKDIR=mkdir -p
DEPEND_FILE=depend

OS := $(shell uname)

ifeq ($(TARGET_ARM),1)
CC=/usr/bin/arm-linux-gnueabihf-g++
AR=/usr/bin/arm-linux-gnueabihf-ar
RANLIB=/usr/bin/arm-linux-gnueabihf-ranlib
TARGET_ARCH=arm
else
CC=g++
AR=ar
RANLIB=ranlib
TARGET_ARCH=x86
endif

TCODE_DIR=..

ifeq ($(RELEASE),1)
OBJS_DIR=$(TCODE_DIR)/build/$(TARGET_ARCH)/release
DBG_FLAG=-DNDEBUG
else
OBJS_DIR=$(TCODE_DIR)/build/$(TARGET_ARCH)/debug
DBG_FLAG=-DDEBUG -g
endif

ifeq ($(MUX),POLL)
MUX_SRCS=io/poll.cpp
DBG_FLAG+= -DMUX_POLL
else 
ifeq ($(MUX),SELECT)
MUX_SRCS=io/select.cpp
DBG_FLAG+= -DMUX_SELECT
else
ifeq ($(OS),Darwin)
MUX_SRCS=io/kqueue.cpp	
else
MUX_SRCS=io/epoll.cpp	
endif
endif
endif

ifeq ($(OPENSSL),0)
OPENSSL_SRCS=
OPENSSL_INCL=
OPENSSL_LIB=
else
OPENSSL_SRCS=ssl/openssl.cpp\
			 ssl/context.cpp\
			 io/ip/tcp/pipeline/filters/openssl_filter.cpp\

OPENSSL_INCL=-I/usr/include/openssl
OPENSSL_LIB=-lssl -lcrypto
endif

TCODE_SRCS=tcode.cpp\
		   error.cpp\
		   operation.cpp\
		   allocator.cpp\
		   base64.cpp\
		   crc32.cpp\
		   block.cpp\
		   byte_buffer.cpp\
		   string.cpp\
		   time/timestamp.cpp\
		   time/timespan.cpp\
		   time/ctime_adapter.cpp\
		   time/util.cpp\
		   time/systemtime.cpp\
		   threading/spinlock.cpp\
		   threading/local_storage.cpp\
		   io/io.cpp\
		   io/buffer.cpp\
		   io/operation.cpp\
		   io/option.cpp\
		   io/pipe.cpp\
		   $(MUX_SRCS)\
		   $(OPENSSL_SRCS)\
		   io/engine.cpp\
		   io/timer.cpp\
		   io/timer_id.cpp\
		   io/ip/ip.cpp\
		   io/ip/address.cpp\
		   io/ip/option.cpp\
		   io/ip/tcp/socket.cpp\
		   io/ip/tcp/acceptor.cpp\
		   io/ip/tcp/operation_connect.cpp\
		   io/ip/tcp/operation_write.cpp\
		   io/ip/tcp/operation_read.cpp\
		   io/ip/tcp/operation_accept.cpp\
		   io/ip/udp/socket.cpp\
		   io/ip/udp/operation_read.cpp\
		   io/ip/udp/operation_write.cpp\
		   io/uds/address.cpp\
		   io/ip/raw.cpp\
		   log/record.cpp\
		   log/logger.cpp\
		   log/formatter.cpp\
		   log/writer.cpp\
		   log/console_writer.cpp\
		   log/file_writer.cpp\
		   log/serialize_formatter.cpp\
		   io/ip/tcp/pipeline/connector.cpp\
		   io/ip/tcp/pipeline/acceptor.cpp\
		   io/ip/tcp/pipeline/channel.cpp\
		   io/ip/tcp/pipeline/pipeline.cpp\
		   io/ip/tcp/pipeline/filter.cpp\
		   io/ip/tcp/pipeline/filters/size_filter.cpp\
		   io/ip/tcp/pipeline/filters/timeout_filter.cpp\
		   
		   

TCODE_SRC_DIR=$(TCODE_DIR)/src

TCODE_SO_MAJOR=1
TCODE_SO_MINOR=0

TCODE_NAME=libtcode

TCODE_SO_NAME=$(TCODE_NAME).so.$(TCODE_SO_MAJOR)
TCODE_SO_REAL_NAME=$(TCODE_SO_NAME).$(TCODE_SO_MINOR)

TCODE_STATIC_OBJS_DIR=$(OBJS_DIR)/$(TCODE_NAME)/static
TCODE_STATIC=$(TCODE_STATIC_OBJS_DIR)/$(TCODE_NAME).a

TCODE_SHARED_OBJS_DIR=$(OBJS_DIR)/$(TCODE_NAME)/shared
TCODE_SHARED=$(TCODE_SHARED_OBJS_DIR)/$(TCODE_SO_REAL_NAME)

TCODE_STATIC_OBJS=$(TCODE_SRCS:%.cpp=$(TCODE_STATIC_OBJS_DIR)/%.o)
TCODE_SHARED_OBJS=$(TCODE_SRCS:%.cpp=$(TCODE_SHARED_OBJS_DIR)/%.o)

TCODE_STATIC_DEPEND_FILE=$(TCODE_STATIC_OBJS_DIR)/$(DEPEND_FILE)
TCODE_SHARED_DEPEND_FILE=$(TCODE_SHARED_OBJS_DIR)/$(DEPEND_FILE)


TCODE_CFLAGS=-I$(TCODE_DIR)/include $(OPENSSL_INCL)  -std=c++11 -fpermissive $(DBG_FLAG)
ifeq ($(OS),Darwin)
TCODE_SHARED_CFLAGS=-shared -Wl,-install_name,
else
TCODE_SHARED_CFLAGS=-shared -Wl,-soname,
endif

TEST_NAME=testtcode
TEST_SRCS=main.cpp\
          test_compile_time_error.cpp\
          test_error.cpp\
		  test_time.cpp\
		  test_function.cpp\
		  test_threading_spinlock.cpp\
		  test_threading_local_storage.cpp\
		  test_io_pipe.cpp\
		  test_operation.cpp\
		  test_io_epoll.cpp\
		  test_io_timer.cpp\
		  test_io_engine.cpp\
		  test_io_ip_address.cpp\
		  test_io_ip_tcp_connect.cpp\
		  test_io_ip_tcp_acceptor.cpp\
		  test_io_ip_udp.cpp\
		  test_bit.cpp\
		  test_pack.cpp\
		  test_io_ip_tcp_pipeline_openssl.cpp\
		  
		  

GTEST_DIR=../external/gtest

TEST_SRC_DIR=$(TCODE_DIR)/test
TEST_OBJS_DIR=$(OBJS_DIR)/$(TEST_NAME)
TEST_OUT=$(TEST_OBJS_DIR)/$(TEST_NAME).out
TEST_OBJS=$(TEST_SRCS:%.cpp=$(TEST_OBJS_DIR)/%.o)
TEST_DEPEND_FILE=$(TEST_OBJS_DIR)/$(DEPEND_FILE)

TEST_CFLAGS=$(TCODE_CFLAGS) -I$(GTEST_DIR)/include
ifeq ($(OS),Darwin)
TEST_LDFLAGS=-L$(GTEST_DIR)/lib -lgtestmac -lpthread $(OPENSSL_LIB)
else
TEST_LDFLAGS=-L$(GTEST_DIR)/lib -lgtest -lpthread  $(OPENSSL_LIB)
endif

ECHO_NAME=echo
ECHO_SRCS=pipeline_echo.cpp

ECHO_SRC_DIR=$(TCODE_DIR)/test
ECHO_OBJS_DIR=$(OBJS_DIR)/$(ECHO_NAME)
ECHO_OUT=$(ECHO_OBJS_DIR)/$(ECHO_NAME).out
ECHO_OBJS=$(ECHO_SRCS:%.cpp=$(ECHO_OBJS_DIR)/%.o)
ECHO_DEPEND_FILE=$(ECHO_OBJS_DIR)/$(DEPEND_FILE)
ECHO_CFLAGS=$(TCODE_CFLAGS) -I$(GTEST_DIR)/include
ECHO_LDFLAGS=-lpthread  $(OPENSSL_LIB)


all: $(TCODE_STATIC) $(TCODE_SHARED) $(TEST_OUT) 

echo: $(ECHO_OUT)

$(ECHO_OUT):$(TCODE_STATIC) $(ECHO_OBJS)
	$(CC) -o $@ $(ECHO_OBJS) $(TCODE_STATIC_OBJS) $(ECHO_CFLAGS) $(ECHO_LDFLAGS)

$(ECHO_OBJS_DIR)/%.o:$(ECHO_SRC_DIR)/%.cpp
	@echo "Compile=$(dir $@)"
	@`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)`
	$(CC) $(ECHO_CFLAGS) -c $< -o $@


test: $(TEST_OUT)

$(TEST_OUT):$(TCODE_STATIC) $(TEST_OBJS)
	$(CC) -o $@ $(TEST_OBJS) $(TCODE_STATIC_OBJS) $(TEST_CFLAGS) $(TEST_LDFLAGS)

$(TEST_OBJS_DIR)/%.o:$(TEST_SRC_DIR)/%.cpp
	@echo "Compile=$(dir $@)"
	@`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)`
	$(CC) $(TEST_CFLAGS) -c $< -o $@


$(TCODE_STATIC):$(TCODE_STATIC_OBJS)
	@echo "archive=$@"
	$(AR) rvs $@ $(TCODE_STATIC_OBJS)
	$(RANLIB) $@
	@echo ""

$(TCODE_SHARED):$(TCODE_SHARED_OBJS)
	$(CC) $(TCODE_SHARED_CFLAGS)$(TCODE_SO_NAME) -o $@ $(TCODE_SHARED_OBJS)

$(TCODE_STATIC_OBJS_DIR)/%.o: $(TCODE_SRC_DIR)/%.cpp
	@echo "Compile=$(dir $@)"
	@`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)`
	$(CC) $(TCODE_CFLAGS) -c $< -o $@

$(TCODE_SHARED_OBJS_DIR)/%.o:  $(TCODE_SRC_DIR)/%.cpp
	@echo "Compile=$(dir $@)"
	@`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)`
	$(CC) $(TCODE_CFLAGS) -fPIC -c $< -o $@

clean:
	$(RM) -rf $(OBJS_DIR)

temp:
	@echo $(TCODE_SRCS)
	@echo $(TCODE_STATIC_OBJS)

depend:
	@`[ -d $(TCODE_STATIC_OBJS_DIR) ] || $(MKDIR) $(TCODE_STATIC_OBJS_DIR)`
	@`[ -d $(TCODE_SHARED_OBJS_DIR) ] || $(MKDIR) $(TCODE_SHARED_OBJS_DIR)`
	@ $(RM) -f $(TCODE_STATIC_DEPEND_FILE) $(TCODE_SHARED_DEPEND_FILE)
	@ for FILE in $(TCODE_SRCS:%.cpp=%);do\
		$(CC) $(TCODE_CFLAGS) -MM -MT $(TCODE_STATIC_OBJS_DIR)/$$FILE.o $(TCODE_SRC_DIR)/$$FILE.cpp >> $(TCODE_STATIC_DEPEND_FILE);\
		$(CC) $(TCODE_CFLAGS) -MM -MT $(TCODE_SHARED_OBJS_DIR)/$$FILE.o $(TCODE_SRC_DIR)/$$FILE.cpp >> $(TCODE_SHARED_DEPEND_FILE);\
	done

-include $(TCODE_STATIC_OBJS_DIR)/$(DEPEND_FILE)
-include $(TCODE_SHARED_OBJS_DIR)/$(DEPEND_FILE)
