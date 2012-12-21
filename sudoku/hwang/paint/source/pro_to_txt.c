/*************************************************************************/

/* comments

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
#define MAX_NUMBER_COLS 200
#define MAX_NUMBER_ROWS 200
#define TEXT_SIZE 256
#define MAX_STRIP_BLOCKS 16

/*************************************************************************/

/* hash_macros

*/

#define CHK(testbad, error_msg)         \
 if (testbad){                          \
   printf("%s: %s\n", name, error_msg); \
   return ERROR; }                      \
 else

#define CHB(testbad, error_msg)         \
 if (testbad){                          \
   printf("%s: %s\n", name, error_msg); \
   return ERROR; }                      \
 else

#define IFF(tryit)       \
 if (tryit ISNT OK){     \
   printf("%s\n", name); \
   return ERROR; }       \
 else

/*************************************************************************/

/* structures


A strip is a row or column of the puzzle. The squares of the strip are
not in the strip class, but are in the squares of the paint model.

*/

struct strip
{
  int number_targets;            /* number of targets in the strip  */
  int targets[MAX_STRIP_BLOCKS]; /* the targets of the strip        */
};

struct paint_world
{
  struct strip cols[MAX_NUMBER_COLS]; /* data on columns                     */
  int number_cols;                    /* number of columns in puzzle         */
  int number_rows;                    /* number of rows in puzzle            */
  int number_spaces;                  /* number of blanks or digits before | */
  struct strip rows[MAX_NUMBER_ROWS]; /* data on rows                        */
};

/*************************************************************************/

/* declare_functions

*/

int add_target(struct strip * a_strip, int length);
int get_paint1(char * file_name, struct paint_world * world);
int get_paint2(char * file_name, struct paint_world * world);
int init_world(struct paint_world * world);
int main(int argc, char ** argv);
int print_txt(struct paint_world * world);
int read_col_numbers(FILE * in_port, int * col_total,
    struct paint_world * world);
int read_problem(char * file_name, struct paint_world * world);
int read_row_numbers(FILE * in_port, int * row_total,
    struct paint_world * world);

/*************************************************************************/

/* globals

*/

/*************************************************************************/

/* define_functions

*/

/*************************************************************************/

/* add_target

Returned Value: int (OK)

Called By:
  read_col_numbers
  read_row_numbers

Side Effects: This adds a target of the given length to a_strip.

Notes:

A target of length 0 is treated like every other target.

*/

int add_target(          /* ARGUMENTS                                  */
 struct strip * a_strip, /* the strip to which a target is being added */
 int length)             /* the length of the target being added       */
{
  const char * name SET_TO "add_target";

  a_strip->targets[a_strip->number_targets] SET_TO length;
  a_strip->number_targets++;
  return OK;
}

/*************************************************************************/

/* get_paint1

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The named file could not be opened: "could not open file".
  2. The file ends before the first blank line:
     "file ends before first blank line".
  3. The first line of dashes is not found:
     "file ends before first line of dashes".
  4. The number of spaces for numbers to the left of the puzzle is
     not divisible by 3: "spaces before dashes not divisible by three".
  5. The first line of dashes does not start in the right place or is not
     followed immediately by a newline: "bad line of dashes".
  6. The number of dashes in the first line of dashes is not
     divisible by two: "dashes not divisible by two".
  7. A row of the puzzle is the wrong length: "bad row length".
  8. A | is missing where it should be: "bad row end marker".
  8. The second line of dashes is not found:
     "file ends before second line of dashes".
  9. A row of the puzzle has the wrong number of spaces between the
     opening | and the closing |: "bad row spaces".
 10. The second row of dashes differs from the first row of dashes:
     "bad second row of dashes".


Called By:  read_problem

Side Effects: The number of rows, number of columns, and number of
spaces in the puzzle model are set.

Notes:

The puzzle is read twice. This function does the first reading, mainly
to get the size of the puzzle, but also to make a lot of format
checks.  The format checks cover everything except the top rows of
numbers and the numbers to the left of the puzzle.

The number of spaces for the numbers to the left of the puzzle is
required to be divisible by 3 to make reading the numbers easy.
Three spaces are allowed per number, providing for a one-digit or
two-digit number in each place with two or one spaces before it.

1. Open the file and check that it opened.
2. Look for a blank line.
3. Look for first line of dashes
4. Check the first line of dashes
5. Count rows to the second line of dashes and check each row
6. Check second row of dashes

*/

