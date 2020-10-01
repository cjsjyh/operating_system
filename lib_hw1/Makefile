CC = gcc
CFLAGS = -w
TARGET = testlib
OBJECTS = hex_dump.o hash.o bitmap.o list.o main.o hash.o

all = $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean :
	rm -f $(TARGET) $(OBJECTS)
