CXX=/usr/bin/g++-5
AR=/usr/bin/ar
RANLIB=/usr/bin/ranlib
RM=rm
MKDIR=mkdir -p
DEPEND_FILE=depend

ifeq ( $(RELEASE),1)
OBJS_DIR  = ./build/release
DBG_FLAGS = -DNDEBUG 
else
OBJS_DIR  = ./build/debug
DBG_FLAGS = -DDEBUG -g
endif

TARGET_CPPFLAGS=-std=c++11 -I./ $(DBG_FLAGS) -fpermissive

TARGET_COMMON_NAME=tcode.common
TARGET_COMMON_OBJS_DIR=$(OBJS_DIR)/$(TARGET_COMMON_NAME)
TARGET_COMMON_DEPEND_FILE = $(TARGET_COMMON_OBJS_DIR)/$(DEPEND_FILE)
TARGET_COMMON_OBJS=$(TARGET_COMMON_SRCS:%.cpp=$(TARGET_COMMON_OBJS_DIR)/%.o)
TARGET_COMMON=$(TARGET_COMMON_OBJS_DIR)/lib$(TARGET_COMMON_NAME).a
TARGET_COMMON_SRCS=common/lib_allocator.cpp\
common/time_span.cpp\
common/time_stamp.cpp\
common/time_util.cpp\
common/rtti.cpp\

$(TARGET_COMMON_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_CPPFLAGS) -c $< -o $@




TARGET_THREADING_NAME=tcode.threading
TARGET_THREADING_OBJS_DIR=$(OBJS_DIR)/$(TARGET_THREADING_NAME)
TARGET_THREADING_DEPEND_FILE = $(TARGET_THREADING_OBJS_DIR)/$(DEPEND_FILE)
TARGET_THREADING_OBJS=$(TARGET_THREADING_SRCS:%.cpp=$(TARGET_THREADING_OBJS_DIR)/%.o)
TARGET_THREADING=$(TARGET_THREADING_OBJS_DIR)/lib$(TARGET_THREADING_NAME).a

TARGET_THREADING_SRCS=threading/spin_lock.cpp\
threading/tls.cpp\

$(TARGET_THREADING_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_CPPFLAGS) -c $< -o $@




TARGET_DIAGNOSTICS_NAME=tcode.diagnostics
TARGET_DIAGNOSTICS_OBJS_DIR=$(OBJS_DIR)/$(TARGET_DIAGNOSTICS_NAME)
TARGET_DIAGNOSTICS_DEPEND_FILE = $(TARGET_DIAGNOSTICS_OBJS_DIR)/$(DEPEND_FILE)
TARGET_DIAGNOSTICS_OBJS=$(TARGET_DIAGNOSTICS_SRCS:%.cpp=$(TARGET_DIAGNOSTICS_OBJS_DIR)/%.o)
TARGET_DIAGNOSTICS=$(TARGET_DIAGNOSTICS_OBJS_DIR)/lib$(TARGET_DIAGNOSTICS_NAME).a

TARGET_DIAGNOSTICS_SRCS=diagnostics/tcode_error_code.cpp\
diagnostics/tcode_category_impl.cpp\
diagnostics/posix_category_impl.cpp\
diagnostics/last_error.cpp\

$(TARGET_DIAGNOSTICS_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_CPPFLAGS) -c $< -o $@
	



TARGET_BUFFER_NAME=tcode.buffer
TARGET_BUFFER_OBJS_DIR=$(OBJS_DIR)/$(TARGET_BUFFER_NAME)
TARGET_BUFFER_DEPEND_FILE = $(TARGET_BUFFER_OBJS_DIR)/$(DEPEND_FILE)
TARGET_BUFFER_OBJS=$(TARGET_BUFFER_SRCS:%.cpp=$(TARGET_BUFFER_OBJS_DIR)/%.o)
TARGET_BUFFER=$(TARGET_BUFFER_OBJS_DIR)/lib$(TARGET_BUFFER_NAME).a

TARGET_BUFFER_SRCS=buffer/block.cpp\
buffer/byte_buffer.cpp\


$(TARGET_BUFFER_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_CPPFLAGS) -c $< -o $@




TARGET_IO_NAME=tcode.io
TARGET_IO_OBJS_DIR=$(OBJS_DIR)/$(TARGET_IO_NAME)
TARGET_IO_DEPEND_FILE = $(TARGET_IO_OBJS_DIR)/$(DEPEND_FILE)
TARGET_IO_OBJS=$(TARGET_IO_SRCS:%.cpp=$(TARGET_IO_OBJS_DIR)/%.o)
TARGET_IO=$(TARGET_IO_OBJS_DIR)/lib$(TARGET_IO_NAME).a

TARGET_IO_SRCS=io/completion_handler.cpp\
io/epoll.cpp\
io/address.cpp\
io/basic_socket.cpp\
io/resolver.cpp\


$(TARGET_IO_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_CPPFLAGS) -c $< -o $@



TARGET_TRANSPORT_NAME=tcode.transport
TARGET_TRANSPORT_OBJS_DIR=$(OBJS_DIR)/$(TARGET_TRANSPORT_NAME)
TARGET_TRANSPORT_DEPEND_FILE = $(TARGET_TRANSPORT_OBJS_DIR)/$(DEPEND_FILE)
TARGET_TRANSPORT_OBJS=$(TARGET_TRANSPORT_SRCS:%.cpp=$(TARGET_TRANSPORT_OBJS_DIR)/%.o)
TARGET_TRANSPORT=$(TARGET_TRANSPORT_OBJS_DIR)/lib$(TARGET_TRANSPORT_NAME).a

