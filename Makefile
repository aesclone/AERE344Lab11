CVER = -std=c99
IN = -Iinclude
LIBCSV = -Llib -lcsv
CC = gcc

all: script

temp:
	mkdir temp

script.o: Script.c include/csv.h temp
	$(CC) $(CVER) $(IN) -c Script.c -g -o temp/script.o

script: script.o lib/libcsv.so
	$(CC) temp/script.o $(LIBCSV) -lm -g -o script

clean:
	rm -r temp
	rm -f script
