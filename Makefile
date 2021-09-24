all: miniftp
	mkdir test

miniftp: client.o server.o miniftp.o
	gcc filefinder.o client.o server.o miniftp.o -o miniftp

miniftp.o: server.o client.o
	gcc -c src/miniftp.c -o miniftp.o

client.o: filefinder.o
	gcc -c src/net/client.c -o client.o

server.o: filefinder.o
	gcc -c src/net/server.c -o server.o

filefinder.o:
	gcc -c src/utils/filefinder.c -o filefinder.o

clean:
	rm *.o miniftp
	rm -r test