all: weather

CC = gcc
AR = ar
CFLAGS = -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -I../Adafruit_Python_DHT/source/Raspberry_Pi_2
LFLAGS = -L. -ldht -pthread -ldl


weather: main.o data.o http.o sql.o sqlite3.o calendar.o libdht
	$(CC) $(LFLAGS) main.o http.o data.o sql.o sqlite3.o calendar.o -o weather $(LFLAGS)

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

libdht:
	$(CC) $(CFLAGS) -c ../Adafruit_Python_DHT/source/Raspberry_Pi_2/pi_2_dht_read.c
	$(CC) $(CFLAGS) -c ../Adafruit_Python_DHT/source/Raspberry_Pi_2/pi_2_mmio.c
	$(CC) $(CFLAGS) -c ../Adafruit_Python_DHT/source/common_dht_read.c
	$(AR) rcs libdht.a pi_2_dht_read.o pi_2_mmio.o common_dht_read.o

clean:
	rm *.o
	if [ -f weather ]; then rm weather; fi;
