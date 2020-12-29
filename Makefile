TARGET := seth

CC := cc
SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)

#LFLAGS =
CFLAGS = -ggdb3 -Og

all: build

build: $(TARGET)

clean:
	rm -f $(TARGET) *.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all build clean
