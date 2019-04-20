CC=cc
FLAGS=-Wall -O2 -g
SOURCE=./src/*.c
IMAN=ma
ARGS=arg_test
VERBOSITY=VERBOSE_2

imanager:
	$(CC) $(FLAGS) -D ITEM_MANAGER -D $(VERBOSITY) -o $(IMAN) $(SOURCE)

test_arguments:
	$(CC) $(FLAGS) -D TEST_ARGUMENTS -o $(ARGS) $(SOURCE)

clean_full:
	rm -rf ARTIGOS STRINGS $(IMAN) $(ARGS)
