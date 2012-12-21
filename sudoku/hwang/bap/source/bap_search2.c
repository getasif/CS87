/*************************************************************************/

/* comments

This file solves battleships problems by search. No ship parts
are constructed, and no water is constructed. Ship parts are marked
by guessing.

As compared with the bap_search1 solver, the following functions have
been removed that construct water:

fill_bap
legal_col_beside
legal_row_beside
make_water
pad_bap
pad_square

Tests that require water to be present (in mark dones) have been
removed the code.

The legal_row_across and legal_col_across functions weed out bad
guesses earlier by checking whether any strip going across or next to
the test strip would have an illegal condition if the guess were to be
inserted and rejecting the guess if so. This file may be used with and
without these functions commented out.

*/

/*************************************************************************/

/* includes

*/

#include <stdio.h>
#include <math.h>
#include <string.h>

/*************************************************************************/

/* hash_defs

*/

#define AND       &&
#define IS        ==
#define ISNT      !=
#define NOT       !
#define OR        ||
#define SET_TO    =
#define OK        0
#define ERROR     -1
#define TEXT_SIZE 128
#define MAX_NUMBER_ROWS 20
#define MAX_NUMBER_COLS 20
#define MAX_GUESSES 200

/*************************************************************************/

/* hash_macros

CHB prints the error message. CHK does not.

CHB is used both for bugs and for things that make the file unreadable.
Bug messages all begin with "Bug".

*/

#define CHK(testbad, error_msg) if (testbad){ \
  return ERROR; }                             \
  else

#define CHB(testbad, error_msg) if (testbad){ \
   printf("%s: %s\n", name, error_msg);       \
   return ERROR; }                            \
  else

#define IFF(tryit) if (tryit ISNT OK){ \
   return ERROR; }                     \
  else

/*************************************************************************/

/* structs

A strip is a row or column.
Need to initialize strips. All attributes should have value 0.

The locations array is indexed by puzzle row and column. The integer
at locations[row][col] is positive if a ship lies in that row and
has its left end at that column. The value is the length of the
ship. The integer is negative if a ship lies in that column and has
its top end at that row. The value is the negative of the length of
the ship. Otherwise, the integer is zero. Currently, all the tests
of the locations array only check for zero or non-zero.

*/

struct guess
{
  int row;     /* row at left or top of ship                         */
  int col;     /* column at left or top of ship                      */
  int size;    /* length of ship                                     */
  int in_row;  /* non-zero means ship lies in row, zero means column */
};

struct strip
{
  int aim;          /* number of squares to fill altogether    */
  int done;         /* set to 1 if strip is done, 0 otherwise  */
  int got;          /* number of squares filled so far         */
};

struct bap_world
{
  int battleships_left;           /* number battleships unlocated            */
  struct strip cols[MAX_NUMBER_COLS]; /* data on columns                     */
  int cols_in[MAX_NUMBER_COLS];   /* columns on which progress made          */
  int cruisers_left;              /* number cruisers unlocated               */
  int destroyers_left;            /* number destroyers unlocated             */
  int find_all;                   /* set to 1 if all answers to be found     */
  struct guess guesses_tried[MAX_GUESSES]; /* guesses already tried          */
  int locations[MAX_NUMBER_COLS][MAX_NUMBER_ROWS]; /* ship end locations     */
  char logic[TEXT_SIZE][TEXT_SIZE]; /* explanation of changes in puzzle      */
  int logic_line;                 /* number of next line of logic to write   */
  int number_cols;                /* number of columns in puzzle             */
  int number_cols_in;             /* number of colums on which progress made */
  int number_guesses_tried;       /* number of guesses tried                 */
  int number_rows;                /* number of rows in puzzle                */
  int number_rows_in;             /* number of rows on which progress made   */
  int print_all;                  /* set to 1 for printing partial solutions */
  int progress;                   /* set to 1 if progress made by solve loop */
  struct strip rows[MAX_NUMBER_ROWS]; /* data on rows                        */
  int rows_in[MAX_NUMBER_ROWS];   /* rows on which progress made             */
  char squares[MAX_NUMBER_COLS][MAX_NUMBER_ROWS]; /* array of squares        */
  int submarines_left;            /* number submarines unlocated             */
  int undone;                     /* number of strips not done               */
};

/*************************************************************************/

/* declare_functions

*/

int allocate_ship_col(int row, int col, int length, struct bap_world * world);
int allocate_ship_row(int row, int col, int length, struct bap_world * world);
int already_tried(int row, int col, int size, int in_row,
		  int * is_legal, struct bap_world * world);
int copy_bap(struct bap_world * world_copy, struct bap_world * world);
int copy_guess(struct guess * guess_copy, struct guess * guess_original);
int copy_guesses(struct guess * guesses_copy, struct guess * guesses,
		 int number_guesses);
int copy_strip(struct strip * strip_copy, struct strip * strip_old);
int explain_bap(struct bap_world * world);
int get_bap(char * file_name, struct bap_world * world);
int init_bap(struct bap_world * world);
int insert_guess_col(struct guess * the_guess, struct bap_world * world);
int insert_guess_row(struct guess * the_guess, struct bap_world * world);
int insert_guess_square(int row, int col,
			char item1, struct bap_world * world);
int legal_col_across(char * test, int * is_legal, struct bap_world * world);
int legal_row_across(char * test, int * is_legal, struct bap_world * world);
int locate_ships(struct bap_world * world);
int make_guesses_ships(int size, int * number_guesses,
			     struct guess * guesses, struct bap_world * world);
int make_guess_ship_col(int size, int col, int * number_guesses, int limit,
			struct guess * guesses, struct bap_world * world);
int make_guess_ship_row(int size, int row, int * number_guesses, int limit,
			struct guess * guesses, struct bap_world * world);
int mark_dones(struct bap_world * world);
int print_bap(struct bap_world * world);
int read_arguments(int argc, char * argv[], struct bap_world * world);
int read_column_numbers(FILE * in_port, char * buffer,
			struct bap_world * world);
int read_dashes(FILE * in_port, char * dashes, int * row_length,
		struct bap_world * world);
int read_row(char * buffer, struct bap_world * world);
int read_rows(FILE * in_port, char * buffer, int row_length,
	      struct bap_world * world);
int read_ships(FILE * in_port, char * buffer, struct bap_world * world);
int record_progress(char * tried, struct bap_world * world);
int ship_fits_col_row(int col, int row, int size,
		      int * ys, int * is_legal, struct bap_world * world);
int ship_fits_row_col(int row, int col, int size,
		      int * ys, int * is_legal, struct bap_world * world);
int solve_bap(struct bap_world * world);
char square(int row, int col, struct bap_world * world);
int submarine_covers(int row, int col, int * cover, struct bap_world * world);
int try_guessing(struct bap_world * world);
int verify_square(char item, int row, int col, struct bap_world * world);
int verify_squares(struct bap_world * world);

/*************************************************************************/

/* declare_globals

*/

/*************************************************************************/
/*************************************************************************/

/* define_functions

*/

/*************************************************************************/

/* allocate_ship_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The ship is a submarine: "Bug cannot allocate submarines to columns".
  2. The ship is a destroyer, but there are no destroyers left to allocate:
     "Puzzle has no answer too many destroyers".
  3. The ship is a cruiser, but there are no cruisers left to allocate:
     "Puzzle has no answer too many cruisers".
  4. The ship is a battleship, but there are no battleships left to allocate:
     "Puzzle has no answer too many battleships".
  5. The length of the ship is not 1, 2, 3, or 4:
     "Puzzle has no answer bad ship length".

Called By:  locate_ships

Side Effects: This allocates a ship to a column. This checks that
there is at least one ship of the given type left to be located and
subtracts one from the number of such ships.

This also records the location of the top end of the ship.

The "Bad ship length" message will be given if a ship more than
four squares long is given in the problem statement.

*/

int allocate_ship_col(     /* ARGUMENTS                 */
 int row,                  /* row of top end of ship    */
 int col,                  /* column of top end of ship */
 int length,               /* length of ship            */
 struct bap_world * world) /* puzzle model              */
{
  static char name[] SET_TO "allocate_ship_col";
  int newby;

  newby SET_TO 1;
  if (NOT(world->locations[row][col]))
    {
      if (world->number_guesses_tried > 0)
	{
	  IFF(already_tried(row, col, length, 0, &newby, world));
	}
      if (newby)
	{      switch(length)
	  {
	  case 2:
	    CHK((world->destroyers_left < 1),
		"Puzzle has no answer too many destroyers");
	    world->locations[row][col] SET_TO -2;
	    world->destroyers_left--;
	    break;
	  case 3:
	    CHK((world->cruisers_left < 1),
		"Puzzle has no answer too many cruisers");
	    world->locations[row][col] SET_TO -3;
	    world->cruisers_left--;
	    break;
	  case 4:
	    CHK((world->battleships_left < 1),
		"Puzzle has no answer too many battleships");
	    world->locations[row][col] SET_TO -4;
	    world->battleships_left--;
	    break;
	  case 1:
	    CHB(1, "Bug cannot allocate submarines to columns");
	    break;
	  default:
	    CHK(1, "Puzzle has no answer bad ship length");
	  }
	world->progress SET_TO 1;
	}
    }
  CHK((newby IS 0), "All possibilities already tried");  
  return OK;
}

/*************************************************************************/

