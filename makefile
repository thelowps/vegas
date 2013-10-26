all: server_vegas vegas

server_vegas: server_vegas.o eztcp.o
	gcc server_vegas.o eztcp.o -o server_vegas -pthread

server_vegas.o: server_vegas.c
	gcc -c server_vegas.c

vegas: vegas.o eztcp.o
	gcc vegas.o eztcp.o -o vegas -pthread

vegas.o: vegas.c
	gcc -c vegas.c

eztcp.o: eztcp.c
	gcc -c eztcp.c

clean:
	rm -rf server_vegas vegas *.o
