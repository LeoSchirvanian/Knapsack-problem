CC=gcc
CFLAGS=-W -Wall
LIBS += -lm
EXEC=tp3

all: $(EXEC)

$(EXEC): tp3.o
	$(CC) -o $@ $< $(CFLAGS) $(LIBS)

tp3.o: tp3.c
	$(CC) -o $@ -c $< $(CFLAGS) $(LIBS)

clean:
	rm -rf *.o $(EXEC)
