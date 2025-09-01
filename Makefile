CC = gcc
CFLAGS = -fPIC -Wall -Wextra -O2
LDFLAGS = -shared

TARGET_LIB = libvector.so
SRC_LIB = vector.c

.PHONY: all compile clean test

all: compile test

compile: $(TARGET_LIB)

$(TARGET_LIB): $(SRC_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

test:
	luajit test.lua

clean:
	rm -f $(TARGET_LIB)
