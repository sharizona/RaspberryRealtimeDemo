CC = gcc
CFLAGS = -Wall -Wextra -O2
# macOS doesn't have librt (part of libc), Linux needs it
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS = -lpthread -lrt
else
    LDFLAGS = -lpthread
endif

TARGET = priority_inversion
SRC = priority_inversion.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	@echo "Running priority inversion demo..."
	sudo ./$(TARGET)

.PHONY: all clean run
