CC = gcc
CFLAGS = -Wall -Wextra -O2
# macOS doesn't have librt (part of libc), Linux needs it
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS = -lpthread -lrt -lm
else
    LDFLAGS = -lpthread -lm
endif

TARGET = realtime_pthread
SRC = realtime_pthread.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	@echo "Note: This program requires root privileges"
	@echo "Running with sudo..."
	sudo ./$(TARGET)

.PHONY: all clean run
