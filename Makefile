CC=gcc
FLAGS=-Wall -O2 -g
SOURCE=./src/*.c


#test_arguments:
#	gcc $(FLAGS) -D TEST_ARGUMENTS -o exe_test_arguments $(SOURCE)

test_imanager:
	gcc $(FLAGS) -D ITEM_MANAGER -o ma $(SOURCE)
