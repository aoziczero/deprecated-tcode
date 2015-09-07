RM=rm
MKDIR=mkdir -p
DEPEND_FILE=depend

OS := $(shell uname)

ifdef TARGET_ARM
CC=g++
AR=ar
RANLIB=ranlib
TARGET_ARCH=arm
else
CC=g++
AR=ar
RANLIB=ranlib
TARGET_ARCH=x86
endif

TCODE_DIR=..

ifdef RELEASE
OBJS_DIR=$(TCODE_DIR)/build/$(TARGET_ARCH)/release
DBG_FLAG=-DNDEBUG
else
OBJS_DIR=$(TCODE_DIR)/build/$(TARGET_ARCH)/debug
DBG_FLAG=-DDEBUG -g
endif

ifeq ($(OS),Darwin)
MUX=io/kqueue.cpp	
else
MUX=io/epoll.cpp
endif

TCODE_SRCS=tcode.cpp\
		   error.cpp\
		   operation.cpp\
		   active_ref.cpp\
		   allocator.cpp\
		   time/timestamp.cpp\
		   time/timespan.cpp\
		   time/ctime_adapter.cpp\
		   threading/spinlock.cpp\
		   io/io.cpp\
		   io/buffer.cpp\
		   io/operation.cpp\
		   io/option.cpp\
           io/pipe.cpp\
		   io/poll.cpp\
		   $(MUX)\
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


TCODE_CFLAGS=-I$(TCODE_DIR)/include -std=c++11 -fpermissive $(DBG_FLAG)
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
		  test_io_pipe.cpp\
		  test_io_poll.cpp\
		  test_operation.cpp\
		  test_io_epoll.cpp\
		  test_io_timer.cpp\
		  test_io_engine.cpp\
		  test_io_ip_address.cpp\
		  test_io_ip_tcp_connect.cpp\
		  test_io_ip_tcp_acceptor.cpp\
		  test_io_ip_udp.cpp\
		  
 #test_epoll_wakeup.cpp\
		  #test_epoll_et.cpp\
		  

GTEST_DIR=../external/gtest

TEST_SRC_DIR=$(TCODE_DIR)/test
TEST_OBJS_DIR=$(OBJS_DIR)/$(TEST_NAME)
TEST_OUT=$(TEST_OBJS_DIR)/$(TEST_NAME).out
TEST_OBJS=$(TEST_SRCS:%.cpp=$(TEST_OBJS_DIR)/%.o)
TEST_DEPEND_FILE=$(TEST_OBJS_DIR)/$(DEPEND_FILE)

TEST_CFLAGS=$(TCODE_CFLAGS) -I$(GTEST_DIR)/include
ifeq ($(OS),Darwin)
TEST_LDFLAGS=-L$(GTEST_DIR)/lib -lgtestmac -lpthread 
else
TEST_LDFLAGS=-L$(GTEST_DIR)/lib -lgtest -lpthread
endif
all: $(TCODE_STATIC) $(TCODE_SHARED) $(TEST_OUT) 

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
	$(RM) -rf $(TCODE_STATIC_OBJS) $(TCODE_STATIC) $(TCODE_SHARED_OBJS) $(TCODE_SHARED) $(TEST_OBJS)

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
