CC 			= g++
STD 		= -std=c++17
WARNINGS 	= -Wall #-Wextra #-Wpedantic #-Werror
LDFLAGS 	= ${STD} $(WARNINGS) -lIce

SRC_DIR		= src
OBJ_DIR		= obj

ICEOUT 		= ./${SRC_DIR}
ICEFLAGS 	= --impl-c++17 --output-dir $(ICEOUT)

all: client server factory

slice:
	mkdir -p ${SRC_DIR}
	slice2cpp ${ICEFLAGS} chat.ice

client: client.cpp $(SRC_DIR)/chat.cpp $(SRC_DIR)/chatI.cpp
	g++ -I${SRC_DIR} $^ -lIce -lpthread -o $@

server: server.cpp $(SRC_DIR)/chat.cpp $(SRC_DIR)/chatI.cpp
	g++ -I${SRC_DIR} $^ -lIce -lpthread -o $@

factory: factory.cpp $(SRC_DIR)/chat.cpp $(SRC_DIR)/chatI.cpp
	g++ -I${SRC_DIR} $^ -lIce -lpthread -o $@

clean:
	rm -r client server