/* allocate_ship_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The ship is a submarine, but there are no submarines left to allocate:
     "Puzzle has no answer too many submarines".
  2. The ship is a destroyer, but there are no destroyers left to allocate:
     "Puzzle has no answer too many destroyers".
  3. The ship is a cruiser, but there are no cruisers left to allocate:
     "Puzzle has no answer too many cruisers".
  4. The ship is a battleship, but there are no battleships left to allocate:
     "Puzzle has no answer too many battleships".
  5. The length of the ship is not 1, 2, 3, or 4:
     "Puzzle has no answer bad ship length".

Called By:  locate_ships

Side Effects: This allocates a ship to a row. This checks that there
is at least one ship of the given type left to be located and
subtracts one from the number of such ships.

Notes:

Submarines lie fully in a row and a column. Other ship types lie fully
in either a row or a column but not both. To prevent double counting
of submarines, submarines may be allocated only to rows. This is
a limiting decision and should be changed, if convenient. The simplest
change would be to allow either rows or columns, but not both.

This sets world->progress to 1.

The "Bad ship length" message will be given if a ship more than
four squares long is given in the problem statement.

*/

int allocate_ship_row(     /* ARGUMENTS                  */
 int row,                  /* row of left end of ship    */
 int col,                  /* column of left end of ship */
 int length,               /* length of ship             */
 struct bap_world * world) /* puzzle model               */
{
  static char name[] SET_TO "allocate_ship_row";
  int newby;

  newby SET_TO 1;
  if (NOT(world->locations[row][col]))
    {
      if (world->number_guesses_tried > 0)
	{
	  IFF(already_tried(row, col, length, 1, &newby, world));
	}
      if (newby)
	{
	  switch(length)
	    {
	    case 1:
	      CHK((world->submarines_left < 1), 
		  "Puzzle has no answer too many submarines");
	      world->locations[row][col] SET_TO 1;
	      world->submarines_left--;
	      break;
	    case 2:
	      CHK((world->destroyers_left < 1),
		  "Puzzle has no answer too many destroyers");
	      world->locations[row][col] SET_TO 2;
	      world->destroyers_left--;
	      break;
	    case 3:
	      CHK((world->cruisers_left < 1),
		  "Puzzle has no answer too many cruisers");
	      world->locations[row][col] SET_TO 3;
	      world->cruisers_left--;
	      break;
	    case 4:
	      CHK((world->battleships_left < 1),
		  "Puzzle has no answer too many battleships");
	      world->locations[row][col] SET_TO 4;
	      world->battleships_left--;
	      break;
	    default:
	      CHK(1, "Bad ship length");
	    }
	  world->progress SET_TO 1;
	}
    }
  CHK((newby IS 0), "All possibilities already tried");
  return OK;
}

/*************************************************************************/

/* already_tried

Returned Value: int

Called By:
 allocate_ship_col
 allocate_ship_row
 make_guess_ship_col
 make_guess_ship_row

Notes:

*/

int already_tried(         /* ARGUMENTS              */
 int row,                  /* row to check           */
 int col,                  /* column to check        */
 int size,                 /* size of ship to check  */
 int in_row,               /* 1 = ship is in a row   */
 int * newby,              /* set to 0 here if tried */
 struct bap_world * world) /* puzzle model           */
{
  static char name[] SET_TO "already_tried";
  int n;
  
  *newby SET_TO 1;
  for (n SET_TO 0; n < world->number_guesses_tried; n++)
    {
      if ((world->guesses_tried[n].row IS row) AND
	  (world->guesses_tried[n].col IS col) AND
	  (world->guesses_tried[n].size IS size) AND
	  (world->guesses_tried[n].in_row IS in_row))
	{
	  *newby SET_TO 0;
	  break;
	}
    }
  return OK;
}

/*************************************************************************/

/* copy_bap

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. copy_strip returns ERROR.

Called By: try_guessing

Side Effects: This copies the world model into the world_copy model,
except that rows_in and cols_in are not copied.

*/

int copy_bap(                   /* ARGUMENTS                      */
 struct bap_world * world_copy, /* puzzle model being copied into */
 struct bap_world * world)      /* puzzle model being copied from */
{
  static char name[] SET_TO "copy_bap";
  int row;         /* index of row being processed         */
  int col;         /* index of column being processed      */
  int n;           /* index of other array being processed */
  
  world_copy->battleships_left SET_TO world->battleships_left;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      IFF(copy_strip(&(world_copy->cols[col]), &(world->cols[col])));
    }
  world_copy->cruisers_left SET_TO world->cruisers_left;
  world_copy->destroyers_left SET_TO world->destroyers_left;
  world_copy->find_all SET_TO world->find_all;
  for (n SET_TO 0; n < world->logic_line; n++)
    {
      strcpy(world_copy->logic[n], world->logic[n]);
    }
  IFF(copy_guesses(world_copy->guesses_tried, world->guesses_tried,
		   world->number_guesses_tried));
  world_copy->logic_line SET_TO world->logic_line;
  world_copy->number_cols SET_TO world->number_cols;
  world_copy->number_cols_in SET_TO 0;
  world_copy->number_guesses_tried SET_TO world->number_guesses_tried;
  world_copy->number_rows SET_TO world->number_rows;
  world_copy->number_rows_in SET_TO 0;
  world_copy->print_all SET_TO world->print_all;
  world_copy->progress SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      IFF(copy_strip(&(world_copy->rows[row]), &(world->rows[row])));
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  world_copy->squares[row][col] SET_TO world->squares[row][col];
	  world_copy->locations[row][col] SET_TO world->locations[row][col];
	}
    }
  world_copy->submarines_left SET_TO world->submarines_left;
  world_copy->undone SET_TO world->undone;
  return OK;
}

/*************************************************************************/

/* copy_guess

Returned Value: int (OK)

Called By:
 copy_guesses
 try_guessing

Side Effects: This copies the guess_original into the guess_copy.

*/

int copy_guess(                 /* ARGUMENTS          */
 struct guess * guess_copy,     /* guess to copy into */
 struct guess * guess_original) /* guess to copy from */
{
  static char name[] SET_TO "copy_guess";

  guess_copy->row SET_TO guess_original->row;
  guess_copy->col SET_TO guess_original->col;
  guess_copy->size SET_TO guess_original->size;
  guess_copy->in_row SET_TO guess_original->in_row;
  return OK;
}

/*************************************************************************/

/* copy_guesses

Returned Value: int (OK)

Called By: make_guesses

Side Effects: This copies the first number_guesses entries from the
guesses array into the guesses_copy array.

*/

int copy_guesses(             /* ARGUMENTS                     */
 struct guess * guesses_copy, /* array of guesses to copy into */
 struct guess * guesses,      /* array of guesses to copy from */
 int number_guesses)          /* number of guesses to copy     */
{
  static char name[] SET_TO "copy_guesses";
  int n; /* index for guesses          */

  for (n SET_TO 0; n < number_guesses; n++)
    {
      copy_guess(&(guesses_copy[n]), &(guesses[n]));
    }
  return OK;
}

/*************************************************************************/

/* copy_strip

Returned Value: int (OK)

Called By: copy_bap

Side Effects: This copies the strip_old into the strip_copy.

*/

int copy_strip(             /* ARGUMENTS          */
 struct strip * strip_copy, /* strip to copy into */
 struct strip * strip_old)  /* strip to copy from */
{
  static char name[] SET_TO "copy_strip";
  strip_copy->aim SET_TO strip_old->aim;
  strip_copy->done SET_TO strip_old->done;
  strip_copy->got SET_TO strip_old->got;
  return OK;
}

/*************************************************************************/

/* explain_bap

Returned Value: int (OK)

Called By:  solve_bap

Side Effects: This prints the explanation of the steps in solving the
puzzle, which has been accumulated in the world->logic array of strings.

*/

int explain_bap(           /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  static char name[] SET_TO "explain_bap";
  int line;      /* index of logic line */
  
  printf("\nEXPLANATION\n-----------\n");
  for (line SET_TO 0; line < world->logic_line; line++)
    printf("%s\n", world->logic[line]);
  return OK;
}

/*************************************************************************/

/* get_bap

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The named file cannot be opened: "Could not open file".
  2. The file ends before the first blank line:
     "File ends before first blank line".
  3. After read_rows returns, the next line is not a line of dashes identical
     to the first line of dashes: "Bad row or second line of dashes".
  4. Any of the following functions returns ERROR:
     print_bap
     read_column_numbers
     read_dashes
     read_rows
     read_ships

Called By:  main

Side Effects: The puzzle in the file named file_name is read and stored.

Notes:

The executable compiled from this code is expected to be named bap.
The executable should have one command argument, the name of a file
containing the external representation of the problem. The file
represents an M columns by N rows rectangle to be filled with an
arbitrary collection of ships of lengths 4, 3, 2, and 1. The file must
have all of the following in the order given:

1. a blank line (above which everything is read and ignored).
2. a row with one space followed by alternate spaces and dashes; each dash
   indicates a column, so the number of dashes is the number of columns,
   which must be between 1 and 99.
3. 1 to 99 rows each consisting of two spaces followed by |stuff|, followed
   by an integer between 0 and the number of columns. The stuff is a
   mixture of blanks, ship parts and water, with appropriate spacing so
   water and ship parts occur in the proper column. water is ., ship end
   is one of v<^>, ship middle is +, submarine is O (capital letter O),
   ship part of not fully determined type is X (which does not occur
   in standard battleships).
4. a row identical to the one described in item 2, above.
5. a row with two spaces followed by numbers, each of which is either
   a one-digit number between 0 and 9 followed by a blank or a two-digit
   number between 10 and 99. The number gives the number of squares to be
   be filled in the column above it.
6. a blank line
7. 1 to four rows of any length starting with two blanks and containing
   ships. All the ships in a row are the same size. Larger ships are in
   rows above smaller ships. Ships are length 4, 3, 2, or 1. Two blanks
   separate ships in a row.
8. The word "end" on a row by itself. Lines may occur below this but will
   not be read.


Here is a sample file between rows of stars.

****************************************************
This is a sample problem file.

  - - - - - - - - - -
 |                   |1
 |                   |1
 |                   |2
 |                  O|1
 |                   |3
 |                   |3
 |                   |2
 |                   |6
 |                  O|1
 |                   |0
  - - - - - - - - - -
  2 0 2 4 1 3 2 2 0 4

  <++>
  <+>  <+>
  <>  <>  <>
  O  O  O  O
end

****************************************************

The reading procedure is:
1. Open the file and check that it opened.
2. Look for a blank line. Read and ignore everything before the blank line.
3. Read the first line of dashes. This gives the number of columns.
4. Read the rows.
5. Read the second line of dashes.
6. Read the column numbers.
7. Read the ships.
8. Close the file.

Anything before the first blank line is a header which is intended to
contain explanatory text, but could be anything.

*/

