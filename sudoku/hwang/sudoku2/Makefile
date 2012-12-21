COMPILE = g++ -Wall -c -v -g
LINK = g++ -v

RHOME = /home/isd/kramer/reason

bin/sudoku: ofiles/sudoku.o
	$(LINK) -o bin/sudoku ofiles/sudoku.o

ofiles/sudoku.o: source/sudoku.c
	$(COMPILE) -o ofiles/sudoku.o source/sudoku.c

# stuff after this is for Centerline

load_sudoku:
	echo "Loading sudoku source into Centerline"
	#setopt program_name sudoku
	#load source/sudoku.c -lm

