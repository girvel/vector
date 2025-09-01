LUA_INC = /usr/include/lua5.3
LUA_LIB = -llua5.3

# Compiler and flags
CC = gcc
CFLAGS = -fPIC -Wall -Wextra -O2
LDFLAGS = -shared

# The target shared library
TARGET = vector.so

# The source file
SRC = vector.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(LUA_INC) $(LUA_LIB) -o $@ $<

clean:
	rm -f $(TARGET)
