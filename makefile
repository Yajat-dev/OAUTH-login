CC = g++
CFLAGS = -o2
SRC = file.cpp code.cpp context.cpp login.cpp
INC = file.hpp code.hpp context.hpp
OBJ = $(SRC:%.cpp=%.o)
LDFLAGS = -lcrypto -lssl
BIN = oauth.login

.PHONY: all debug clean install

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

login.o: login.cpp $(INC)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp %.hpp $(INC)
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS = -ggdb3 -o0
debug: all

clean: 
	rm *.o oauth.login

install:
	install -m 700 $(BIN) ~/.local/bin/
