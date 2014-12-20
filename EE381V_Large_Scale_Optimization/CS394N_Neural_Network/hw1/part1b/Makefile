

RM=rm -f
CC=cc

#	Compiler Flags, Include, Library Paths
#	--------------------------------------
#
# Use CFLAGS = -g if you want to use a debugger, otherwise,
# use CFLAGS = O , which makes the program about twice as fast.
CFLAGS=  -g
INCLUDES= -I/usr/include -I/lusr/X11/include -I/p/include

# make xsofm gives program with X-window display
# make sofm gives program without display.

DEFINES1 = -DXWINDOW -D_NO_PROTO
LIBS1=  -lXm -lXt -lX11 -lXext -lm -L/p/lib -L/lusr/X11/lib -L/usr/lib

DEFINES2 = -D_NO_PROTO
LIBS2=  -lm -L/usr/lib


#############################################################################
# xsofm

xsofm: mainx.o geninp.o xdraw.o xdisplay.o
	$(RM) $@
	$(CC) -o $@ $(CFLAGS) mainx.o geninp.o xdraw.o xdisplay.o $(LIBS1)
############################################################################

xdisplay.o: xdisplay.c sofmdefs.h
	$(CC) -c  $(CFLAGS) $(DEFINES1) $(INCLUDES) xdisplay.c
#############################################################################

mainx.o: main.c sofmdefs.h
	$(CC) -c $(CFLAGS) $(DEFINES1) $(INCLUDES) main.c
	mv main.o mainx.o
#############################################################################

geninp.o: geninp.c sofmdefs.h
	$(CC) -c $(CFLAGS) $(DEFINES1) $(INCLUDES) geninp.c
#############################################################################

xdraw.o: xdraw.c sofmdefs.h
	$(CC) -c  $(CFLAGS) $(DEFINES1) $(INCLUDES) xdraw.c

#############################################################################
sofm: main.o geninp.o
	$(RM) $@
	$(CC) -o $@ $(CFLAGS) main.o geninp.o $(LIBS2)
############################################################################
main.o: main.c sofmdefs.h
	$(CC) -c $(CFLAGS) $(DEFINES2) $(INCLUDES) main.c
#############################################################################

clean:
	$(RM) *.o *~ xsofm sofm core