int get_bap(               /* ARGUMENTS                    */
 char * file_name,         /* name of file to read         */
 struct bap_world * world) /* puzzle model, filled in here */
{
  static char name[] SET_TO "get_bap";
  FILE * in_port;
  char dashes[TEXT_SIZE];
  char buffer[TEXT_SIZE];
  int row_length;

  in_port SET_TO fopen(file_name, "r");
  CHB((in_port IS NULL), "Could not open file");
  for(;;)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "File ends before first blank line");
      if(buffer[0] IS '\n')
	break;
    }
  IFF(read_dashes(in_port, dashes, &row_length, world));
  IFF(read_rows(in_port, buffer, row_length, world));
  CHB((strcmp(dashes, buffer) ISNT 0), "Bad row or second line of dashes");
  IFF(read_column_numbers(in_port, buffer, world));
  IFF(read_ships(in_port, buffer, world));
  world->undone SET_TO (world->number_cols + world->number_rows);
  fclose(in_port);
  strcpy(world->logic[world->logic_line++], "read problem");
  if (world->print_all)
    {
      IFF(print_bap(world));
      printf("%s\n\n", world->logic[world->logic_line - 1]);
    }
  return OK;
}

/*************************************************************************/

/* init_bap

Returned Value: int (OK)

Called By:  main

Side Effects: This sets all attributes in the world model to zero or NULL,
except those that do not need initialization because they are set later.

Notes:

The following attributes of world need no initialization
cols_in
guesses_tried
logic
number_cols_in
number_rows_in
rows
squares
rows_in
undone

The following attributes of world are already initialized when this is called
find_all
print_all

*/

int init_bap(              /* ARGUMENTS                     */
 struct bap_world * world) /* puzzle model, filled in here  */
{
  static char name[] SET_TO "init_bap";
  int row;
  int col;

  world->battleships_left SET_TO 0;
  for (col SET_TO 0; col < MAX_NUMBER_COLS; col++)
    {
      world->cols[col].aim SET_TO 0;
      world->cols[col].done SET_TO 0;
      world->cols[col].got SET_TO 0;
    }
  world->cruisers_left SET_TO 0;
  world->destroyers_left SET_TO 0;
  world->logic_line SET_TO 0;
  world->number_cols SET_TO 0;
  world->number_guesses_tried SET_TO 0;
  world->number_rows SET_TO 0;
  world->progress SET_TO 0;
  for (row SET_TO 0; row < MAX_NUMBER_ROWS; row++)
    {
      world->rows[row].aim SET_TO 0;
      world->rows[row].done SET_TO 0;
      world->rows[row].got SET_TO 0;
      for (col SET_TO 0; col < MAX_NUMBER_COLS; col++)
	{
	  world->locations[row][col] SET_TO 0;
	}
    }
  world->submarines_left SET_TO 0;
  return OK;
}

/*************************************************************************/

/* insert_guess_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. A row or column is out of bounds: "Bug bad guess location".
  2. The size of the guess is not 1, 2, 3, or 4:"Bug bad guess size".
  3. No progress is made when the guess is inserted:
     "Bug guess changed nothing".
  4. record_progress returns ERROR.
  5. insert_guess_square returns ERROR.

Called By:  try_guessing

Side Effects: This places a guess in a column of the puzzle.

This checks that each character under the guess is either blank or is
already what the guess says it should be.

*/

int insert_guess_col(      /* ARGUMENTS            */
 struct guess * the_guess, /* guess to be inserted */
 struct bap_world * world) /* puzzle model         */
{
  static char name[] SET_TO "insert_guess_col";
  int n;
  int row;
  int col;
  char ship_part;
  char buffer[TEXT_SIZE];

  row SET_TO the_guess->row;
  col SET_TO the_guess->col;
  CHB(((row < 0) OR (row >= world->number_rows) OR
       (col < 0) OR (col >= world->number_cols)),
      "Bug bad guess location");
  if (the_guess->size IS 1)
    {
      strcpy(buffer, "guess submarine is at ");
      IFF(insert_guess_square(row, col, 'O', world));
    }
  else if (the_guess->size IS 2)
    {
      strcpy(buffer, "guess destroyer is in column at ");
      IFF(insert_guess_square( row,      col, '^', world));
      IFF(insert_guess_square((row + 1), col, 'v', world));
    }
  else if (the_guess->size IS 3)
    {
      strcpy(buffer, "guess cruiser is in column at ");
      IFF(insert_guess_square( row,      col, '^', world));
      IFF(insert_guess_square((row + 1), col, '+', world));
      IFF(insert_guess_square((row + 2), col, 'v', world));
    }
  else if (the_guess->size IS 4)
    {
      strcpy(buffer, "guess battleship is in column at ");
      IFF(insert_guess_square( row,      col, '^', world));
      IFF(insert_guess_square((row + 1), col, '+', world));
      IFF(insert_guess_square((row + 2), col, '+', world));
      IFF(insert_guess_square((row + 3), col, 'v', world));
    }
  else
    CHB(1, "Bug bad guess size");
  
  CHB((world->progress IS 0), "Bug guess changed nothing");
  world->cols_in[0] SET_TO col;
  world->number_cols_in SET_TO 1;
  world->rows_in[0] SET_TO row;
  world->number_rows_in SET_TO 1;
  IFF(record_progress(buffer, world));
  return OK;
}

/*************************************************************************/

/* insert_guess_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. A row or column is out of bounds: "Bug bad guess location".
  2. The size of the guess is not 1, 2, 3, or 4:"Bug bad guess size".
  3. No progress is made when the guess is inserted:
     "Bug guess changed nothing".
  4. record_progress returns ERROR.
  5. insert_guess_square returns ERROR.

Called By:  try_guessing

Side Effects: This places a guess in a row of the puzzle.

This checks that each character under the guess is either blank or is
already what the guess says it should be.

*/

int insert_guess_row(      /* ARGUMENTS            */
 struct guess * the_guess, /* guess to be inserted */
 struct bap_world * world) /* puzzle model         */
{
  static char name[] SET_TO "insert_guess_row";
  int n;
  int row;
  int col;
  char ship_part;
  char buffer[TEXT_SIZE];

  row SET_TO the_guess->row;
  col SET_TO the_guess->col;
  CHB(((row < 0) OR (row >= world->number_rows) OR
       (col < 0) OR (col >= world->number_cols)),
      "Bug bad guess location");
  if (the_guess->size IS 1)
    {
      strcpy(buffer, "guess submarine is at ");
      IFF(insert_guess_square(row, col, 'O', world));
    }
  else if (the_guess->size IS 2)
    {
      strcpy(buffer, "guess destroyer is in row at ");
      IFF(insert_guess_square(row,  col,      '<', world));
      IFF(insert_guess_square(row, (col + 1), '>', world));
    }
  else if (the_guess->size IS 3)
    {
      strcpy(buffer, "guess cruiser is in row at ");
      IFF(insert_guess_square(row,  col,      '<', world));
      IFF(insert_guess_square(row, (col + 1), '+', world));
      IFF(insert_guess_square(row, (col + 2), '>', world));
    }
  else if (the_guess->size IS 4)
    {
      strcpy(buffer, "guess battleship is in row at ");
      IFF(insert_guess_square(row,  col,      '<', world));
      IFF(insert_guess_square(row, (col + 1), '+', world));
      IFF(insert_guess_square(row, (col + 2), '+', world));
      IFF(insert_guess_square(row, (col + 3), '>', world));
    }
  else
    CHB(1, "Bug bad guess size");
  
  CHB((world->progress IS 0), "Bug guess changed nothing");
  world->cols_in[0] SET_TO col;
  world->number_cols_in SET_TO 1;
  world->rows_in[0] SET_TO row;
  world->number_rows_in SET_TO 1;
  IFF(record_progress(buffer, world));
  return OK;
}

/*************************************************************************/

/* insert_guess_square

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The square to be set is neither blank nor X:
     "Bug bad character at guess location".

Called By:
  insert_guess_col
  insert_guess_row

*/

int insert_guess_square(   /* ARGUMENTS                     */
 int row,                  /* row index of square           */
 int col,                  /* column index of square        */
 char item1,               /* character to insert in square */
 struct bap_world * world) /* puzzle model                  */
{
  static char name[] SET_TO "insert_guess_square";
  char item2;
  
  item2 SET_TO world->squares[row][col];
  if (item2 IS item1);
  else if (item2 IS ' ')
    {
      world->squares[row][col] SET_TO item1;
      world->progress SET_TO 1;
      world->rows[row].got++;
      world->cols[col].got++;
    }
  else
    CHB(1, "Bug bad character at guess location");
  return OK;
}

/*************************************************************************/

/* legal_col_across

Returned Value: int (OK)

Called By:
  legal_col
  ship_fits_col_row

Side Effects: This sets *is_legal to 0 if the column is not legal.

Notes: 

Is_legal should be set to 1 when this is called.

Y is used to fill test rows so that the squares to be tested are not
confused with squares previously set to X.

*/

int legal_col_across(      /* ARGUMENTS                         */
 char * test,              /* column being tested               */
 int * is_legal,           /* set here to 0 if column not legal */
 struct bap_world * world) /* puzzle model                      */
{
  static char name[] SET_TO "legal_col_across";
  int row;

  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if((test[row] IS 'Y') AND
	 (world->rows[row].aim < (world->rows[row].got + 1)))
	{
	  *is_legal SET_TO 0;
	  break;
	}
    }
  return OK;
}

