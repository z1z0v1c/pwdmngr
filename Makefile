CC = gcc
CFLAGS = -I/usr/include/sqlite3
LDFLAGS = -L/usr/lib -lsqlite3

main: main.o database.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o main
