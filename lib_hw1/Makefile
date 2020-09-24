CC = gcc
CFLAGS = -W
TARGET = testlib
OBJECTS = hex_dump.o hash.o bitmap.o list.o main.o

all = $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean :
	rm -f $(TARGET) $(OBJECTS)
