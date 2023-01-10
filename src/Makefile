# ----------------------------------------------------------------------------
# PROJEKT do predmetu IFJ a IAL
# Encoding:  ISO-8859-2
# Datum:     2010/14/11
# Autor:     David Molnár, xmolna02
# Nazev:     Implementace interpretu imperativniho jazyka IFJ10
# varianta:  Tym 6, varianta b/2/I
# Popis:     Makefile pro sestaveni celeho projektu
# ----------------------------------------------------------------------------

# make - sestaveni celeho projektu
# make clean - smazeni souburu *.o a spustitelni program

EXECUTABLE = ifj10_interpret

SOURCES = i_list.c ial.c interpret.c main.c parser.c precedence_parser.c \
          scaner.c str.c table.c
			
OBJECTS = $(SOURCES:.c=.o)

CC = gcc

CFLAGS = -std=c99 -Wall -pedantic

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE)

.PHONY: clean
clean:
	rm -f *.o $(EXECUTABLE) *~

#
