CC=clang

all: main alsa.o

main: main.c alsa.o
	${CC} -g -o $@ $^ -lasound

alsa.o: alsa.c alsa.h
	${CC} -g -c $<

clean:
	rm -rf *.o main
