all: weather

CC = gcc
CFLAGS = -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
LFLAGS = 


weather: main.o data.o http.o sql.o sqlite3.o calendar.o
	$(CC) $(LFLAGS) main.o http.o data.o sql.o sqlite3.o calendar.o -o weather -pthread -ldl

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

data.o: data.c
	$(CC) $(CFLAGS) -c data.c

http.o: http.c
	$(CC) $(CFLAGS) -c http.c

sql.o: sql.c
	$(CC) $(CFLAGS) -c sql.c

sqlite3.o: sqlite3.c
	$(CC) $(CFLAGS) -c sqlite3.c

calendar.o: calendar.c
	$(CC) $(CFLAGS) -c calendar.c


clean:
	rm *.o
	if [ -f weather ]; then rm weather; fi;