int get_paint1(              /* ARGUMENTS            */
 char * file_name,           /* name of file to read */
 struct paint_world * world) /* puzzle model         */
{
  const char * name SET_TO "get_paint1";
  FILE * in_port;
  char buffer[TEXT_SIZE];
  char dashes[TEXT_SIZE];
  char spaces[TEXT_SIZE];
  int number_dashes;
  int number_spaces;
  int length;

  in_port SET_TO fopen(file_name, "r");
  CHB((in_port IS NULL), "could not open file");
  for (; ; )
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends before first blank line");
      if (buffer[0] IS '\n')
	break;
    }
  for (; ; )
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends before first line of dashes");
      sscanf(buffer, "%[ ]", spaces);
      number_spaces SET_TO strlen(spaces);
      if (buffer[number_spaces] IS '-')
	break;
    }
  number_spaces--;
  CHB(((number_spaces % 3) ISNT 0),
      "spaces before dashes not divisible by three");
  sscanf((buffer + number_spaces + 1), "%[-]", dashes);
  number_dashes SET_TO strlen(dashes);
  CHB((buffer[number_spaces + number_dashes + 1] ISNT '\n'),
      "bad line of dashes");
  CHB(((number_dashes % 2) ISNT 0), "dashes not divisible by two");
  world->number_cols SET_TO (number_dashes / 2);
  for (world->number_rows SET_TO 0; ; world->number_rows++)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends before second line of dashes");
      length SET_TO strlen(buffer);
      if (length IS (number_spaces + number_dashes + 2))
	break;
      CHB((length ISNT (number_spaces + number_dashes + 3)), "bad row length");
      CHB(((buffer[number_spaces] ISNT '|') OR
	   (buffer[number_spaces + number_dashes + 1] ISNT '|')),
	  "bad row end marker");
      sscanf((buffer + number_spaces + 1), "%[ ]", spaces);
      CHB((strlen(spaces) ISNT number_dashes), "bad row spaces");
    }
  sscanf(buffer, "%[ ]", spaces);
  CHB((strlen(spaces) ISNT (number_spaces + 1)), "bad second row of dashes");
  sscanf((buffer + number_spaces), "%[-]", dashes);
  CHB((strlen(dashes) ISNT number_dashes), "bad second row of dashes");
  world->number_spaces SET_TO number_spaces;
  fclose(in_port);
  return OK;
}

/*************************************************************************/

/* get_paint2

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The named file cannot be opened: "could not open file".
  2. The file ends before the first blank line:
     "file ends before first blank line".
  3. The total of the row numbers does not equal the total of the
     column numbers: "row total differs from column total".
  4. Any of the following functions returns ERROR:
     print_paint
     read_row_numbers
     read_col_numbers

Called By:  read_problem

Side Effects: The puzzle in the file named file_name is read and stored.

Notes:

The file represents an M columns by N rows rectangle to be filled. The
file must have all of the following in the order given:

1. a blank line (above which everything is read and ignored).
2. a number of rows each starting with B+1 blanks, and then having one-digit
   or two-digit numbers or blanks, each occupying two spaces. The total
   length of each row must not exceed (B+1 plus twice the number of columns).
   B must be divisible by 3.
3. a line consisting of B+1 blanks followed by a number of dashes equal
   to twice the number of columns.
4. 1 to 269 rows each consisting of (i) B characters in sets of three that
   are either all blanks or have a one-digit or two-digit number at the
   end, (ii) a "|", (iii) a number of blanks equal to twice the number of
   columns, (iv)  a "|".
5. a row identical to the one described in item 3, above.

Here is a sample file between rows of stars.

****************************************************
This is a sample problem file.

            1
        1 2 6 9 6 5 5 4 3 4
       --------------------
     2|                    |
  1  1|                    |
     4|                    |
  2  1|                    |
  3  1|                    |
     8|                    |
     8|                    |
     7|                    |
     5|                    |
     3|                    |
       --------------------


****************************************************

The reading procedure is:
1. Open the file and check that it opened.
2. Look for a blank line, echoing everything above it, plus the blank line.
3. Read the top rows of numbers.
4. Read the numbers to the left of the puzzle.

*/

