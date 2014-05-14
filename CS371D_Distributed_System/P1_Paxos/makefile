SHELL=/bin/sh
BIN="bin"

JC = javac
FLAGS = -d $(BIN) -cp $(BIN)
SOURCES = $(shell echo src/*.java)

all: 
	$(JC) $(FLAGS) $(SOURCES)

clean:
	rm -rf bin/*.class
