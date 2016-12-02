CC = g++
CFLAGS =  -pedantic -lm -lrt -lpthread -lglut -lGL -std=c++11

all: main

main: main.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm main 
