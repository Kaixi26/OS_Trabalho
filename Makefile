CC=cc
FLAGS=-Wall -O2 -g -lm -Wno-unused-function
SOURCE=./src/*.c
IMAN=ma
SERVER=sv
ARGS=arg_test
PPRINTER=pp
VERBOSITY=VERBOSE_2

all:
	make imanager
	make server

imanager:
	$(CC) $(FLAGS) -D ITEM_MANAGER -D $(VERBOSITY) -o $(IMAN) $(SOURCE)

server:
	$(CC) $(FLAGS) -D SERVER -D $(VERBOSITY) -o $(SERVER) $(SOURCE)

test_arguments:
	$(CC) $(FLAGS) -D TEST_ARGUMENTS -o $(ARGS) $(SOURCE)

pprinter:
	$(CC) $(FLAGS) -D _COMPILE_PPRINTER -D $(VERBOSITY) -o $(PPRINTER) $(SOURCE)


clean:
	rm -rf ARTIGOS STRINGS VENDAS STOCKS

clean_full:
	make clean
	rm -rf $(IMAN) $(ARGS)
