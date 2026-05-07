CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -I..
LDFLAGS=-lcriterion

SRC=../base64.c test_base64.c
OBJ=$(SRC:.c=.o)
TARGET=test_base64

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)