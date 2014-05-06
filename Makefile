CC=gcc
CFLAGS=-W -Wall -Wno-unused-parameter
LDFLAGS= -lSDL -lGL -lGLU
EXEC=kgdflow

all: $(EXEC)

kgdflow: main.o view.o model.o config.o error.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)