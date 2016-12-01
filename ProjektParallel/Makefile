CC = g++
CFLAGS = -Wall -pedantic -lm -lrt -lpthread -lglut -lGL -std=c++11

all: main

main: main.cpp
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm main 