/*************************************************************************/

/* legal_row_across

Returned Value: int (OK)

Called By:
  legal_row
  ship_fits_row_col

Side Effects: This sets *is_legal to 0 if the row is not legal.

Notes:

Is_legal should be set to 1 when this is called.

Y is used to fill test rows so that the squares to be tested are not
confused with squares previously set to X.

*/

int legal_row_across(      /* ARGUMENTS                     */
 char * test,              /* the row being tested          */
 int * is_legal,           /* set here to 0 if not legal    */
 struct bap_world * world) /* puzzle model                  */
{
  static char name[] SET_TO "legal_row_across";
  int col;

  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if((test[col] IS 'Y') AND
	 (world->cols[col].aim < (world->cols[col].got + 1)))
	{
	  *is_legal SET_TO 0;
	  break;
	}
    }
  return OK;
}

/*************************************************************************/

/* locate_ships

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. allocate_ship_col returns ERROR.
  2. allocate_ship_row returns ERROR.
  3. an illegal ship continuation is found: "Puzzle has no answer bad ship".

Called By:  solve_bap

Side Effects: This reads the rows and columns looking for whole ships
with no X's. Each whole ship is allocated by allocate_ship_row or
allocate_ship_col.

Submarines are allocated from rows in this function but not from
columns, since they should be allocated only once.

*/

int locate_ships(          /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  static char name[] SET_TO "locate_ships";
  int row;
  int col;
  int reading_ship; /* 1=have started reading a ship, 0=not */
  int length;       /* length of ship being read */
  
  reading_ship SET_TO 0;
  length SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if (world->rows[row].done)
	continue;
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  if (reading_ship)
	    {
	      switch(square(row, col, world))
		{
		case '>':
		  IFF(allocate_ship_row(row, (col - length), length+1, world));
		  reading_ship SET_TO 0;
		  break;
		case '+':
		  length++;
		  break;
		case ' ':
		  reading_ship SET_TO 0;
		  break;
		default:
		  CHK(1, "Puzzle has no answer bad ship");
		  break;
		}
	    }
	  else
	    {
	      if (square(row, col, world) IS '<')
		{
		  reading_ship SET_TO 1;
		  length SET_TO 1;
		}
	      else if (square(row, col, world) IS 'O')
		IFF(allocate_ship_row(row, col, 1, world));
	    }
	}
    }
  reading_ship SET_TO 0;
  length SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if (world->cols[col].done)
	continue;
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  if (reading_ship)
	    {
	      switch(square(row, col, world))
		{
		case 'v':
		  IFF(allocate_ship_col((row - length), col, length+1, world));
		  reading_ship SET_TO 0;
		  break;
		case '+':
		  length++;
		  break;
		case ' ':
		  reading_ship SET_TO 0;
		  break;
		default:
		  CHK(1, "Puzzle has no answer bad ship");
		  break;
		}
	    }
	  else
	    {
	      if (square(row, col, world) IS '^')
		{
		  reading_ship SET_TO 1;
		  length SET_TO 1;
		}
	    }
	}
    }
  return OK;
}

/*************************************************************************/

/* main

This:
1. calls read_arguments to read the arguments.
2. calls init_bap to initialize the world puzzle model.
3. if needed, calls get_bap to read the problem.
4. calls solve_bap to solve the problem.

The IFFs (which contain "return" statements) stop execution in case an
error occurs (as intended) because in a "main" function, "return"
stops execution.

*/

main(
 int argc,
 char * argv[])
{
  static char name[] SET_TO "main";
  struct bap_world world;

  IFF(read_arguments(argc, argv, &world));
  IFF(init_bap(&world));
  IFF(get_bap(argv[1], &world));
  CHK((solve_bap(&world) ISNT OK), "Puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* make_guesses_ships

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. There is no place to put a ship of the given size: "Cannot solve puzzle".
  2. make_guess_ship_row returns ERROR.
  3. make_guess_ship_col returns ERROR.

Called By:  try_guessing

Side Effects: A set of guesses for ships of the given size is made and
stored.

Notes:

For submarines, only rows are used.

*/

int make_guesses_ships(     /* ARGUMENTS                    */
 int size,                  /* length of ship               */
 int * number_guesses,      /* number of guesses, set here  */
 struct guess * guesses,    /* a set of guesses, set here   */
 struct bap_world * world)  /* puzzle model                 */
{
  static char name[] SET_TO "make_guesses_ships";
  int row;
  int col;

  *number_guesses SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      IFF(make_guess_ship_row
	  (size, row, number_guesses, (MAX_GUESSES + 1), guesses, world));
    }
  if (size > 1)
    {
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  IFF(make_guess_ship_col
	      (size, col, number_guesses, (MAX_GUESSES + 1), guesses, world));
	}
      CHK((*number_guesses IS 0), "Cannot solve puzzle");
    }
  return OK;
}


/*************************************************************************/

/* make_guess_ship_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. size is less than 1 or greater than 4: "Bug bad size".
  2. *number_guesses is too large: "Too many alternatives".
  3. submarine_covers returns ERROR.
  4. ship_fits_col_row returns ERROR.

Called By:
  make_guesses_ships

Side Effects:

The current number of guesses is passed in and reset on the way out.
This is so the function can add guesses to an existing set of guesses.

Notes:

This stops making guesses when *number_guesses equals limit.

*/

int make_guess_ship_col(   /* ARGUMENTS                                   */
 int size,                 /* size of ship to make guesses for            */
 int col,                  /* index of column to make guesses in          */
 int * number_guesses,     /* number of guesses in set, may be reset here */
 int limit,                /* the maximum number to put in a set          */
 struct guess * guesses,   /* the guesses, may be reset here              */
 struct bap_world * world) /* puzzle model                                */
{
  static char name[] SET_TO "make_guess_ship_col";
  int row;
  int stop;
  int ys;
  int is_legal;
  int n;

  CHB(((size < 1) OR (size > 4)), "Bug bad size");
  n SET_TO *number_guesses;
  stop SET_TO (world->number_rows - (size - 1));
  for (row SET_TO 0; ((n < limit) AND (row < stop)); row++)
    {
      if (size IS 1)
	IFF(submarine_covers(row, col, &is_legal, world));
      else
	IFF(ship_fits_col_row(col, row, size, &ys, &is_legal, world));
      if ((is_legal) AND (NOT (world->locations[row][col])))
	{
	  IFF(already_tried(row, col, size, 0, &is_legal, world));
	  if (is_legal)
	    {
	      CHB((n > (MAX_GUESSES - 2)), "Bug too many alternatives");
	      guesses[n].size SET_TO size;
	      guesses[n].row SET_TO row;
	      guesses[n].col SET_TO col;
	      guesses[n].in_row SET_TO 0;
	      n++;
	    }
	}
    }
  *number_guesses SET_TO n;
  return OK;
}

/*************************************************************************/

/* make_guess_ship_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. size is less than 1 or greater than 4: "Bug bad size".
  2. *number_guesses is too large: "Too many alternatives".
  3. submarine_covers returns ERROR.
  4. ship_fits_row_col returns ERROR.

Called By:
  make_guesses_ships

Side Effects:

The current number of guesses is passed in and reset on the way out.
This is so the function can add guesses to an existing set of guesses.

Notes:

This stops making guesses when *number_guesses equals limit.

*/

int make_guess_ship_row(   /* ARGUMENTS                                   */
 int size,                 /* size of ship to make guesses for            */
 int row,                  /* index of row to make guesses in             */
 int * number_guesses,     /* number of guesses in set, may be reset here */
 int limit,                /* the maximum number to put in a set          */
 struct guess * guesses,   /* the guesses, may be reset here              */
 struct bap_world * world) /* puzzle model                                */
{
  static char name[] SET_TO "make_guess_ship_row";
  int col;
  int stop;
  int ys;
  int is_legal;
  int n;

  CHB(((size < 1) OR (size > 4)), "Bug bad size");
  n SET_TO *number_guesses;
  stop SET_TO (world->number_cols - (size - 1));
  for (col SET_TO 0; ((n < limit) AND (col < stop)); col++)
    {
      if (size IS 1)
	IFF(submarine_covers(row, col, &is_legal, world));
      else
	IFF(ship_fits_row_col(row, col, size, &ys, &is_legal, world));
      if ((is_legal) AND (NOT (world->locations[row][col])))
	{
	  IFF(already_tried(row, col, size, 1, &is_legal, world));
	  if (is_legal)
	    {
	      CHB((n > (MAX_GUESSES - 2)), "Bug too many alternatives");
	      guesses[n].size SET_TO size;
	      guesses[n].row SET_TO row;
	      guesses[n].col SET_TO col;
	      guesses[n].in_row SET_TO 1;
	      n++;
	    }
	}
    }
  *number_guesses SET_TO n;
  return OK;
}

/*************************************************************************/

/* mark_dones

Returned Value: int (OK)

Called By:  solve_bap

Side Effects: For each strip not already marked done in which aim and
got are equal, the strip is marked done and world->undone is decreased
by one.

Notes: Locate_ships should normally be run before this so that no
strip which has unlocated ships is marked done.

*/

int mark_dones(            /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  static char name[] SET_TO "mark_dones";
  int row;
  int col;

  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if (world->rows[row].done);
      else if (world->rows[row].aim IS world->rows[row].got)
	{
	  world->rows[row].done SET_TO 1;
	  world->undone--;
	}
    }
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if (world->cols[col].done);
      else if (world->cols[col].aim IS world->cols[col].got)
	{
	  world->cols[col].done SET_TO 1;
	  world->undone--;
	}
    }
  return OK;
}

/*************************************************************************/

