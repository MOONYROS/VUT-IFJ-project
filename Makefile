EXEC := gigachad_compiler

#Zde pridavejte zdrojaky chlapi
SRCS := main.c lex.c
OBJS := main.o lex.o

CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -Wpedantic -c

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $^ -o $@

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $^
	
clean:
	rm -vf *.o
	rm -f gigachad_compiler
