CROSS_COMPILE=/opt/toolchains/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-
CC=$(CROSS_COMPILE)gcc
#CC=gcc
CFLAGS=-Wall -g -static

BIN_DIR=../bin

RM=rm -rf
MKDIR=mkdir -p
CP=cp

SOURCES=$(wildcard *.c)
BIN_FILES=$(patsubst %.c,$(BIN_DIR)/%,$(SOURCES))

RM=rm -rf
MKDIR=mkdir -p
CP=cp

.PHONY: all clean

all: $(BIN_DIR) $(BIN_FILES)

$(BIN_DIR)/%: %.c
	$(CC) $(CFLAGS) -pthread $^ -lrt -o $@

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

clean:
	$(RM) $(BIN_FILES)
