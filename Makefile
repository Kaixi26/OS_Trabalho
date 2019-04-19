CC=gcc
FLAGS=-Wall -O2 -g
SOURCE=./src/*.c
IMAN=ma
ARGS=arg_test

imanager:
	gcc $(FLAGS) -D ITEM_MANAGER -o $(IMAN) $(SOURCE)

test_arguments:
	gcc $(FLAGS) -D TEST_ARGUMENTS -o $(ARGS) $(SOURCE)

clean_full:
	rm -rf ARTIGOS STRINGS $(IMAN) $(ARGS)
