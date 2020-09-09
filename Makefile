SRC	 	:= src
INCLUDE := include
TEST	:= test
BUILD  	:= build
DOC		:= doc

CXX := clang++

CXX_FLAGS := \
	-std=c++17 \
	-Wall \
	-Wextra \
	-Werror \
	-O3

LD_FLAGS := -pthread -lcrypto -lsqlite3

DEFINES := -DDEBUG_LEVEL=100


all: init doxygen
	@make -j servers

servers: notification

init:
	@mkdir -p $(BUILD)

clean:
	@rm -rf $(BUILD)
	@rm -rf $(DOC)
	@make init

doxygen:
	doxygen

cloc:
	@cloc $(INCLUDE) $(SRC) $(TEST) Makefile


NOTIFICATION_SERVER_SRC = \
	$(SRC)/net/Socket.cpp \
	$(SRC)/Communication.cpp \
	$(SRC)/Database.cpp \
	$(SRC)/Server.cpp \
	$(SRC)/NotificationServer/NotificationServer.cpp
NOTIFICATION_SERVER_TARGET = NotificationServer

notification:
	$(CXX) $(CXX_FLAGS) \
		-I $(INCLUDE) \
		$(NOTIFICATION_SERVER_SRC) \
		$(LD_FLAGS) \
		$(DEFINES) \
		-o $(BUILD)/$(NOTIFICATION_SERVER_TARGET)
