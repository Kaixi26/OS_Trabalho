CC=gcc
FLAGS=-Wall -Wextra -O2
SOURCE=./src/*.c
IMAN=ma
SERVER=sv
CLIENT=cv
AGGREGATOR=ag
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

pprinter:
	$(CC) $(FLAGS) -D _COMPILE_PPRINTER -D $(VERBOSITY) -o $(PPRINTER) $(SOURCE)

clean:
	rm -rf ARTIGOS STRINGS VENDAS STOCKS 2019-*
	rm -rf $(IMAN) $(SERVER) $(CLIENT) $(PPRINTER) $(PIPE_PATH)
