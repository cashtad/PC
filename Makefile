CC = gcc
CFLAGS = -Wall -lm

SRC = err.c draw_utils.c evaluator.c parser.c lexer.c main.c limits.c
EXEC = graph

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) -o $(EXEC) $(SRC) $(CFLAGS)

clean:
	del $(EXEC)