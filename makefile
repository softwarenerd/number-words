VPATH  = src
ODIR   = ./obj
CC     = gcc
CFLAGS = -Wall
_OBJ   = main.o
OBJ    = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

number-words: $(OBJ)
	gcc -o $@ $^ 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o number-words