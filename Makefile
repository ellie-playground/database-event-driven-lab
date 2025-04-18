CC = gcc
CFLAGS = -Wall -O2 -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lhiredis
TARGET = publisher
SRCS = publisher.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJS)
