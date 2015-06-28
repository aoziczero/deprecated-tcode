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

TARGET_COMMON_NAME=tcode.common
TARGET_COMMON_OBJS_DIR=$(OBJS_DIR)/$(TARGET_COMMON_NAME)
TARGET_COMMON_CPPFLAGS=-std=c++11 -I./ $(DBG_FLAGS) -fpermissive
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
	$(CXX) $(TARGET_COMMON_CPPFLAGS) -c $< -o $@


	

TARGET_BUFFER_NAME=tcode.common
TARGET_BUFFER_OBJS_DIR=$(OBJS_DIR)/$(TARGET_BUFFER_NAME)
TARGET_BUFFER_CPPFLAGS=-std=c++11 -I./ $(DBG_FLAGS) -fpermissive
TARGET_BUFFER_DEPEND_FILE = $(TARGET_BUFFER_OBJS_DIR)/$(DEPEND_FILE)
TARGET_BUFFER_OBJS=$(TARGET_BUFFER_SRCS:%.cpp=$(TARGET_BUFFER_OBJS_DIR)/%.o)
TARGET_BUFFER=$(TARGET_BUFFER_OBJS_DIR)/lib$(TARGET_BUFFER_NAME).a

TARGET_BUFFER_SRCS=


$(TARGET_BUFFER_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_BUFFER_CPPFLAGS) -c $< -o $@


TARGET_TEST_NAME=tcode.test
TARGET_TEST_OBJS_DIR=$(OBJS_DIR)/$(TARGET_TEST_NAME)
TARGET_TEST_CPPFLAGS=-std=c++11 -I./gtest/include -I./ $(DBG_FLAGS)
TARGET_TEST_LDFLAGS= -L./gtest/lib -lgtest -lpthread\
	-L$(TARGET_COMMON_OBJS_DIR) -ltcode.common

TARGET_TEST_DEPEND_FILE = $(TARGET_TEST_OBJS_DIR)/$(DEPEND_FILE)
TARGET_TEST_OBJS=$(TARGET_TEST_SRCS:%.cpp=$(TARGET_TEST_OBJS_DIR)/%.o)
TARGET_TEST=tests.out

TARGET_TEST_SRCS=tests/tests.cpp\
tests/test_common_closure.cpp

$(TARGET_TEST_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_TEST_CPPFLAGS) -c $< -o $@



all: common buffer test

common: $(TARGET_COMMON)

$(TARGET_COMMON): $(TARGET_COMMON_OBJS)
	@echo "archive=$(TARGET_COMMON)"
	$(AR) rvs $@ $(TARGET_COMMON_OBJS)
	$(RANLIB) $@
	@echo ""

buffer: $(TARGET_BUFFER)

$(TARGET_BUFFER): $(TARGET_BUFFER_OBJS)
	@echo "archive=$(TARGET_BUFFER)"
	$(AR) rvs $@ $(TARGET_BUFFER_OBJS)
	$(RANLIB) $@
	@echo ""


test: $(TARGET_TEST)

$(TARGET_TEST): $(TARGET_TEST_OBJS)
	@echo "ld=$(TEST)"
	$(CXX) -o $@ $(TARGET_TEST_OBJS) $(TARGET_TEST_CPPFLAGS) $(TARGET_TEST_LDFLAGS) 
	@echo ""

clean:
	rm -rf $(TARGET_COMMON_OBJS) $(TARGET_COMMON) $(TARGET_TEST_OBJS) $(TARGET_TEST)
	
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