int get_paint2(              /* ARGUMENTS            */
 char * file_name,           /* name of file to read */
 struct paint_world * world) /* puzzle model         */
{
  const char * name SET_TO "get_paint2";
  FILE * in_port;
  char buffer[TEXT_SIZE];
  int row_total;
  int col_total;

  row_total SET_TO 0;
  col_total SET_TO 0;
  in_port SET_TO fopen(file_name, "r");
  CHB((in_port IS NULL), "could not open file");
  for (; ; )
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends before first blank line");
      printf("%s", buffer);
      if (buffer[0] IS '\n')
	break;
    }
  IFF(read_col_numbers(in_port, &col_total, world));
  IFF(read_row_numbers(in_port, &row_total, world));
  fclose(in_port);
  CHB((row_total ISNT col_total), "row total differs from column total");
  return OK;
}

/*************************************************************************/

/* init_world

Returned Value: int (OK)

Called By:  main

Side Effects: The puzzle model is initialized.

Notes:

The number of targets for each strip is set to zero here. The data for
the targets is inserted by read_col_numbers after this function is
called. The following need no initialization because they are set by
other functions before being used.

world->number_cols
world->number_rows
world->number_spaces

*/

int init_world(              /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "init_world";
  int row;
  int col;

  for (col SET_TO 0; col < MAX_NUMBER_COLS; col++)
    {
      world->cols[col].number_targets SET_TO 0;
    }
  for (row SET_TO 0; row < MAX_NUMBER_ROWS; row++)
    {
      world->rows[row].number_targets SET_TO 0;
    }
  return OK;
}

/*************************************************************************/

/* main

Side Effects: This
1. calls init_world to initialize the puzzle model.
2. calls read_problem to read the puzzle, record data, and make
   a lot of format checks.
3. calls print_txt to print the text form of the puzzle.

Notes:

The IFFs (which contain "return" statements) stop execution in case an
error occurs (as intended) because in a "main" function, "return"
stops execution.

*/

int main(      /* ARGUMENTS                             */
 int argc,     /* one more than the number of arguments */
 char ** argv) /* function name and arguments           */
{
  const char * name SET_TO "main";
  struct paint_world world1;
  struct paint_world * world;

  world SET_TO &world1;
  IFF(init_world(world));
  IFF(read_problem(argv[1], world));
  IFF(print_txt(world));
  return OK;
}

/*************************************************************************/

int print_txt(
 struct paint_world * world)
{
  const char * name SET_TO "print_txt";
  struct strip * a_strip;
  int n;
  int m;

  printf("width %d\n", world->number_cols);
  printf("height %d\n\n", world->number_rows);
  printf("rows\n");
  for (n SET_TO 0; n < world->number_rows; n++)
    {
      a_strip SET_TO &(world->rows[n]);
      for (m SET_TO 0; m < a_strip->number_targets; m++)
	{
	  printf("%d", a_strip->targets[m]);
	  if (m IS (a_strip->number_targets - 1))
	    printf("\n");
	  else
	    printf(",");
	}
    }
  printf("\ncolumns\n");
  for (n SET_TO 0; n < world->number_cols; n++)
    {
      a_strip SET_TO &(world->cols[n]);
      for (m SET_TO 0; m < a_strip->number_targets; m++)
	{
	  printf("%d", a_strip->targets[m]);
	  if (m IS (a_strip->number_targets - 1))
	    printf("\n");
	  else
	    printf(",");
	}
    }
  printf("\n");
  return OK;
}

/*************************************************************************/

/* read_col_numbers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The file ends while reading column numbers:
     "file ends while reading column numbers".
  2. add_target returns ERROR.
  3. Two consecutive characters in the column numbers area do not make
     a number and are not blank: "bad column number".
  4. There are more than (MAX_STRIP_BLOCKS - 1) rows of column numbers:
     "too many rows of column numbers".

Called By:  get_paint2

Side Effects: This reads and records the column numbers that appear
above the puzzle. The strip for each column has targets added to it.

Notes:

This expects that the first unread line is the top row of numbers.
It stops reading after reading the first line of dashes.

The column numbers for solving, however, are as read for 0 - 99.
Since there are only two spaces allocated for the numbers, numbers
greater than 99 are coded in the input so that A = 100, B = 110, etc.
Thus, targets as large as 269 (Z9) can be encoded in the input file.

*/

