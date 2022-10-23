EXEC := main#Pozdeji se vytvori sofistikovanejsi nazev
SRCS := $(EXEC).c lex.c#Zde pridavejte zdrojaky chlapi
HEAD := lex.h token.h#Tady headery
CC := gcc
CFLAGS := -std=c99 -Wall -Werror -Wextra -Wpedantic

all: $(EXEC)

$(EXEC): $(EXEC).o

$(EXEC).o: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@
	
