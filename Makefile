install: all
	sudo mv miniftp /bin

all: miniftp
	mkdir test

miniftp: client.o server.o miniftp.o
	gcc conhandler.o filefinder.o client.o server.o miniftp.o -o miniftp

miniftp.o: server.o client.o
	gcc -c src/miniftp.c -o miniftp.o

client.o: filefinder.o
	gcc -c src/net/client.c -o client.o

server.o: filefinder.o conhandler.o
	gcc -c src/net/server.c -o server.o

filefinder.o:
	gcc -c src/utils/filefinder.c -o filefinder.o

conhandler.o: filefinder.o
	gcc -c src/net/conhandler.c -o conhandler.o

clean:
	rm -f *.o miniftp
	rm -f -r test
	rm -f src/utils/test_utils/obj/*
	rm -f src/utils/test_utils/bin/*


testqueue: test_queue.o
	gcc src/utils/test_utils/obj/queue.o src/utils/test_utils/obj/test_queue.o -o src/utils/test_utils/bin/test_queue

test_queue.o: queue.o
	gcc -c src/utils/test_utils/test_queue.c -o src/utils/test_utils/obj/test_queue.o

queue.o:
	gcc -c src/utils/queue.c -o src/utils/test_utils/obj/queue.o