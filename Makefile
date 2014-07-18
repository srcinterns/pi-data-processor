CC=gcc

all: main alsa.o

main: main.c alsa.o network.o
	${CC} -g -o $@ $^ -lasound

alsa.o: alsa.c alsa.h
	${CC} -g -c $<

network.o: network.c network.h
	${CC} -g -c $<

clean:
	rm -rf *.o main
