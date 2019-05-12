CC=cc
FLAGS=-Wall -Wextra -O2 -g -lm -Wno-unused-function
SOURCE=./src/*.c
IMAN=ma
SERVER=sv
CLIENT=cv
AGGREGATOR=ag
ARGS=arg_test
PPRINTER=pp
PIPE_PATH=PIPES/
VERBOSITY=VERBOSE_2

all:
	make imanager
	make server
	make client
	make aggregator
	make pprinter

imanager:
	$(CC) $(FLAGS) -D _COMPILE_IMAN -D $(VERBOSITY) -o $(IMAN) $(SOURCE)

server:
	$(CC) $(FLAGS) -D _COMPILE_SERVER -D $(VERBOSITY) -o $(SERVER) $(SOURCE)
	
client:
	$(CC) $(FLAGS) -D _COMPILE_CLIENT -D $(VERBOSITY) -o $(CLIENT) $(SOURCE)

aggregator:
	$(CC) $(FLAGS) -D _COMPILE_AGGREGATOR -D $(VERBOSITY) -o $(AGGREGATOR) $(SOURCE)

test_arguments:
	$(CC) $(FLAGS) -D TEST_ARGUMENTS -o $(ARGS) $(SOURCE)

pprinter:
	$(CC) $(FLAGS) -D _COMPILE_PPRINTER -D $(VERBOSITY) -o $(PPRINTER) $(SOURCE)


clean:
	rm -rf ARTIGOS STRINGS VENDAS STOCKS

clean_full:
	make clean
	rm -rf $(IMAN) $(ARGS) $(SERVER) $(CLIENT) $(PPRINTER) $(PIPE_PATH) 2019-*
