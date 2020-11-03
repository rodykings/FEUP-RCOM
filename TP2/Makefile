CC=gcc
CFLAGS=-I -w
DEPS = stateMachine.h
OBJ = receiver.o stateMachine.o imitter.o

%.o: %.c $(DEPS)
	$(CC)-c -o $@ $< $(CFLAGS)

prog: $(OBJ)
	$(CC)-o $@ $^ $(CFLAGS)