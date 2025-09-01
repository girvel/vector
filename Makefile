# Makefile for the vec2 Lua library

# Your Lua include and library paths might be different.
LUA_INC = /usr/include/lua5.3
LUA_LIB = -llua5.3

# Compiler and flags
CC = gcc
CFLAGS = -fPIC -Wall -Wextra -O2
LDFLAGS = -shared

# The target shared library
TARGET = vec2.so

# The source file
SRC = vec2.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(LUA_INC) $(LUA_LIB) -o $@ $<

clean:
	rm -f $(TARGET)
