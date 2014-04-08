CROSS_COMPILE=/opt/toolchains/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-
CC=$(CROSS_COMPILE)gcc
#CC=gcc
CFLAGS=-Wall -g

BIN_DIR=../bin

RM=rm -rf
MKDIR=mkdir -p
CP=cp

.PHONY: all clean

all: $(BIN_DIR) $(BIN_DIR)/sleep_research $(BIN_DIR)/doSomeWork $(BIN_DIR)/led5 $(BIN_DIR)/switch

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

# Rules
$(BIN_DIR)/sleep_research: sleep_research.c file_writer.c file_writer.h
	$(CC) $(CFLAGS) $^ -lrt -o $@

$(BIN_DIR)/doSomeWork: doSomeWork.c
	$(CC) $(CFLAGS) $^ -o $@
	
$(BIN_DIR)/led5: led5.c
	$(CC) $(CFLAGS) -pthread $^ -lrt -o $@
	
$(BIN_DIR)/switch: switch.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(BIN_DIR)/*
