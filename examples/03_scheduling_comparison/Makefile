CC = gcc
CFLAGS = -Wall -Wextra -O2
# macOS doesn't have librt (part of libc), Linux needs it
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS = -lpthread -lrt -lm
else
    LDFLAGS = -lpthread -lm
endif

TARGET = sched_comparison
SRC = sched_comparison.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	@echo "Running scheduling comparison test..."
	sudo ./$(TARGET)

.PHONY: all clean run
