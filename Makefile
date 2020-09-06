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

LD_FLAGS := -pthread -lcrypto

DEFINES := -DDEBUG_LEVEL=0


all: init notification


init:
	@mkdir -p $(BUILD)

clean:
	@rm -rf $(BUILD)
	@rm -rf $(DOC)
	@make init

doxygen:
	doxygen


NOTIFICATION_SERVER_SRC = \
	$(SRC)/Database.cpp \
	$(SRC)/net/Socket.cpp \
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
