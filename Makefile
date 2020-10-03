SRC     := src
INCLUDE := include
TOOLS   := tools
TEST    := test
BUILD   := build
DOC     := doc

CXX := clang++

CXX_FLAGS := \
	-std=c++17 \
	-Wall \
	-Wextra \
	-Werror \
	-O3

LD_FLAGS := -pthread -lsqlite3

all: init doxygen
	@make -j servers
	@make -j tests
	@make run-tests

servers: message notification

init:
	@mkdir -p $(BUILD)
	@mkdir -p $(BUILD)/$(TEST)

clean:
	@rm -rf $(BUILD)
	@rm -rf $(DOC)
	@make init

doxygen:
	doxygen

cloc:
	@cloc $(INCLUDE) $(SRC) $(TEST) $(TOOLS) Makefile

MESSAGE_SERVER_SRC = \
	$(SRC)/net/Socket.cpp \
	$(SRC)/Communication.cpp \
	$(SRC)/Database.cpp \
	$(SRC)/Server.cpp \
	$(SRC)/MessageServer/MessageServer.cpp

MESSAGE_SERVER_DEFINES := -DDEBUG_LEVEL=5
MESSAGE_SERVER_TARGET = MessageServer

message:
	$(CXX) $(CXX_FLAGS) \
		-I $(INCLUDE) \
		$(MESSAGE_SERVER_SRC) \
		$(LD_FLAGS) \
		$(MESSAGE_SERVER_DEFINES) \
		-o $(BUILD)/$(MESSAGE_SERVER_TARGET)


NOTIFICATION_SERVER_SRC = \
	$(SRC)/net/Socket.cpp \
	$(SRC)/Communication.cpp \
	$(SRC)/Database.cpp \
	$(SRC)/Server.cpp \
	$(SRC)/NotificationServer/NotificationServer.cpp

NOTIFICATION_DEFINES := -DDEBUG_LEVEL=5
NOTIFICATION_SERVER_TARGET = NotificationServer

notification:
	$(CXX) $(CXX_FLAGS) \
		-I $(INCLUDE) \
		$(NOTIFICATION_SERVER_SRC) \
		$(LD_FLAGS) \
		$(NOTIFICATION_DEFINES) \
		-o $(BUILD)/$(NOTIFICATION_SERVER_TARGET)


TEST_SRC += \
	$(TEST)/Test.cpp \
	$(TEST)/SocketTest.cpp \
	$(SRC)/util/TextUtils.cpp \
	$(TEST)/ServerTest.cpp \
	$(SRC)/Server.cpp \
	$(SRC)/net/Socket.cpp \
	$(SRC)/Communication.cpp \
	$(SRC)/Database.cpp

TEST_DEFINES := -DDEBUG_LEVEL=1 -DTEST
TEST_TARGET := Test

tests:
	$(CXX) $(CXX_FLAGS) \
		-I $(INCLUDE) \
		$(TEST_SRC) \
		$(LD_FLAGS) -lgtest -g3 \
		$(TEST_DEFINES) \
		-o $(BUILD)/$(TEST)/$(TEST_TARGET)

run-tests:
	./$(BUILD)/$(TEST)/$(TEST_TARGET)
