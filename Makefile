CC = gcc
CFLAGS = -Wall -ggdb
LIBS = -lcurl
BIN = sendsms
OBJ = odoriksms.o sendsms.o

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LIBS)

clean::
	rm *.o
	rm $(BIN)