/* print_bap

Returned Value: int (OK)

Called By:
  get_bap
  record_progress
  solve_bap

Side Effects: This prints the puzzle from the bap_world. The puzzle
is printed in the same format as required for reading in, except that
the list of ships and end marker are omitted.

*/

int print_bap(             /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  static char name[] SET_TO "print_bap";
  int row;
  int col;
  
  printf("\n ");
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      printf(" -");
    }
  printf("\n");
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      printf(" |");
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  printf("%c", world->squares[row][col]);
	  if (col < (world->number_cols - 1))
	    printf(" ");
	}
      printf("|%d\n", world->rows[row].aim);
    }
  printf(" ");
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      printf(" -");
    }
  printf("\n  ");
  for(col SET_TO 0; col < world->number_cols; col++)
    {
      printf("%d", world->cols[col].aim);
      if (world->cols[col].aim < 10)
	printf(" ");
    }
  printf("\n");
  
  return OK;
}

/*************************************************************************/

/* read_arguments

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. argc is not 4.
  2. argv[2] is not "all" or "one.
  3. argv[3] is not "yes" or "no".

Called By: main

Side Effects: The values of 

*/

int read_arguments(        /* ARGUMENTS                 */
 int argc,                 /* number of arguments       */
 char * argv[],            /* array of argument strings */
 struct bap_world * world) /* puzzle model              */
{
  static char name[] SET_TO "read_arguments";
  
  if ((argc IS 4) AND
      ((strcmp(argv[2], "all") IS 0) OR (strcmp(argv[2], "one") IS 0)) AND
      ((strcmp(argv[3], "no") IS 0)  OR (strcmp(argv[3], "yes") IS 0)))
    {
      if (strcmp(argv[2], "all") IS 0)
	world->find_all SET_TO 1;
      else
	world->find_all SET_TO 0;
      if (strcmp(argv[3], "yes") IS 0)
	world->print_all SET_TO 1;
      else
	world->print_all SET_TO 0;
    }
  else
    {
      printf("Usage: %s <file name> <all|one> <yes|no>\n", argv[0]);
      printf("all = find all solutions\n");
      printf("one = find one solution\n");
      printf("yes = print puzzle when done and when a rule makes progress\n");
      printf("no = print puzzle only when done\n");
      CHK(1,"");
    }
  return OK;
}

/*************************************************************************/

/* read_column_numbers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The file ends before the line of column numbers:
     "File ends before line of column numbers".
  2. The line has bad format: "Bad line of column numbers".
  3. A column number is greater than the number of places in a column:
     "COLUMN COUNT TOO BIG.
  4. The file ends before the blank line: "File ends before lines of ships".
  5. The blank line is missing: "Blank line missing before ships".

Called By:  get_bap

Side Effects: This reads the row of column numbers and records them in
world->col_counts.  It also reads the blank line following that row. The
column numbers line follows the bottom line of dashes.

*/

int read_column_numbers(   /* ARGUMENTS                 */
 FILE * in_port,           /* port to file to read from */
 char * buffer,            /* buffer to read into       */
 struct bap_world * world) /* puzzle model              */
{
  static char name[] SET_TO "read_column_numbers";
  int col;

  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
      "File ends before line of column numbers");
  CHB(((buffer[0] ISNT ' ') OR (buffer[1] ISNT ' ')),
      "Bad line of column numbers");
  for(col SET_TO 0; col < world->number_cols; col++)
    {
      CHB((sscanf((buffer+(2*col)+2), "%2d", &(world->cols[col].aim)) ISNT 1),
	  "Bad line of column numbers");
      CHB((world->cols[col].aim > world->number_rows),
	  "Column count too big");
    }
  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
      "File ends before lines of ships");
  CHB((buffer[0] ISNT '\n'), "Blank line missing before ships");
  return OK;
}

/*************************************************************************/

/* read_dashes

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The file ends before the top row of dashes:
     "File ends before first line of dashes".
  2. The line of dashes has bad format: "Bad first line of dashes".
  3. The line of dashes extends to the end of the buffer:
     "Line of dashes too long".
  4. There is not at least one dash:
     "problem does not have at least one column".

Called By:  get_bap

Side Effects: This
1. reads the top row of dashes and saves it in the "dashes" buffer.
2. sets bap->number_cols and checks that it is at least 1.
3. sets the row_length at two characters more than the length
   of the row of dashes. read_rows uses row_length to tell when
   it has read the bottom row of dashes.

Each dash indicates a column.

*/

int read_dashes(           /* ARGUMENTS                       */
 FILE * in_port,           /* port to file to read from       */
 char * dashes,            /* array into which to copy dashes */
 int * row_length,         /* length of row, set here         */
 struct bap_world * world) /* puzzle model                    */
{
  static char name[] SET_TO "read_dashes";
  int n;

  CHB((fgets(dashes, TEXT_SIZE, in_port) IS NULL),
      "File ends before first line of dashes");
  CHB((dashes[0] ISNT ' '), "Bad first line of dashes");
  for(n SET_TO 1; ; n SET_TO (n+2))
    {
      CHB((n >= (TEXT_SIZE - 1)), "Line of dashes too long");
      if (dashes[n] IS '\n')
	break;
      CHB(((dashes[n] ISNT ' ') OR (dashes[n+1] ISNT '-')),
	  "Bad first line of dashes");
    }
  *row_length SET_TO (n+2);
  world->number_cols SET_TO ((n-1)/2);
  CHB((world->number_cols < 1), "Problem does not have at least one column");
  if (world->print_all)
    printf("%d columns\n", world->number_cols);
  return OK;
}

/*************************************************************************/

/* read_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The row does not start with " |": "Bad problem row".
  2. An illegal character is included in the row: "Bad character".
  3. The number at the end of the row cannot be read: "Bad problem row".
  4. The number at the end of the row is larger than the number of
     places in the row: "Row count too big".

Called By:  read_rows

Side Effects: This
1. copies the row into world->squares
2. sets the value of world->rows[row].aim
3. adds one to world->number_rows.

*/

int read_row(              /* ARGUMENTS                     */
 char * buffer,            /* buffer from which to read row */
 struct bap_world * world) /* puzzle model                  */
{
  static char name[] SET_TO "read_row";
  int square_count;
  int col;
  int n;
  char c;
  
  CHB(((buffer[0] ISNT ' ') OR (buffer[1] ISNT '|')), "Bad problem row");
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      c SET_TO buffer[2 + (2 * col)];
      CHB(((c ISNT '+') AND (c ISNT '<') AND (c ISNT 'v') AND
	   (c ISNT 'O') AND (c ISNT '>') AND (c ISNT '^') AND
	   (c ISNT '.') AND (c ISNT ' ')), "Bad character");
      if (col < (world->number_cols - 1))
	CHB((buffer[3 + (2 * col)] ISNT ' '), "Bad character");
      else
	CHB((buffer[3 + (2 * col)] ISNT '|'), "Bad character");
      world->squares[world->number_rows][col] SET_TO c;
      if(c IS '.');
      else if (c ISNT ' ')
	{
	  world->cols[col].got++;
	  world->rows[world->number_rows].got++;
	}
    }
  n SET_TO sscanf((buffer + 2 + (2 * col)), "%d \n", &square_count);
  CHB((n ISNT 1), "Bad problem row");
  CHB((square_count > world->number_cols), "Row count too big");
  world->rows[world->number_rows].aim SET_TO square_count;
  world->number_rows++;
  
  return OK;
}

/*************************************************************************/

/* read_rows

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The file ends while rows are being read: "File ends while reading rows".
  2. read_row returns ERROR.

Called By:  get_bap

Side Effects: This reads the rows of the problem and builds parts of
the _world model. In verbose mode, this prints the number of rows.

Notes:

The row_length is set by read_dashes.

*/

int read_rows(             /* ARGUMENTS                 */
 FILE * in_port,           /* port to file to read from */
 char * buffer,            /* buffer to read rows into  */
 int row_length,           /* length of a row           */
 struct bap_world * world) /* puzzle model              */
{
  static char name[] SET_TO "read_rows";

  for (; ;)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "File ends while reading rows");
      if(strlen(buffer) < row_length)
	break;
      IFF(read_row(buffer, world));
    }
  if (world->print_all)
    printf("%d rows\n", world->number_rows);
  return OK;
}

/*************************************************************************/

/* read_ships

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The file ends before the end line is read:
     "File ends with no end marker".
  2. The battleships line has bad format: "Bad battleships line".
  3. The cruisers line has bad format: "Bad cruisers line".
  4. The destroyers line has bad format: "Bad destroyers line".
  5. The submarines line has bad format: "Bad submarines line".
  6. The row count total is not the same as the number of ship parts:
     "Bad row count total".
  7. The column count total is not the same as the number of ship parts:
     "Bad column count total".

Called By:  get_bap

Side Effects: This reads zero to four lines of ships, in order
(battleships, cruisers, destroyers, submarines) and the "end" line. It
also records the number of each type of ship.

Setting ship numbers to zero should not be necessary, since it is done
in init_bap, but it is done here just to be sure.

This will report an error if a ships line has bad format in any way,
such as line spacing being incorrect (including extra spaces or tabs
after the last ship on the line).

In verbose mode, this prints the number of each type of ship.

Notes:

This does not signal an error if there are no ships.

*/

int read_ships(            /* ARGUMENTS                 */
 FILE * in_port,           /* port to file to read from */
 char * buffer,            /* buffer to read ships into */
 struct bap_world * world) /* puzzle model              */
{
  static char name[] SET_TO "read_ships";
  int n;
  int sum;

