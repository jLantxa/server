SRC	 	:= src
INCLUDE := include
TEST	:= test
BUILD  	:= build
DOC		:= doc

CXX_FLAGS := \
	-std=c++17 \
	-O3

LD_FLAGS := -pthread -lcrypto


init:
	@mkdir -p $(BUILD)


SERVER_SRC = \
	$(SRC)/net/Socket.cpp \
	$(SRC)/crypto/Utils.cpp \
	$(SRC)/Server.cpp

SERVER_TARGET = Server

server:
	$(CXX) $(CXX_FLAGS) \
		-I $(INCLUDE) \
		$(SERVER_SRC) \
		$(LD_FLAGS) \
	-o $(BUILD)/$(SERVER_TARGET)


clean:
	@rm -rf $(BUILD)
	@rm -rf $(DOC)
