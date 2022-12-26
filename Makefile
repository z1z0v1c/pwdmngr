CC = gcc-9
CFLAGS = -I/usr/include/sqlite3 -g -Wall -I.
LDFLAGS = -L/usr/lib -g -lsqlite3

main: main.o database.o helper.o application.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o main