  world->battleships_left SET_TO 0;
  world->cruisers_left SET_TO 0;
  world->destroyers_left SET_TO 0;
  world->submarines_left SET_TO 0;
  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
      "File ends with no end marker");
  for(n SET_TO 0; ; n SET_TO (n+6))
    {
      if (strncmp((buffer + n), "  <++>", 6) IS 0)
	world->battleships_left++;
      else
	break;
    }
  CHB(((n ISNT 0) AND (buffer[n] ISNT '\n')), "Bad battleships line");
  if (world->print_all)
    printf("%d battleships\n", world->battleships_left);
  if (world->battleships_left ISNT 0)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "File ends with no end marker");
    }
  for(n SET_TO 0; ; n SET_TO (n+5))
    {
      if (strncmp((buffer + n), "  <+>", 5) IS 0)
	world->cruisers_left++;
      else
	break;
    }
  CHB(((n ISNT 0) AND (buffer[n] ISNT '\n')), "Bad cruisers line");
  if (world->print_all)
    printf("%d cruisers\n", world->cruisers_left);
  if (world->cruisers_left ISNT 0)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "File ends with no end marker");
    }
  for(n SET_TO 0; ; n SET_TO (n+4))
    {
      if (strncmp((buffer + n), "  <>", 4) IS 0)
	world->destroyers_left++;
      else
	break;
    }
  CHB(((n ISNT 0) AND (buffer[n] ISNT '\n')), "Bad destroyers line");
  if (world->print_all)
    printf("%d destroyers\n", world->destroyers_left);
  if (world->destroyers_left ISNT 0)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "File ends with no end marker");
    }
  for(n SET_TO 0; ; n SET_TO (n+3))
    {
      if (strncmp((buffer + n), "  O", 3) IS 0)
	  world->submarines_left++;
      else
	break;
    }
  CHB(((n ISNT 0) AND (buffer[n] ISNT '\n')), "Bad submarines line");
  if (world->print_all)
    printf("%d submarines\n", world->submarines_left);
  if (world->submarines_left ISNT 0)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "File ends with no end marker");
    }
  CHB((strncmp(buffer, "end", 3) ISNT 0), "File ends with no end marker");
  sum SET_TO 0;
  for (n SET_TO 0; n < world->number_rows; n++)
    sum SET_TO (sum + world->rows[n].aim);
  CHB((sum ISNT ((4 * world->battleships_left) + (3 * world->cruisers_left) +
		 (2 * world->destroyers_left)  + world->submarines_left)),
      "Bad row count total");
  sum SET_TO 0;
  for (n SET_TO 0; n < world->number_cols; n++)
    sum SET_TO (sum + world->cols[n].aim);
  CHB((sum ISNT ((4 * world->battleships_left) + (3 * world->cruisers_left) +
		 (2 * world->destroyers_left)  + world->submarines_left)),
      "Bad column count total");
  return OK;
}

/*************************************************************************/

/* record_progress

Returned Value: int (OK)

Called By:
  insert_guess

Side Effects: If progress has been made, a line of explanation is
recorded, and in verbose mode, the puzzle is printed.

*/

int record_progress(       /* ARGUMENTS       */
 char * tried,             /* text of message */
 struct bap_world * world) /* puzzle model    */
{
  static char name[] SET_TO "record_progress";
  int numb;     /* number of characters printed       */
  int n;        /* counter for rows_in and columns_in */
  char * where; /* where to put next characters       */
  
  if (world->progress ISNT 0)
    {
      numb SET_TO sprintf(world->logic[world->logic_line], tried);
      where SET_TO (world->logic[world->logic_line] + numb);
      if (world->number_rows_in > 0)
	{
	  sprintf(where, "row");
	  where SET_TO (where + 3);
	  numb SET_TO
	    sprintf(where, ((world->number_rows_in > 1) ? "s " : " "));
	  where SET_TO (where + numb);
	  for (n SET_TO 0; n < world->number_rows_in; n++)
	    {
	      numb SET_TO sprintf(where, "%d ", (1 + world->rows_in[n]));
	      where SET_TO (where + numb);
	    }
	}
      if (world->number_cols_in > 0)
	{
	  if (world->number_rows_in > 0)
	    {
	      sprintf(where, "and ");
	      where SET_TO (where + 4);
	    }
	  sprintf(where, "column");
	  where SET_TO (where + 6);
	  numb SET_TO
	    sprintf(where, ((world->number_cols_in > 1) ? "s " : " "));
	  where SET_TO (where + numb);
	  for (n SET_TO 0; n < world->number_cols_in; n++)
	    {
	      numb SET_TO sprintf(where, "%d ", (1 + world->cols_in[n]));
	      where SET_TO (where + numb);
	    }
	}
      if (world->print_all)
	{
	  print_bap(world);
	  printf("%s\n\n", world->logic[world->logic_line]);
	}
      world->logic_line++;
    }
  return OK;
}

/*************************************************************************/

/* ship_fits_col_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. Size is less than one or greater than four: "Bug bad size".
  2. legal_col_across returns ERROR.

Called By:
  make_guess_ship_col
  submarine_covers

Side Effects: This checks if a ship of the given size fits in the
given column, starting in the given row. It also checks if that is a
legal placement. If the placement is legal, *is_legal is set to 1;
otherwise, *is_legal is set to 0.

ys is set to the number of empty squares that need to be filled
so the ship has no blanks in it.

Notes:

size must be 1, 2, 3, or 4

This does not check if a ship is already located in the column
starting at the given row.

This builds a test column used by legal_col_across.  The ship to be
tested is inserted in the test column starting at the given
row. Squares of the ship that were previously blank are marked with
Y.

*/

int ship_fits_col_row(     /* ARGUMENTS                                 */
 int col,                  /* index of column being checked             */
 int row,                  /* index of row being checked                */
 int size,                 /* size of ship                              */
 int * ys,                 /* number of additional ship parts, set here */
 int * is_legal,           /* set here to 0 or 1                        */
 struct bap_world * world) /* puzzle model                              */
{
  static char name[] SET_TO "ship_fits_col_row";
  int rows;
  char item1;
  char item2;
  char item3;
  char item4;
  char test[MAX_NUMBER_ROWS + 2];
  int n;
  int total;
  
