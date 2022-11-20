EXEC := gigachad_compiler

#Zde pridavejte zdrojaky chlapi
SRCS := main.c lex.c parser.c support.c
OBJS := main.o lex.o parser.o support.h

CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -Wpedantic -c

.PHONY: all run clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $^ -o $@

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $^

run: $(EXEC)
	./$(EXEC)
	
clean:
	rm -vf *.o
	rm -f gigachad_compiler