TARGET_TRANSPORT_SRCS=transport/event_loop.cpp\

$(TARGET_TRANSPORT_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_CPPFLAGS) -c $< -o $@





TARGET_TEST_NAME=tcode.test
TARGET_TEST_OBJS_DIR=$(OBJS_DIR)/$(TARGET_TEST_NAME)
TARGET_TEST_CPPFLAGS=-std=c++11 -I./gtest/include -I./ $(DBG_FLAGS)
TARGET_TEST_LDFLAGS= -L./gtest/lib -lgtest -lpthread\
	-L$(TARGET_COMMON_OBJS_DIR) -l$(TARGET_COMMON_NAME) \
	-L$(TARGET_THREADING_OBJS_DIR) -l$(TARGET_THREADING_NAME) \
	-L$(TARGET_DIAGNOSTICS_OBJS_DIR) -l$(TARGET_DIAGNOSTICS_NAME) \
	-L$(TARGET_BUFFER_OBJS_DIR) -l$(TARGET_BUFFER_NAME) \
	-L$(TARGET_IO_OBJS_DIR) -l$(TARGET_IO_NAME) \
	-L$(TARGET_TRANSPORT_OBJS_DIR) -l$(TARGET_TRANSPORT_NAME) \
	

TARGET_TEST_DEPEND_FILE = $(TARGET_TEST_OBJS_DIR)/$(DEPEND_FILE)
TARGET_TEST_OBJS=$(TARGET_TEST_SRCS:%.cpp=$(TARGET_TEST_OBJS_DIR)/%.o)
TARGET_TEST=tests.out

TARGET_TEST_SRCS=tests/tests.cpp\
tests/test_common_closure.cpp\
tests/test_diagnostics_last_error.cpp

$(TARGET_TEST_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_TEST_CPPFLAGS) -c $< -o $@



all: common threading diagnostics buffer io transport test

common: $(TARGET_COMMON)

$(TARGET_COMMON): $(TARGET_COMMON_OBJS)
	@echo "archive=$(TARGET_COMMON)"
	$(AR) rvs $@ $(TARGET_COMMON_OBJS)
	$(RANLIB) $@
	@echo ""

threading: $(TARGET_THREADING)

$(TARGET_THREADING): $(TARGET_THREADING_OBJS)
	@echo "archive=$(TARGET_THREADING)"
	$(AR) rvs $@ $(TARGET_THREADING_OBJS)
	$(RANLIB) $@
	@echo ""

	
diagnostics: $(TARGET_DIAGNOSTICS)

$(TARGET_DIAGNOSTICS): $(TARGET_DIAGNOSTICS_OBJS)
	@echo "archive=$(TARGET_DIAGNOSTICS)"
	$(AR) rvs $@ $(TARGET_DIAGNOSTICS_OBJS)
	$(RANLIB) $@
	@echo ""

	
buffer: $(TARGET_BUFFER)

$(TARGET_BUFFER): $(TARGET_BUFFER_OBJS)
	@echo "archive=$(TARGET_BUFFER)"
	$(AR) rvs $@ $(TARGET_BUFFER_OBJS)
	$(RANLIB) $@
	@echo ""


	
io: $(TARGET_IO)

$(TARGET_IO): $(TARGET_IO_OBJS)
	@echo "archive=$(TARGET_IO)"
	$(AR) rvs $@ $(TARGET_IO_OBJS)
	$(RANLIB) $@
	@echo ""

	
transport: $(TARGET_TRANSPORT)

$(TARGET_TRANSPORT): $(TARGET_TRANSPORT_OBJS)
	@echo "archive=$(TARGET_TRANSPORT)"
	$(AR) rvs $@ $(TARGET_TRANSPORT_OBJS)
	$(RANLIB) $@
	@echo ""


test: $(TARGET_TEST)

$(TARGET_TEST): $(TARGET_TEST_OBJS)
	@echo "ld=$(TEST)"
	$(CXX) -o $@ $(TARGET_TEST_OBJS) $(TARGET_TEST_CPPFLAGS) $(TARGET_TEST_LDFLAGS) 
	@echo ""

clean:
	rm -rf $(TARGET_COMMON_OBJS) $(TARGET_COMMON) \
	$(TARGET_COMMON_OBJS) $(TARGET_COMMON) \
	$(TARGET_THREADING_OBJS) $(TARGET_THREADING) \
	$(TARGET_DIAGNOSTICS_OBJS) $(TARGET_DIAGNOSTICS) \
	$(TARGET_BUFFER_OBJS) $(TARGET_BUFFER) \
	$(TARGET_IO_OBJS) $(TARGET_IO) \
	$(TARGET_TRANSPORT_OBJS) $(TARGET_TRANSPORT) \
	$(TARGET_TEST_OBJS) $(TARGET_TEST) \
	
test_clean:
	rm -rf $(TEST_OBJS) $(TEST)

depend:
	$'[ -d $(TARGET_OBJS_DIR) ] || $(MKDIR) $(TARGET_OBJS_DIR)
	$(RM) -f $(TARGET_DEPEND_FILE)
	@for FILE in $(TARGET_SRCS:%.cpp=%); do\
		$(CXX) -MM -MT $(TARGET_OBJS_DIR)/$$FILE.o $$FILE.cpp $(TARGET_CPPFLAGS) >> $(TARGET_DEPEND_FILE); \
	done

echo:
	echo $(OBJS)

ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), depend)
ifneq ($(SRCS),)
-include $(TARGET_DEPEND_FILE) 
endif
endif
endif