  CHB(((size < 1) OR (size > 4)), "Bug bad size")
  *ys SET_TO 0;
  *is_legal SET_TO 1;
  item1 SET_TO world->squares[row][col];
  if (((square(row-1, col-1, world) ISNT '.') AND
       (square(row-1, col-1, world) ISNT ' ')) OR
      ((square(row-1, col  , world) ISNT '.') AND
       (square(row-1, col  , world) ISNT ' ')) OR
      ((square(row-1, col+1, world) ISNT '.') AND
       (square(row-1, col+1, world) ISNT ' ')) OR
      ((square(row,   col-1, world) ISNT '.') AND
       (square(row,   col-1, world) ISNT ' ')) OR
      ((square(row,   col+1, world) ISNT '.') AND
       (square(row,   col+1, world) ISNT ' ')) OR
      ((square(row+1, col-1, world) ISNT '.') AND
       (square(row+1, col-1, world) ISNT ' ')) OR
      ((square(row+1, col+1, world) ISNT '.') AND
       (square(row+1, col+1, world) ISNT ' ')))
    *is_legal SET_TO 0;
  if (*is_legal AND (size > 1))
    {
      item2 SET_TO world->squares[row+1][col];
      if(((square(row+2, col-1, world) ISNT '.') AND 
	  (square(row+2, col-1, world) ISNT ' ')) OR
	 ((square(row+2, col+1, world) ISNT '.') AND
	  (square(row+2, col+1, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal AND (size > 2))
    {
      item3 SET_TO world->squares[row+2][col];
      if(((square(row+3, col-1, world) ISNT '.') AND
	  (square(row+3, col-1, world) ISNT ' ')) OR
	 ((square(row+3, col+1, world) ISNT '.') AND
	  (square(row+3, col+1, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal AND (size > 3))
    {
      item4 SET_TO world->squares[row+3][col];
      if(((square(row+4, col-1, world) ISNT '.') AND
	  (square(row+4, col-1, world) ISNT ' ')) OR
	 ((square(row+4, col+1, world) ISNT '.') AND
	  (square(row+4, col+1, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal)
    {
      if (size IS 1)
	{
	  if(((item1 ISNT 'O') AND (item1 ISNT ' ')) OR
	     ((square(row+1, col, world) ISNT '.') AND
	      (square(row+1, col, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 2)
	{
	  if(((item1 ISNT '^') AND (item1 ISNT ' ')) OR
	     ((item2 ISNT 'v') AND (item2 ISNT ' ')) OR 
	     ((square(row+2, col, world) ISNT '.') AND
	      (square(row+2, col, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 3)
	{
	  if(((item1 ISNT '^') AND (item1 ISNT ' ')) OR
	     ((item2 ISNT '+') AND (item2 ISNT ' ')) OR
	     ((item3 ISNT 'v') AND (item3 ISNT ' ')) OR 
	     ((square(row+3, col, world) ISNT '.') AND
	      (square(row+3, col, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 4)
	{
	  if(((item1 ISNT '^') AND (item1 ISNT ' ')) OR
	     ((item2 ISNT '+') AND (item2 ISNT ' ')) OR
	     ((item3 ISNT '+') AND (item3 ISNT ' ')) OR 
	     ((item4 ISNT 'v') AND (item4 ISNT ' ')) OR 
	     ((square(row+4, col, world) ISNT '.') AND
	      (square(row+4, col, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
    }
  if (*is_legal)
    {
      rows SET_TO world->number_rows;
      test[0] SET_TO '.';
      test[rows+1] SET_TO '.';
      total SET_TO 0;
      for (n SET_TO 0; n < rows; n++)
	{
	  if ((n < row) OR (n > (row + (size -1))))
	    {
	      if ((square(n, col, world) IS ' ') OR
		  (square(n, col, world) IS '.'))
		test[n+1] SET_TO '.';
	      else
		{
		  test[n+1] SET_TO square(n, col, world);
		  total++;
		}
	    }
	  else
	    {
	      total++;
	      if (square(n, col, world) IS ' ')
		{
		  test[n+1] SET_TO 'Y';
		  (*ys)++;
		}
	      else
		test[n+1] SET_TO square(n, col, world);
	    }
	}
      if (total > world->cols[col].aim)
	*is_legal SET_TO 0;
/* see discussion at beginning of this file */
      if (*is_legal)
	IFF(legal_col_across(test+1, is_legal, world));
/* end comment out */
    }
  return OK;
}

/*************************************************************************/

/* ship_fits_row_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. Size is less than one or greater than four: "Bug bad size".
  2. legal_row_across returns ERROR.

Called By:
  make_guess_ship_row
  submarine_covers

Side Effects: This checks if a ship of the given size fits in the
given row, starting in the given column. It also checks if that is a
legal placement. If the placement is legal, *is_legal is set to 1;
otherwise, *is_legal is set to 0.

ys is set to the number of empty squares that need to be filled so the
ship has no blanks in it.

Notes:

size must be 1, 2, 3, or 4

This does not check if a ship is already located in the given row
starting at the given column.

This builds a test row used by legal_row_across.  The ship to be
tested is inserted in the test row starting at the given col. Squares
of the ship that were previously blank are marked with Y. Blank
squares in the test row outside the ship are marked as water. This is
only for the convenience of legal_row_across, not because they are all
necessarily water (although the two at the ends of the ship are
necessarily water).

*/

int ship_fits_row_col(     /* ARGUMENTS                                 */
 int row,                  /* index of row being checked                */
 int col,                  /* index of column being checked             */
 int size,                 /* size of ship                              */
 int * ys,                 /* number of additional ship parts, set here */
 int * is_legal,           /* set here to 0 or 1                        */
 struct bap_world * world) /* puzzle model                              */
{
  static char name[] SET_TO "ship_fits_row_col";
  int cols;
  char item1;
  char item2;
  char item3;
  char item4;
  char test[MAX_NUMBER_COLS + 2];
  int n;
  int total;
  
  CHB(((size < 1) OR (size > 4)), "Bug bad size")
  *ys SET_TO 0;
  *is_legal SET_TO 1;
  item1 SET_TO world->squares[row][col];
  if (((square(row-1, col-1, world) ISNT '.') AND
       (square(row-1, col-1, world) ISNT ' ')) OR
      ((square(row,   col-1, world) ISNT '.') AND
       (square(row,   col-1, world) ISNT ' ')) OR
      ((square(row+1, col-1, world) ISNT '.') AND
       (square(row+1, col-1, world) ISNT ' ')) OR
      ((square(row-1, col,   world) ISNT '.') AND
       (square(row-1, col,   world) ISNT ' ')) OR
      ((square(row+1, col,   world) ISNT '.') AND
       (square(row+1, col,   world) ISNT ' ')) OR
      ((square(row-1, col+1, world) ISNT '.') AND
       (square(row-1, col+1, world) ISNT ' ')) OR
      ((square(row+1, col+1, world) ISNT '.') AND
       (square(row+1, col+1, world) ISNT ' ')))
    *is_legal SET_TO 0;
  if (*is_legal AND (size > 1))
    {
      item2 SET_TO world->squares[row][col+1];
      if(((square(row-1, col+2, world) ISNT '.') AND
	  (square(row-1, col+2, world) ISNT ' ')) OR
	 ((square(row+1, col+2, world) ISNT '.') AND
	  (square(row+1, col+2, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal AND (size > 2))
    {
      item3 SET_TO world->squares[row][col+2];
      if(((square(row-1, col+3, world) ISNT '.') AND
	  (square(row-1, col+3, world) ISNT ' ')) OR
	 ((square(row+1, col+3, world) ISNT '.') AND
	  (square(row+1, col+3, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal AND (size > 3))
    {
      item4 SET_TO world->squares[row][col+3];
      if(((square(row-1, col+4, world) ISNT '.') AND
	  (square(row-1, col+4, world) ISNT ' ')) OR
	 ((square(row+1, col+4, world) ISNT '.') AND
	  (square(row+1, col+4, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal)
    {
      if (size IS 1)
	{
	  if(((item1 ISNT 'O') AND (item1 ISNT ' ')) OR
	     ((square(row, col+1, world) ISNT '.') AND
	      (square(row, col+1, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 2)
	{
	  if(((item1 ISNT '<') AND (item1 ISNT ' ')) OR
	     ((item2 ISNT '>') AND (item2 ISNT ' ')) OR 
	     ((square(row, col+2, world) ISNT '.') AND
	      (square(row, col+2, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 3)
	{
	  if(((item1 ISNT '<') AND (item1 ISNT ' ')) OR
	     ((item2 ISNT '+') AND (item2 ISNT ' ')) OR
	     ((item3 ISNT '>') AND (item3 ISNT ' ')) OR 
	     ((square(row, col+3, world) ISNT '.') AND
	      (square(row, col+3, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 4)
	{
	  if(((item1 ISNT '<') AND (item1 ISNT ' ')) OR
	     ((item2 ISNT '+') AND (item2 ISNT ' ')) OR
	     ((item3 ISNT '+') AND (item3 ISNT ' ')) OR 
	     ((item4 ISNT '>') AND (item4 ISNT ' ')) OR 
	     ((square(row, col+4, world) ISNT '.') AND
	      (square(row, col+4, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
    }
  if (*is_legal)
    {
      cols SET_TO world->number_cols;
      test[0] SET_TO '.';
      test[cols+1] SET_TO '.';
      total SET_TO 0;
      for (n SET_TO 0; n < cols; n++)
	{
	  if ((n < col) OR (n > (col + (size -1))))
	    {
	      if ((square(row, n, world) IS ' ') OR
		  (square(row, n, world) IS '.'))
		test[n+1] SET_TO '.';
	      else
		{
		  test[n+1] SET_TO square(row, n, world);
		  total++;
		}
	    }
	  else
	    {
	      total++;
	      if (square(row, n, world) IS ' ')
		{
		  test[n+1] SET_TO 'Y';
		  (*ys)++;
		}
	      else
		test[n+1] SET_TO square(row, n, world);
	    }
	}
      if (total > world->rows[row].aim)
	*is_legal SET_TO 0;
/* see discussion at beginning of this file */
      if (*is_legal)
	IFF(legal_row_across(test+1, is_legal, world));
/* end comment out */
    }
  return OK;
}

/*************************************************************************/

/* solve_bap

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. One of the following functions returns ERROR:
     explain_bap,
     locate_ships,
     mark_dones,
     print_bap,
     try_guessing,
     verify_squares.

Called By:
  main
  try_guessing

Side Effects: The puzzle gets filled in, if possible. While this is
happening, the bap_world gets changed continually.

Notes:

This has a hard-coded list of things to try for solving
the puzzle arranged in order from easy to hard:

1.  Check the current state of the puzzle for being legal (verify_squares).
2.  Locate previously unlocated identifiable ships. A ship is not
    identifiable until it has no Xs in it and has all parts (ends
    and middle, if any) in place (locate_ships).
3.  Mark the rows and columns that are done (mark dones).
4. If done, print the puzzle and the explanation of how it was solved.
    If not, return whatever try_guessing returns.

The function goes down the list of things to try. verify_squares and
locate_ships are always run. They do not fill in any squares. After
that, as soon as one of the functions succeeds in filling in even one
square, solve_bap goes back to the top of the list (skipping the
remaining, harder, things to try) and goes down it again. The easier
things to try (even the same thing that worked last time) may now work
because more of the puzzle is filled in.

If all the things to try do not solve the puzzle, call try_guessing,
which calls solve_bap recursively.

Since this does not call fill_bap, if the statement of the puzzle
includes ship parts, this may reach a state in which world->undone
is zero, but an original ship part is not part of a complete ship and
one of the world->XXX_left's is not zero. Thus, as compared with other
versions of this source code, a check that all the world->XXX_left's
are zero has been added.

*/

int solve_bap(             /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  static char name[] SET_TO "solve_bap";

  for(world->progress SET_TO 0; world->undone ISNT 0; world->progress SET_TO 0)
    {
      IFF(verify_squares(world));
      IFF(locate_ships(world));
      IFF(mark_dones(world));
      if (world->progress)
	continue;
      else
	break;
    }
  if ((world->undone IS 0) AND
      (world->battleships_left IS 0) AND
      (world->cruisers_left IS 0) AND
      (world->destroyers_left IS 0) AND
      (world->submarines_left IS 0))
    {
      IFF(print_bap(world));
      IFF(explain_bap(world));
    }
  return (((world->undone IS 0) AND
	   (world->battleships_left IS 0) AND
	   (world->cruisers_left IS 0) AND
	   (world->destroyers_left IS 0) AND
	   (world->submarines_left IS 0)) ? OK : try_guessing(world));
}

/*************************************************************************/

/* square

Returned Value: char 

This returns the actual character of world->squares in the row, col
position if the square is inside the defined area, and returns water
('.') otherwise.

Called By:
  legal_col_across
  legal_row_across
  locate_ships
  ship_fits_col_row
  ship_fits_row_col
  verify_square

Side Effects: none

Notes:

This is an array referencer for world->squares that pads the occupied
squares with a virtually infinite amount of virtual water. Doing this
allows the functions that use it to pretend the array consists of
all inside squares (none along the edges), which simplifies things
greatly.

Virtual water is better than actual water because there is as much of
it as necessary, and it is not necessary to fiddle with the world->squares
array itself.

*/

char square(               /* ARGUMENTS              */
 int row,                  /* row index of square    */
 int col,                  /* column index of square */
 struct bap_world * world) /* puzzle model           */
{
  return (((row > -1) AND (row < world->number_rows) AND
	   (col > -1) AND (col < world->number_cols)) ?
	  world->squares[row][col] : '.');
}

/*************************************************************************/

/* submarine_covers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_row_col returns ERROR.
  2. ship_fits_col_row returns ERROR.

Called By:
  make_guess_ship_col
  make_guess_ship_row

Side Effects: This checks that it is OK for a submarine to be at the
given row and column. It sets *cover to 1 if OK and to 0 if not.

Notes:

This calls both ship_fits_row_col and ship_fits_col_row so that both
strips containing the submarine are checked and all four strips
next to those two strips are also checked.
 
*/


int submarine_covers(      /* ARGUMENTS              */
 int row,                  /* row index of square    */
 int col,                  /* column index of square */
 int * cover,              /* set here to 0  or 1    */
 struct bap_world * world) /* puzzle model           */
{
  static char name[] SET_TO "submarine_covers";
  int ys;

  IFF(ship_fits_row_col(row, col, 1, &ys, cover, world));
  if (*cover IS 1)
    IFF(ship_fits_col_row(col, row, 1, &ys, cover, world));

  return OK;
}

/*************************************************************************/

/* try_guessing

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. No answer is found: "Cannot solve puzzle".
  2. make_guesses_ships returns ERROR.
  3. copy_bap returns ERROR.
  4. insert_guess returns ERROR.

Called By:  solve_bap

Side Effects: This picks an exhaustive set of guesses for the largest
remaining type of ship and tries the guesses one at a time until at
least one works.

If world->find_all is 1, this finds all solutions to the puzzle by trying
all guesses. Otherwise, this stops at the first successful guess.

Each guess is recorded in the world model.

Notes:

*/

int try_guessing(          /* ARGUMENTS     */
 struct bap_world * world) /* puzzle model  */
{
  static char name[] SET_TO "try_guessing";
  int number_guesses;
  int n;
  struct guess guesses[MAX_GUESSES];
  int result;
  int return_value;
  struct bap_world world_copy;

  return_value SET_TO ERROR;
  if (world->battleships_left)
    IFF(make_guesses_ships(4, &number_guesses, guesses, world));
  else if (world->cruisers_left)
    IFF(make_guesses_ships(3, &number_guesses, guesses, world));
  else if (world->destroyers_left)
    IFF(make_guesses_ships(2, &number_guesses, guesses, world));
  else if (world->submarines_left)
    IFF(make_guesses_ships(1, &number_guesses, guesses, world));
  else
    CHB(1, "Bug in guess system");
  for(n SET_TO 0; n < number_guesses; n++)
    {
       IFF(copy_bap(&world_copy, world));
       if (guesses[n].in_row)
	 IFF(insert_guess_row(&(guesses[n]), &world_copy));
       else
	 IFF(insert_guess_col(&(guesses[n]), &world_copy));
       result SET_TO solve_bap(&world_copy);
       IFF(copy_guess(&(world->guesses_tried[world->number_guesses_tried++]),
		      &(guesses[n])));
       if (result IS OK)
	 {
	   return_value SET_TO OK;
	   if (world->find_all IS 0)
	     break;
	 }	 
    }
  return return_value;
}

/*************************************************************************/

/* verify_square

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. An illegal square is found above, below, right, or left of the given
     square: "Puzzle has no answer".
  2. The given character should not have been given:
     "Bug bad character undetected".

Called By:  verify_squares

Side Effects: This checks that no illegal condition exists involving
the squares above, below, right, and left of the square at
world->squares[row][col] containing the given item. The given item must
be a ship part, one of <>^vO+

Notes:

The sides of the puzzle are dealt with by using the function "square"
which returns water ('.') along the sides of the puzzle.

The first four cases handle ship ends, one of <>^v
1. A ship end must have its continuation side adjacent to one of:
   a. a mating ship end,
   b. a ship middle ('+'),
   c. a blank.
2. A ship end must have spaces or blanks to the right and left of it
   (for a vertical end) or above and below (for a horizontal end).

The fifth case is a submarine, which must have water or blanks above,
below, right, and left.

The sixth case is a ship middle for which:
1. Any vertically or horizontally adjacent ship end must have its
   continuation side facing the ship middle.
2. It must be possible to continue the ship middle either both right
   and left or both up and down.
3. If there is a ship part above or below, the square on the other side
   of the ship middle from that ship part must be one of:
   a. a ship middle,
   b. an unknown ship part,
   c. a blank,
   d. a ship end with its continuation side towards the ship middle.

"L" configurations because in the statement of the puzzle, will cause
this to fail, but the solving procedure never makes them. Examples of
"L" configurations follow.

   ^    <+
   +>    v

*/

int verify_square(         /* ARGUMENTS              */
 char item,                /* character in square    */
 int row,                  /* row index of square    */
 int col,                  /* column index of square */
 struct bap_world * world) /* puzzle model           */
{
  static char name[] SET_TO "verify_square";
  char up;
  char dn;
  char rt;
  char lf;
  
  up SET_TO square(row-1, col, world);
  dn SET_TO square(row+1, col, world);
  rt SET_TO square(row, col+1, world);
  lf SET_TO square(row, col-1, world);
  switch(item)
    {
    case '<':
      CHK(((rt ISNT '>') AND (rt ISNT '+') AND
	   (rt ISNT ' ')), "Puzzle has no answer");
      CHK(((lf ISNT '.') AND (lf ISNT ' ')), "Puzzle has no answer");
      CHK(((up ISNT '.') AND (up ISNT ' ')), "Puzzle has no answer");
      CHK(((dn ISNT '.') AND (dn ISNT ' ')), "Puzzle has no answer");
      break;
    case '>':
      CHK(((rt ISNT '.') AND (rt ISNT ' ')), "Puzzle has no answer");
      CHK(((lf ISNT '<') AND (lf ISNT '+') AND
	   (lf ISNT ' ')), "Puzzle has no answer");
      CHK(((up ISNT '.') AND (up ISNT ' ')), "Puzzle has no answer");
      CHK(((dn ISNT '.') AND (dn ISNT ' ')), "Puzzle has no answer");
      break;
    case 'v':
      CHK(((rt ISNT '.') AND (rt ISNT ' ')), "Puzzle has no answer");
      CHK(((lf ISNT '.') AND (lf ISNT ' ')), "Puzzle has no answer");
      CHK(((up ISNT '^') AND (up ISNT '+') AND
	   (up ISNT ' ')), "Puzzle has no answer");
      CHK(((dn ISNT '.') AND (dn ISNT ' ')), "Puzzle has no answer");
      break;
    case '^':
      CHK(((rt ISNT '.') AND (rt ISNT ' ')), "Puzzle has no answer");
      CHK(((lf ISNT '.') AND (lf ISNT ' ')), "Puzzle has no answer");
      CHK(((up ISNT '.') AND (up ISNT ' ')), "Puzzle has no answer");
      CHK(((dn ISNT 'v') AND (dn ISNT '+') AND
	   (dn ISNT ' ')), "Puzzle has no answer");
      break;
    case 'O':
      CHK(((rt ISNT '.') AND (rt ISNT ' ')), "Puzzle has no answer");
      CHK(((lf ISNT '.') AND (lf ISNT ' ')), "Puzzle has no answer");
      CHK(((up ISNT '.') AND (up ISNT ' ')), "Puzzle has no answer");
      CHK(((dn ISNT '.') AND (dn ISNT ' ')), "Puzzle has no answer");
      break;
    case '+':
      CHK(((rt IS '<') OR (rt IS 'v') OR (rt IS '^') OR (rt IS 'O')),
	  "Puzzle has no answer");
      CHK(((lf IS '>') OR (lf IS 'v') OR (lf IS '^') OR (lf IS 'O')),
	  "Puzzle has no answer");
      CHK(((up IS 'v') OR (up IS '<') OR (up IS '>') OR (up IS 'O')),
	  "Puzzle has no answer");
      CHK(((dn IS '^') OR (dn IS '<') OR (dn IS '>') OR (dn IS 'O')),
	  "Puzzle has no answer");
      CHK((((rt IS '.') OR (lf IS '.')) AND ((up IS '.') OR (dn IS '.'))),
	  "Puzzle has no answer");
      CHK((((rt IS '>') OR (rt IS '+')) AND (lf IS '.')),
	  "Puzzle has no answer");
      CHK((((lf IS '<') OR (lf IS '+')) AND (rt IS '.')),
	  "Puzzle has no answer");
      CHK((((up IS '^') OR (up IS '+')) AND (dn IS '.')),
	  "Puzzle has no answer");
      CHK((((dn IS 'v') OR (dn IS '+')) AND (up IS '.')),
	  "Puzzle has no answer");
      break;
    default:
      CHB(1, "Bug bad character undetected");
      break;
    }
  return OK;
}

/*************************************************************************/

/* verify_squares

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. verify_square returns ERROR.
  2. A strip has too many ship parts: "Puzzle has no answer"

Called By:  solve_bap

Side Effects: This checks that the world->squares layout is not
illegal. It looks at every square, and if it is a ship part, makes
sure it is possible to fit that part into a ship.

This also checks that:
a. the number of filled squares in each row and column is not larger
   than the total given for that row or column.

Notes:

The squares diagonally adjacent to each ship part, which must all be
blank or water are not checked here.

This does not check that the ships themselves are legal (not too many
of a given type, etc.).

*/

int verify_squares(        /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  static char name[] SET_TO "verify_squares";
  int row;
  int col;
  char item;
  struct strip rown;
  struct strip coln;

  for (row SET_TO 0; row < world->number_rows; row++)
    {
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  item SET_TO world->squares[row][col];
	  if ((item ISNT '.') AND (item ISNT ' '))
	    {
	      IFF(verify_square(item, row, col, world));
	    }
	}  
    }
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      rown SET_TO world->rows[row];
      CHK((rown.got > rown.aim), "Puzzle has no answer");
    }
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      coln SET_TO world->cols[col];
      CHK((coln.got > coln.aim), "Puzzle has no answer");
    }
  return OK;
}

/*************************************************************************/

