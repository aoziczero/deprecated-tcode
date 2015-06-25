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

TARGET_NAME=tcode
TARGET_OBJS_DIR=$(OBJS_DIR)/$(TARGET_NAME)
TARGET_CPPFLAGS=-std=c++11 -I./tcode -I./ $(DBG_FLAGS) -fpermissive
TARGET_DEPEND_FILE = $(TARGET_OBJS_DIR)/$(DEPEND_FILE)
TARGET_SRCS=

TARGET_OBJS=$(TARGET_SRCS:%.cpp=$(TARGET_OBJS_DIR)/%.o)

TARGET=$(TARGET_OBJS_DIR)/lib$(TARGET_NAME).a

$(TARGET_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TARGET_CPPFLAGS) -c $< -o $@


TEST_NAME=testtcode
TEST_OBJS_DIR=$(OBJS_DIR)/$(TEST_NAME)
TEST_CPPFLAGS=-std=c++11 -I./gtest/include -I./ -I./testtcode $(DBG_FLAGS)
TEST_LDFLAGS= -L./gtest/lib -lgtest -L$(TARGET_OBJS_DIR) -ltcode -lpthread
TEST_DEPEND_FILE = $(TEST_OBJS_DIR)/$(DEPEND_FILE)
TEST_SRCS=

TEST_OBJS=$(TEST_SRCS:%.cpp=$(TEST_OBJS_DIR)/%.o)

TEST=$(TEST_OBJS_DIR)/$(TEST_NAME).exe

$(TEST_OBJS_DIR)/%.o : %.cpp
	@echo "Compile=$(dir $@)"
	$`[ -d $(dir $@) ] || $(MKDIR) $(dir $@)
	$(CXX) $(TEST_CPPFLAGS) -c $< -o $@


all: $(TARGET)

$(TARGET): $(TARGET_OBJS)
	@echo "archive=$(TARGET)"
	$(AR) rvs $@ $(TARGET_OBJS)
	$(RANLIB) $@
	@echo ""


test: $(TEST)

$(TEST): $(TEST_OBJS)
	@echo "ld=$(TEST)"
	$(CXX) -o $@ $(TEST_OBJS) $(TEST_CPPFLAGS) $(TEST_LDFLAGS) 
	@echo ""

clean:
	rm -rf $(TARGET_OBJS) $(TARGET) $(TEST_OBJS) $(TEST)
	
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
