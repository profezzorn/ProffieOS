all: fontconvert

CC     = gcc
CFLAGS = -g -Wall -I/usr/local/include/freetype2 -I/usr/include/freetype2 -I/usr/include
LIBS   = -lfreetype

fontconvert: fontconvert.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f fontconvert
