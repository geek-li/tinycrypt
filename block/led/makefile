CC = gcc
OBJECTS =
LIBS = -lm
CFLAGS = -I ./ #-Wall
BINDIR = 
NAME = led
SOURCES = led.c

main:
	$(CC) $(SOURCES) -DLED=128 $(CFLAGS) $(LIBS) -o led-128
	$(CC) $(SOURCES) -DLED=64 $(CFLAGS) $(LIBS) -o led-64

clean:
	rm -vf *.o $(NAME)
