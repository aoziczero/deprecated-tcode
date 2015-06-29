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


	
TARGET_THREADING_NAME=tcode.threading
TARGET_THREADING_OBJS_DIR=$(OBJS_DIR)/$(TARGET_THREADING_NAME)
TARGET_THREADING_CPPFLAGS=-std=c++11 -I./ $(DBG_FLAGS) -fpermissive
TARGET_THREADING_DEPEND_FILE = $(TARGET_THREADING_OBJS_DIR)/$(DEPEND_FILE)
TARGET_THREADING_OBJS=$(TARGET_THREADING_SRCS:%.cpp=$(TARGET_THREADING_OBJS_DIR)/%.o)
TARGET_THREADING=$(TARGET_THREADING_OBJS_DIR)/lib$(TARGET_THREADING_NAME).a

TARGET_THREADING_SRCS=threading/spin_lock.cpp\


$(TARGET_THREADING_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_THREADING_CPPFLAGS) -c $< -o $@



TARGET_DIAGNOSTICS_NAME=tcode.diagnostics
TARGET_DIAGNOSTICS_OBJS_DIR=$(OBJS_DIR)/$(TARGET_DIAGNOSTICS_NAME)
TARGET_DIAGNOSTICS_CPPFLAGS=-std=c++11 -I./ $(DBG_FLAGS) -fpermissive
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
	$(CXX) $(TARGET_DIAGNOSTICS_CPPFLAGS) -c $< -o $@
	

TARGET_TEST_NAME=tcode.test
TARGET_TEST_OBJS_DIR=$(OBJS_DIR)/$(TARGET_TEST_NAME)
TARGET_TEST_CPPFLAGS=-std=c++11 -I./gtest/include -I./ $(DBG_FLAGS)
TARGET_TEST_LDFLAGS= -L./gtest/lib -lgtest -lpthread\
	-L$(TARGET_COMMON_OBJS_DIR) -ltcode.common \
	-L$(TARGET_THREADING_OBJS_DIR) -ltcode.threading \
	-L$(TARGET_DIAGNOSTICS_OBJS_DIR) -ltcode.diagnostics
	

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



all: common threading diagnostics test

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



test: $(TARGET_TEST)

$(TARGET_TEST): $(TARGET_TEST_OBJS)
	@echo "ld=$(TEST)"
	$(CXX) -o $@ $(TARGET_TEST_OBJS) $(TARGET_TEST_CPPFLAGS) $(TARGET_TEST_LDFLAGS) 
	@echo ""

clean:
	rm -rf $(TARGET_COMMON_OBJS) $(TARGET_COMMON) $(TARGET_TEST_OBJS) $(TARGET_TEST) \
	$(TARGET_THREADING_OBJS) $(TARGET_THREADING) $(TARGET_DIAGNOSTICS_OBJS) $(TARGET_DIAGNOSTICS) 
	
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