int read_col_numbers(        /* ARGUMENTS                         */
 FILE * in_port,             /* port to file to read from         */
 int * col_total,            /* total of column numbers, set here */
 struct paint_world * world) /* puzzle model                      */
{
  const char * name SET_TO "read_col_numbers";
  int line;
  int n;
  int col;
  int length;
  char digits[3];
  char buffer[TEXT_SIZE];

  digits[2] SET_TO 0;
  for (line SET_TO 0; line < MAX_STRIP_BLOCKS; line++)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends while reading column numbers");
      if (buffer[world->number_spaces + 1] IS '-')
	break;
      col SET_TO 0;
      for (n SET_TO (world->number_spaces + 1);
	   buffer[n] ISNT '\n';
	   n SET_TO (n + 2))
	{
	  digits[0] SET_TO buffer[n];
	  digits[1] SET_TO buffer[n + 1];
	  if ((digits[0] > 64) AND (digits[0] < 91))
	    {
	      length SET_TO
		(100 + (10 * (digits[0] - 65)) + (digits[1] - 48));
	      *col_total SET_TO (*col_total + length);
	      IFF(add_target(&(world->cols[col]), length));
	    }
	  else if (sscanf(digits, "%d", &length) IS 1)
	    {
	      *col_total SET_TO (*col_total + length);
	      IFF(add_target(&(world->cols[col]), length));
	    }
	  else if ((digits[0] IS ' ') AND (digits[1] IS ' '))
	    buffer[(world->number_spaces + 1) + col] SET_TO ' ';
	  else
	    CHB(1, "bad column number");
	  col++;
	}
    }
  CHB((buffer[world->number_spaces + 1] ISNT '-'),
      "too many rows of column numbers");
  return OK;
}

/*************************************************************************/

/* read_problem

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. get_paint1 returns ERROR.
  2. get_paint2 returns ERROR.

Called By:  main

Side Effects: This reads the problem and puts data into the puzzle model.

*/

int read_problem(            /* ARGUMENTS            */
 char * file_name,           /* name of file to read */
 struct paint_world * world) /* puzzle model         */
{
  const char * name SET_TO "read_problem";

  IFF(get_paint1(file_name, world));
  IFF(get_paint2(file_name, world));
  return OK;
}

/*************************************************************************/

/* read_row_numbers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The file ends while reading row numbers:
     "file ends while reading row numbers".
  2. add_target returns ERROR.

Called By:  get_paint2

Side Effects: This reads and records the row numbers that appear
to the left of the puzzle.

Notes:

This repeatedly reads a line of row numbers into a buffer and then
transcribes characters from the beginning of the row into the digits
array three at a time. Then it tries to read a number from the digits
array. If there is one, it is recorded by add_target.

The row numbers can be 0 - 999.

*/

int read_row_numbers(        /* ARGUMENTS                      */
 FILE * in_port,             /* port to file to read from      */
 int * row_total,            /* total of row numbers, set here */
 struct paint_world * world) /* puzzle model                   */
{
  const char * name SET_TO "read_row_numbers";
  char buffer[TEXT_SIZE];
  int n;
  int row;
  int length;
  char digits[4];

  digits[3] SET_TO 0;
  for (row SET_TO 0; ; row++)
    {
      CHK((row > MAX_NUMBER_ROWS), "puzzle has too many rows");
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends while reading row numbers");
      if (buffer[world->number_spaces + 1] IS '-')
	break;
      for (n SET_TO 0; n < world->number_spaces; n SET_TO (n + 3))
	{
	  digits[0] SET_TO buffer[n];
	  digits[1] SET_TO buffer[n + 1];
	  digits[2] SET_TO buffer[n + 2];
	  if (sscanf(digits, "%d", &length) IS 1)
	    {
	      *row_total SET_TO (*row_total + length);
	      IFF(add_target(&(world->rows[row]), length));
	    }
	}
      buffer[n + 1] SET_TO 0;
    }
  return OK;
}

/*************************************************************************/
