CC = g++
CFLAGS = -o2
SRC = code.cpp context.cpp helper.cpp login.cpp
INC = code.hpp context.hpp helper.hpp
OBJ = $(SRC:%.cpp=%.o)

.PHONY: all debug clean

all: oauth.login

oauth.login: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

login.o: login.cpp $(INC)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp %.hpp $(INC)
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS = -ggdb3 -o0
debug: all

clean: 
	rm *.o oauth.login
