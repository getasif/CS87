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
#define MAX_STRIP_BLOCKS 18

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
  struct strip cols[MAX_NUMBER_COLS]; /* data on columns                      */
  int max_targets_col;                /* largest number of targets in any col */
  int max_targets_row;                /* largest number of targets in any row */
  int number_cols;                    /* number of columns in puzzle          */
  int number_rows;                    /* number of rows in puzzle             */
  struct strip rows[MAX_NUMBER_ROWS]; /* data on rows                         */
};

/*************************************************************************/

/* declare_functions

*/

int add_target(struct strip * a_strip, int length);
int init_world(struct paint_world * world);
int main(int argc, char ** argv);
int print_pro(struct paint_world * world);
int read_problem(char * file_name, struct paint_world * world);

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

*/

int init_world(              /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "init_world";
  int row;
  int col;

  world->max_targets_col SET_TO 0;
  world->max_targets_row SET_TO 0;
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
  IFF(print_pro(world));
  return OK;
}

/*************************************************************************/

int print_pro(
 struct paint_world * world)
{
  const char * name SET_TO "print_pro";
  struct strip * a_strip;
  int n;
  int m;
  int col;
  int row;
  int number;

  printf("\n");
  for (n SET_TO 0; n < world->max_targets_col; n++)
    {
      for (m SET_TO (3 * world->max_targets_row); m > -1; m--)
	putchar(' ');
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  a_strip SET_TO &(world->cols[col]);
	  m SET_TO ((n + a_strip->number_targets) - world->max_targets_col);
	  if (m > -1)
	    {
	      number SET_TO a_strip->targets[m];
	      if (number > 99)
		printf("%c%d", ((number / 10) + 55), (number % 10));
	      else
		printf("%2d", number);
	    }
	  else
	    printf("  ");
	}
      printf("\n");
    }
  for (m SET_TO (3 * world->max_targets_row); m > -1; m--)
	putchar(' ');
  for (m SET_TO 0; m < world->number_cols; m++)
    printf("--");
  printf("\n");
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      a_strip SET_TO &(world->rows[row]);
      for (n SET_TO 0;
	   n < (world->max_targets_row - a_strip->number_targets);
	   n++)
	printf("   ");
      for (n SET_TO 0; n < a_strip->number_targets; n++)
	printf("%3d", a_strip->targets[n]);
      putchar('|');
      for (m SET_TO 0; m < world->number_cols; m++)
	printf("  ");
      printf("|\n");
    }
  for (m SET_TO (3 * world->max_targets_row); m > -1; m--)
    putchar(' ');
  for (m SET_TO 0; m < world->number_cols; m++)
    printf("--");
  printf("\n");
  return OK;
}

/*************************************************************************/

/* read_problem

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. 
  2. 

Called By:  main

Side Effects: This reads the problem and puts data into the puzzle model.

The file must have the following format:

1. any number of lines of comments, which will be reprinted in the output.
2. one blank line
3. "width <int>" - this is the number of columns
4. "height <int>" - this is the number of rows
5. one blank line
6. "rows"
7. height lines of the form  "<int>" or "<int>,<int>,...,<int>"
8. one blank line
9. "columns"
10. width lines of the form  "<int>" or "<int>,<int>,...,<int>"
11. one blank line
12. any number of lines of comments, which will not be read

The lines with the ints give the targets for that strip.
A blank line must have only one character, a newline.

*/

int read_problem(            /* ARGUMENTS            */
 char * file_name,           /* name of file to read */
 struct paint_world * world) /* puzzle model         */
{
  const char * name SET_TO "read_problem";
  FILE * in_port;
  char buffer[TEXT_SIZE];
  int height;
  int width;
  int length;  /* length of a target */
  int n;
  int k;
  int targets; /* number of targets in a strip */
  int start;
  char stop;
  struct strip * a_strip;

  in_port SET_TO fopen(file_name, "r");
  CHB((in_port IS NULL), "could not open file");
  for (; ; )
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends before first blank line");
      if (buffer[0] IS '\n')
	break;
      else
	printf("%s", buffer);
    }
  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends before width line");
  n SET_TO sscanf(buffer, "width %d\n", &width);
  CHB((n ISNT 1), "could not read width");
  world->number_cols SET_TO width;
  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends before width line");
  n SET_TO sscanf(buffer, "height %d\n", &height);
  CHB((n ISNT 1), "could not read height");
  world->number_rows SET_TO height;
  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
      "file ends after height");
  CHB((buffer[0] ISNT '\n'), "blank line missing before rows");
  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
      "file ends before rows");
  CHK((strcmp("rows\n", buffer) ISNT 0), "bad rows heading");
  for(k SET_TO 0; k < height; k++)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL), "too few rows");
      stop SET_TO ',';
      a_strip SET_TO &(world->rows[k]);
      targets SET_TO 0;
      for(start SET_TO 0; stop ISNT '\n'; start++)
	{
	  n SET_TO sscanf((buffer + start), "%d%c", &length, &stop);
	  CHB((n ISNT 2), "bad rows line");
	  IFF(add_target(a_strip, length));
	  targets++;
	  for(; ((buffer[start] < 58) AND (buffer[start] > 47)); start++);
	}
      if (targets > world->max_targets_row)
	world->max_targets_row SET_TO targets;
    }
  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
      "file ends after rows");
  CHB((buffer[0] ISNT '\n'), "blank line missing before columns");
  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
      "file ends before columns");
  CHK((strcmp("columns\n", buffer) ISNT 0), "bad columns heading");
  for(k SET_TO 0; k < width; k++)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL), "too few columns");
      stop SET_TO ',';
      a_strip SET_TO &(world->cols[k]);
      targets SET_TO 0;
      for(start SET_TO 0; stop ISNT '\n'; start++)
	{
	  n SET_TO sscanf((buffer + start), "%d%c", &length, &stop);
	  CHB((n ISNT 2), "bad columns line");
	  IFF(add_target(a_strip, length));
	  targets++;
	  for(; ((buffer[start] < 58) AND (buffer[start] > 47)); start++);
	}
      if (targets > world->max_targets_col)
	world->max_targets_col SET_TO targets;
    }
  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
      "blank line missing after columns");
  CHB((buffer[0] ISNT '\n'), "line after columns not blank");
  fclose(in_port);
  return OK;
}

/*************************************************************************/

