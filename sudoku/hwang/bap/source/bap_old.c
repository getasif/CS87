/*************************************************************************/

/* comments

This version of the battleships solver is intended to be suitable as
input to the single solver generator generator (gen_gen). As compared
with the original version, the following changes have been made.

1. All source code has been put into a single file.
2. The code has been modified so a C compiler can compile it.
   All classes were changed to structs and all dynamic arrays
   (which required new and delete) were changed to static arrays.
3. The code was arranged strictly in sections.
4. The error handling system was modified so that no #defines or
   error message arrays are required.

*/

/*************************************************************************/

/* includes

*/

#include <stdio.h>
#include <math.h>
#include <string.h>

/*************************************************************************/

/* hash_defs

MAX_TEST should be larger than MAX_NUMBER_ROWS and MAX_NUMBER_COLS by
2, so that the actual first and last squares of a character array
MAX_TEST long can be actual water. See ship_fits_row_col,
ship_fits_col_row, try_full_but_one_col, and try_full_but_one_row.
Using [MAX_NUMBER_ROWS + 2] in those functions stumps the gen_gen_C
parser.

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
#define MAX_TEST 22
#define MAX_GUESSES 20

/*************************************************************************/

/* hash_macros

*/

#define CHK(testbad, error_msg)           \
 if (testbad){                            \
   if (world->verbose)                    \
     printf("%s: %s\n", name, error_msg); \
   return ERROR; }                        \
 else

#define CHB(testbad, error_msg)         \
 if (testbad){                          \
   printf("%s: %s\n", name, error_msg); \
   return ERROR; }                      \
 else

#define IFF(tryit)         \
 if (tryit ISNT OK){       \
   if (world->verbose)     \
     printf("%s\n", name); \
   return ERROR; }         \
 else

/*************************************************************************/

/* structures

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
  int aim;          /* number of squares to fill altogether              */
  int battleships;  /* number battleships assigned but not fully located */
  int cruisers;     /* number cruisers assigned but not fully located    */
  int destroyers;   /* number destroyers assigned but not fully located  */
  int done;         /* set to 1 if strip is done, 0 otherwise            */
  int dry;          /* number of squares not yet marked water            */
  int exes;         /* number of X's                                     */
  int got;          /* number of squares filled so far                   */
  int submarines;   /* number submarines assigned but not fully located  */
};

struct bap_world
{
  int battleships_ass;            /* number battleships assigned, unlocated  */
  int battleships_left;           /* number battleships unassigned unlocated */
  struct strip cols[MAX_NUMBER_COLS]; /* data on columns                     */
  int cols_in[MAX_NUMBER_COLS];   /* columns on which progress made          */
  int cruisers_ass;               /* number cruisers assigned, unlocated     */
  int cruisers_left;              /* number cruisers unassigned, unlocated   */
  int destroyers_ass;             /* number destroyers assigned, unlocated   */
  int destroyers_left;            /* number destroyers unassigned unlocated  */
  int exes;                       /* number of X's                           */
  int find_all;                   /* set to 1 if all answers to be found     */
  struct guess guesses_tried[MAX_GUESSES]; /* guesses already tried          */
  int locations[MAX_NUMBER_ROWS][MAX_NUMBER_COLS]; /* ship end locations     */
  char logic[TEXT_SIZE][TEXT_SIZE]; /* explanation of changes in puzzle      */
  int logic_line;                 /* number of next line of logic to write   */
  int number_cols;                /* number of columns in puzzle             */
  int number_in_cols;             /* number of colums on which progress made */
  int number_guesses_tried;       /* number of guesses tried                 */
  int number_rows;                /* number of rows in puzzle                */
  int number_in_rows;             /* number of rows on which progress made   */
  int print_all;                  /* set to 1 for printing partial solutions */
  int progress;                   /* set to 1 if progress made by solve loop */
  struct strip rows[MAX_NUMBER_ROWS]; /* data on rows                        */
  int rows_in[MAX_NUMBER_ROWS];   /* rows on which progress made             */
  char squares[MAX_NUMBER_ROWS][MAX_NUMBER_COLS]; /* array of squares        */
  int submarines_ass_col;         /* number subs in cols assigned unlocated  */
  int submarines_ass_row;         /* number subs in rows assigned unlocated  */
  int submarines_left_col;        /* number subs in cols unassigned unlocated*/
  int submarines_left_row;        /* number subs in rows unassigned unlocated*/
  int undone;                     /* number of strips not done               */
  int verbose;                    /* verbose (non-zero) or not (zero)        */
};

/*************************************************************************/

/* declare_functions

*/

int allocate_ship_col(int row, int col, int length, struct bap_world * world);
int allocate_ship_row(int row, int col, int length, struct bap_world * world);
int already_tried(int row, int col, int size, int in_row, int * newby,
  struct bap_world * world);
int assign_battleships(struct bap_world * world);
int assign_cruisers(struct bap_world * world);
int assign_destroyers(struct bap_world * world);
int assign_submarines(struct bap_world * world);
int assign_submarines_cols(int * total_assigned, struct bap_world * world);
int assign_submarines_rows(int * total_assigned, struct bap_world * world);
int copy_bap(struct bap_world * world_copy, struct bap_world * world);
int copy_guess(struct guess * guess_copy, struct guess * guess_original);
int copy_guesses(struct guess * guesses_copy, struct guess * guesses,
  int number_guesses);
int copy_strip(struct strip * strip_copy, struct strip * strip_old);
int dex_ends_col(int col, struct bap_world * world);
int dex_ends_row(int row, struct bap_world * world);
int dex_square(int row, int col, struct bap_world * world);
int dex_squares(struct bap_world * world);
int enx_square(int row, int col, struct bap_world * world);
int explain_bap(struct bap_world * world);
int extend_ends(struct bap_world * world);
int extend_horizontal(int row, int col, struct bap_world * world);
int extend_middle(int row, int col, struct bap_world * world);
int extend_middles(struct bap_world * world);
int extend_vertical(int row, int col, struct bap_world * world);
int fill_bap(struct bap_world * world);
int find_max_starts_col(int col, int size, int assigned, int * max_starts,
  struct bap_world * world);
int find_max_starts_row(int row, int size, int assigned, int * max_starts,
  struct bap_world * world);
int find_min_starts_col(int col, int size, int assigned, int * min_starts,
  struct bap_world * world);
int find_min_starts_row(int row, int size, int assigned, int * min_starts,
  struct bap_world * world);
int init_solution(struct bap_world * world);
int init_world(struct bap_world * world);
int insert_guess_col(struct guess * the_guess, struct bap_world * world);
int insert_guess_row(struct guess * the_guess, struct bap_world * world);
int insert_guess_square(int row, int col, char item1,
  struct bap_world * world);
int legal_col(char * test, int col, int * is_legal, struct bap_world * world);
int legal_col_across(char * test, int col, int * is_legal,
  struct bap_world * world);
int legal_col_beside(char * test, int col, int * is_legal,
  struct bap_world * world);
int legal_col_ships(char * test, int col, int * is_legal,
  struct bap_world * world);
int legal_row(char * test, int row, int * is_legal, struct bap_world * world);
int legal_row_across(char * test, int row, int * is_legal,
  struct bap_world * world);
int legal_row_beside(char * test, int row, int * is_legal,
  struct bap_world * world);
int legal_row_ships(char * test, int row, int * is_legal,
  struct bap_world * world);
int locate_ships(struct bap_world * world);
int main(int argc, char ** argv);
int make_guess_ship_col(int size, int col, int * number_guesses, int limit,
  struct guess * guesses, struct bap_world * world);
int make_guess_ship_row(int size, int row, int * number_guesses, int limit,
  struct guess * guesses, struct bap_world * world);
int make_guesses_battleships(int * number_guesses, struct guess * guesses,
  struct bap_world * world);
int make_guesses_cruisers(int * number_guesses, struct guess * guesses,
  struct bap_world * world);
int make_guesses_destroyers(int * number_guesses, struct guess * guesses,
  struct bap_world * world);
int make_guesses_submarines(int * number_guesses, struct guess * guesses,
  struct bap_world * world);
int make_water(int row, int col, struct bap_world * world);
int mark_dones(struct bap_world * world);
int pad_bap(struct bap_world * world);
int pad_square(char item, int row, int col, struct bap_world * world);
int place_battleships(struct bap_world * world);
int place_cruisers(struct bap_world * world);
int place_destroyers(struct bap_world * world);
int place_ships_col(int col, int size, int assigned,
  struct bap_world * world);
int place_ships_col_water(int col, int size, int assigned, int * min_starts,
  int * max_starts, int * placed, struct bap_world * world);
int place_ships_row(int row, int size, int assigned,
  struct bap_world * world);
int place_ships_row_water(int row, int size, int assigned, int * min_starts,
  int * max_starts, int * placed, struct bap_world * world);
int print_bap(struct bap_world * world);
int read_arguments(int argc, char ** argv, struct bap_world * world);
int read_column_numbers(FILE * in_port, char * buffer,
  struct bap_world * world);
int read_dashes(FILE * in_port, char * dashes, int * row_length,
  struct bap_world * world);
int read_problem(char * file_name, struct bap_world * world);
int read_row(char * buffer, struct bap_world * world);
int read_rows(FILE * in_port, char * buffer, int row_length,
  struct bap_world * world);
int read_ships(FILE * in_port, char * buffer, struct bap_world * world);
int record_progress(char * tried, struct bap_world * world);
int ship_covers_col(int row, int col, int size, int * cover,
  struct bap_world * world);
int ship_covers_row(int row, int col, int size, int * cover,
  struct bap_world * world);
int ship_fits_col(int col, int size, int * fits, struct bap_world * world);
int ship_fits_col_row(int col, int row, int size, int * ys, int * is_legal,
  struct bap_world * world);
int ship_fits_row(int row, int size, int * fits, struct bap_world * world);
int ship_fits_row_col(int row, int col, int size, int * ys, int * is_legal,
  struct bap_world * world);
int solve_problem(struct bap_world * world);
char square(int row, int col, struct bap_world * world);
int submarine_covers(int row, int col, int * cover, struct bap_world * world);
int try_full(struct bap_world * world);
int try_full_but_one(struct bap_world * world);
int try_full_but_one_col(int col, struct bap_world * world);
int try_full_but_one_row(int row, struct bap_world * world);
int try_guessing(struct bap_world * world);
int try_square_blank(struct bap_world * world);
int verify_square(char item, int row, int col, struct bap_world * world);
int verify_squares(struct bap_world * world);

/*************************************************************************/

/* globals

*/

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

Side Effects: This allocates a ship to a column. If one or more ships
of the given length are assigned to the column, this subtracts one
from the number assigned.  Otherwise, this checks that there is at
least one ship of the given type left to be located or assigned and
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
  const char * name SET_TO "allocate_ship_col";
  int newby;
  int switcher;            /* length of ship */

  newby SET_TO 1;
  if (NOT (world->locations[row][col]))
    {
      if (world->number_guesses_tried > 0)
	{
	  IFF(already_tried(row, col, length, 0, &newby, world));
	}
      if (newby)
	{
	  switcher SET_TO length;
	  if (switcher IS 2)
	    {
	      if (world->cols[col].destroyers > 0)
		{
		  world->cols[col].destroyers--;
		  world->destroyers_ass--;
		}
	      else
		{
		  CHK((world->destroyers_left < 1),
		      "Puzzle has no answer too many destroyers");
		  world->destroyers_left--;
		}
	      world->locations[row][col] SET_TO -2;
	    }
	  else if (switcher IS 3)
	    {
	      if (world->cols[col].cruisers > 0)
		{
		  world->cols[col].cruisers--;
		  world->cruisers_ass--;
		}
	      else
		{
		  CHK((world->cruisers_left < 1),
		      "Puzzle has no answer too many cruisers");
		  world->cruisers_left--;
		}
	      world->locations[row][col] SET_TO -3;
	    }
	  else if (switcher IS 4)
	    {
	      if (world->cols[col].battleships > 0)
		{
		  world->cols[col].battleships--;
		  world->battleships_ass--;
		}
	      else
		{
		  CHK((world->battleships_left < 1),
		      "Puzzle has no answer too many battleships");
		  world->battleships_left--;
		}
	      world->locations[row][col] SET_TO -4;
	    }
	  else if (switcher IS 1)
	    CHK(1, "Bug cannot allocate submarines to columns");
	  else
	    CHK(1, "Puzzle has no answer bad ship length");
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

Side Effects: This allocates a ship to a row. If one or more ships of
the given length are assigned to the row, this subtracts one from the
number assigned.  Otherwise, this checks that there is at least one
ship of the given type left to be located or assigned and subtracts
one from the number of such ships.

Notes:

Submarines lie fully in a row and a column. Other ship types lie fully
in either a row or a column but not both. To prevent double counting
of submarines, submarines may be allocated only to rows. This is
a limiting decision and should be changed, if convenient. The simplest
change would be to allow either rows or columns, but not both.

This sets world->progress to 1.

The "Bad ship length" message will be given if a ship more than
four squares long is given in the problem statement.

(move the following text somewhere else)
Conventions:
There are three levels of locating a ship.
a. unknown - ship accounted for only by xxxships_left.
b. in a row or column - ship is "assigned" to a row or column in the
   strip for the row or column. When a ship is assigned, world->xxxships_left
   is decremented and xxxships for the strip is incremented.
c. fully located - xxxships for the row or column is decremented
   if not zero. Otherwise world->xxxships_left is decremented.
   Evidence of the ship's existence no longer appears anywhere except
   the ship appears as it should in world->squares.

This is minimalist approach to data about ships and does not support
checking at end. May discover need to preserve more data about ship
location.

*/

int allocate_ship_row(     /* ARGUMENTS                  */
 int row,                  /* row of left end of ship    */
 int col,                  /* column of left end of ship */
 int length,               /* length of ship             */
 struct bap_world * world) /* puzzle model               */
{
  const char * name SET_TO "allocate_ship_row";
  int newby;
  int switcher;            /* length of ship */

  newby SET_TO 1;
  if (NOT (world->locations[row][col]))
    {
      if (world->number_guesses_tried > 0)
	{
	  IFF(already_tried(row, col, length, 1, &newby, world));
	}
      if (newby)
	{
	  switcher SET_TO length;
	  if (switcher IS 1)
	    {
	      if (world->rows[row].submarines > 0)
		{
		  world->rows[row].submarines--;
		  world->submarines_ass_row--;
		}
	      else
		{
		  CHK((world->submarines_left_row < 1),
		      "Puzzle has no answer too many submarines");
		  world->submarines_left_row--;
		}
	      if (world->cols[col].submarines > 0)
		{
		  world->cols[col].submarines--;
		  world->submarines_ass_col--;
		}
	      else
		{
		  CHK((world->submarines_left_col < 1),
		      "Puzzle has no answer too many submarines");
		  world->submarines_left_col--;
		}
	      world->locations[row][col] SET_TO 1;
	    }
	  else if (switcher IS 2)
	    {
	      if (world->rows[row].destroyers > 0)
		{
		  world->rows[row].destroyers--;
		  world->destroyers_ass--;
		}
	      else
		{
		  CHK((world->destroyers_left < 1),
		      "Puzzle has no answer too many destroyers");
		  world->destroyers_left--;
		}
	      world->locations[row][col] SET_TO 2;
	    }
	  else if (switcher IS 3)
	    {
	      if (world->rows[row].cruisers > 0)
		{
		  world->rows[row].cruisers--;
		  world->cruisers_ass--;
		}
	      else
		{
		  CHK((world->cruisers_left < 1),
		      "Puzzle has no answer too many cruisers");
		  world->cruisers_left--;
		}
	      world->locations[row][col] SET_TO 3;
	    }
	  else if (switcher IS 4)
	    {
	      if (world->rows[row].battleships > 0)
		{
		  world->rows[row].battleships--;
		  world->battleships_ass--;
		}
	      else
		{
		  CHK((world->battleships_left < 1),
		      "Puzzle has no answer too many battleships");
		  world->battleships_left--;
		}
	      world->locations[row][col] SET_TO 4;
	    }
	  else
	    CHK(1, "Bad ship length");
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

Side Effects:

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
  const char * name SET_TO "already_tried";
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

/* assign_battleships

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The unassigned battleships do not all fit somewhere in the puzzle:
     "Puzzle has no answer".
  2. ship_fits_row returns ERROR.
  3. ship_fits_col returns ERROR.
  4. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: This tries to assign battleships to strips. This
determines how many battleships will fit in each strip in addition to
any already located in the strip. Few decisions can be made if it is
known only that at least one can fit.

The number assigned to a strip is the minimum number that must be
assigned to the strip in any case. This is the number of battleships
left minus the number that fit in other strips.  The number that fit
in other strips is (of course) the total number that fit in all strips
minus the number that fit in the given strip.

Notes:

Note that being assigned does not guarantee that all assigned ships
will fit, because locating some may make it impossible to locate
others. This can happen only if the puzzle has no solution.

The ship_fits_row and ship_fits_col functions set the value of fits.

*/

int assign_battleships(     /* ARGUMENTS    */
 struct bap_world * world)  /* puzzle model */
{
  const char * name SET_TO "assign_battleships";
  int row;
  int col;
  int assign;
  int fits;                       /* number that fit in a strip             */
  int rows_fits[MAX_NUMBER_ROWS]; /* number unassigned that fit in each row */
  int cols_fits[MAX_NUMBER_COLS]; /* number unassigned that fit in each col */
  int total_fits;                 /* total number unassigned that fit       */
  int total_assigned;             /* total newly assigned                   */
  struct strip * rown;            /* an entire row being processed          */
  struct strip * coln;            /* an entire column being processed       */
  int spares;

  total_fits SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      rown SET_TO (world->rows + row);
      rows_fits[row] SET_TO 0;
      if (rown->done)
	continue;
      spares SET_TO
	((rown->aim) -
	 ((rown->cruisers * 3) + (rown->destroyers * 2) + (rown->submarines)));
      if (spares < 4)
	continue;
      IFF(ship_fits_row(row, 4, &fits, world));
      rows_fits[row] SET_TO
	(((fits - rown->battleships) > 0) ? (fits - rown->battleships) : 0);
      total_fits SET_TO (total_fits + rows_fits[row]);
    }
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      coln SET_TO (world->cols + col);
      cols_fits[col] SET_TO 0;
      if (coln->done)
	continue;
      spares SET_TO
	((coln->aim) -
	 ((coln->cruisers * 3) + (coln->destroyers * 2) + (coln->submarines)));
      if (spares < 4)
	continue;
      IFF(ship_fits_col(col, 4, &fits, world));
      cols_fits[col] SET_TO
	(((fits - coln->battleships) > 0) ? (fits - coln->battleships) : 0);
      total_fits SET_TO (total_fits + cols_fits[col]);
    }
  CHK((total_fits < world->battleships_left), "Puzzle has no answer");
  total_assigned SET_TO 0;
  world->number_in_rows SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      assign SET_TO (world->battleships_left - (total_fits - rows_fits[row]));
      if (assign > 0)
	{
	  world->rows[row].battleships SET_TO
	    (world->rows[row].battleships + assign);
	  world->rows_in[world->number_in_rows++] SET_TO row;
	  total_assigned SET_TO (total_assigned + assign);
	}
    }
  world->number_in_cols SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      assign SET_TO (world->battleships_left - (total_fits - cols_fits[col]));
      if (assign > 0)
	{
	  world->cols[col].battleships SET_TO
	    (world->cols[col].battleships + assign);
	  world->cols_in[world->number_in_cols++] SET_TO col;
	  total_assigned SET_TO (total_assigned + assign);
	}
    }
  if (total_assigned > 0)
    {
      world->progress SET_TO 1;
      IFF(record_progress("assign battleship to ", world));
      world->battleships_left SET_TO
	(world->battleships_left - total_assigned);
      world->battleships_ass SET_TO (world->battleships_ass + total_assigned);
    }
  return OK;
}

/*************************************************************************/

/* assign_cruisers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The unassigned cruisers do not all fit somewhere in the puzzle:
     "Puzzle has no answer"
  2. ship_fits_row returns ERROR.
  3. ship_fits_col returns ERROR.
  4. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: See assign_battleships

*/

int assign_cruisers(       /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "assign_cruisers";
  int row;                        /* index of row being processed           */
  int col;                        /* index of column being processed        */
  int assign;
  int fits;                       /* number that fit in a strip             */
  int rows_fits[MAX_NUMBER_ROWS]; /* number unassigned that fit in each row */
  int cols_fits[MAX_NUMBER_COLS]; /* number unassigned that fit in each col */
  int total_fits;                 /* total number unassigned that fit       */
  int total_assigned;             /* total newly assigned                   */
  struct strip * rown;            /* an entire row being processed          */
  struct strip * coln;            /* an entire column being processed       */
  int spares;

  total_fits SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      rown SET_TO (world->rows + row);
      rows_fits[row] SET_TO 0;
      if (rown->done)
	continue;
      spares SET_TO
	((rown->aim) -
	 ((rown->battleships * 4) +
	  (rown->destroyers * 2) +
	  (rown->submarines)));
      if (spares < 3)
	continue;
      IFF(ship_fits_row(row, 3, &fits, world));
      rows_fits[row] SET_TO
	(((fits - rown->cruisers) > 0) ? (fits - rown->cruisers) : 0);
      total_fits SET_TO (total_fits + rows_fits[row]);
    }
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      coln SET_TO (world->cols + col);
      cols_fits[col] SET_TO 0;
      if (coln->done)
	continue;
      spares SET_TO
	((coln->aim) -
	 ((coln->battleships * 4) +
	  (coln->destroyers * 2) +
	  (coln->submarines)));
      if (spares < 3)
	continue;
      IFF(ship_fits_col(col, 3, &fits, world));
      cols_fits[col] SET_TO
	(((fits - coln->cruisers) > 0) ? (fits - coln->cruisers) : 0);
      total_fits SET_TO (total_fits + cols_fits[col]);
    }
  CHK((total_fits < world->cruisers_left), "Puzzle has no answer");
  total_assigned SET_TO 0;
  world->number_in_rows SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      assign SET_TO (world->cruisers_left - (total_fits - rows_fits[row]));
      if (assign > 0)
	{
	  world->rows[row].cruisers SET_TO
	    (world->rows[row].cruisers + assign);
	  world->rows_in[world->number_in_rows++] SET_TO row;
	  total_assigned SET_TO (total_assigned + assign);
	}
    }
  world->number_in_cols SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      assign SET_TO (world->cruisers_left - (total_fits - cols_fits[col]));
      if (assign > 0)
	{
	  world->cols[col].cruisers SET_TO
	    (world->cols[col].cruisers + assign);
	  world->cols_in[world->number_in_cols++] SET_TO col;
	  total_assigned SET_TO (total_assigned + assign);
	}
    }
  if (total_assigned > 0)
    {
      world->progress SET_TO 1;
      IFF(record_progress("assign cruiser to ", world));
      world->cruisers_left SET_TO (world->cruisers_left - total_assigned);
      world->cruisers_ass SET_TO (world->cruisers_ass + total_assigned);
    }
  return OK;
}

/*************************************************************************/

/* assign_destroyers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The unassigned destroyers do not all fit somewhere in the puzzle:
     "Puzzle has no answer".
  2. ship_fits_row returns ERROR.
  3. ship_fits_col returns ERROR.
  4. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: See assign_battleships

*/

int assign_destroyers(     /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "assign_destroyers";
  int row;                        /* index of row being processed           */
  int col;                        /* index of column being processed        */
  int assign;
  int fits;                       /* number that fit in a strip             */
  int rows_fits[MAX_NUMBER_ROWS]; /* number unassigned that fit in each row */
  int cols_fits[MAX_NUMBER_COLS]; /* number unassigned that fit in each col */
  int total_fits;                 /* total number unassigned that fit       */
  int total_assigned;             /* total newly assigned                   */
  struct strip * rown;            /* an entire row being processed          */
  struct strip * coln;            /* an entire column being processed       */
  int spares;

  total_fits SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      rown SET_TO (world->rows + row);
      rows_fits[row] SET_TO 0;
      if (rown->done)
	continue;
      spares SET_TO
	((rown->aim) -
	 ((rown->battleships * 4) +
	  (rown->cruisers * 3) +
	  (rown->submarines)));
      if (spares < 2)
	continue;
      IFF(ship_fits_row(row, 2, &fits, world));
      rows_fits[row] SET_TO
	(((fits - rown->destroyers) > 0) ? (fits - rown->destroyers) : 0);
      total_fits SET_TO (total_fits + rows_fits[row]);
    }
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      coln SET_TO (world->cols + col);
      cols_fits[col] SET_TO 0;
      if (coln->done)
	continue;
      spares SET_TO
	((coln->aim) -
	 ((coln->battleships * 4) +
	  (coln->cruisers * 3) +
	  (coln->submarines)));
      if (spares < 2)
	continue;
      IFF(ship_fits_col(col, 2, &fits, world));
      cols_fits[col] SET_TO
	(((fits - coln->destroyers) > 0) ? (fits - coln->destroyers) : 0);
      total_fits SET_TO (total_fits + cols_fits[col]);
    }
  CHK((total_fits < world->destroyers_left), "Puzzle has no answer");
  total_assigned SET_TO 0;
  world->number_in_rows SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      assign SET_TO (world->destroyers_left - (total_fits - rows_fits[row]));
      if (assign > 0)
	{
	  world->rows[row].destroyers SET_TO
	    (world->rows[row].destroyers + assign);
	  world->rows_in[world->number_in_rows++] SET_TO row;
	  total_assigned SET_TO (total_assigned + assign);
	}
    }
  world->number_in_cols SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      assign SET_TO (world->destroyers_left - (total_fits - cols_fits[col]));
      if (assign > 0)
	{
	  world->cols[col].destroyers SET_TO
	    (world->cols[col].destroyers + assign);
	  world->cols_in[world->number_in_cols++] SET_TO col;
	  total_assigned SET_TO (total_assigned + assign);
	}
    }
  if (total_assigned > 0)
    {
      world->progress SET_TO 1;
      IFF(record_progress("assign destroyer to ", world));
      world->destroyers_left SET_TO (world->destroyers_left - total_assigned);
      world->destroyers_ass SET_TO (world->destroyers_ass + total_assigned);
    }
  return OK;
}

/*************************************************************************/

/* assign_submarines

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. assign_submarines_rows returns ERROR.
  2. assign_submarines_cols returns ERROR.
  3. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: This assigns submarines to rows and/or columns, if possible,
and reports progress, if any.

The first progress check looks for the case of more than one submarine
being assigned to a row or column and uses the plural "submarines" if
so.

*/

int assign_submarines(     /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "assign_submarines";
  int total_assigned_col; /* total submarines newly assigned to columns */
  int total_assigned_row; /* total submarines newly assigned to rows    */

  total_assigned_col SET_TO 0;
  total_assigned_row SET_TO 0;
  world->number_in_rows SET_TO 0;
  world->number_in_cols SET_TO 0;
  if (world->submarines_left_row ISNT 0)
    {
      IFF(assign_submarines_rows(&total_assigned_row, world));
      world->submarines_left_row SET_TO
	(world->submarines_left_row - total_assigned_row);
      world->submarines_ass_row SET_TO
	(world->submarines_ass_row + total_assigned_row);
    }
  if (world->submarines_left_col ISNT 0)
    {
      IFF(assign_submarines_cols(&total_assigned_col, world));
      world->submarines_left_col SET_TO
	(world->submarines_left_col - total_assigned_col);
      world->submarines_ass_col SET_TO
	(world->submarines_ass_col + total_assigned_col);
    }
  if ((total_assigned_row + total_assigned_col) >
      (world->number_in_rows + world->number_in_cols))
    {
      world->progress SET_TO 1;
      IFF(record_progress("assign submarines to ", world));
    }
  else if ((total_assigned_row + total_assigned_col) > 0)
    {
      world->progress SET_TO 1;
      IFF(record_progress("assign submarine to ", world));
    }
  return OK;
}

/*************************************************************************/

/* assign_submarines_cols

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_col returns ERROR.
  2. The unassigned submarines do not all fit somewhere in the puzzle:
     "Puzzle has no answer".

Called By:  assign_submarines

Side Effects: This assigns one or more submarines to a column if there
is nowhere else to put them.

If

1. no submarine is assigned to a column,
2. only one more ship part is needed to fill the column, and
3. every blank square of the column has water to the right and left of it and
   water or blank above and below it,

then the only way to fill the column is with a submarine, so a submarine
is assigned to the column.

This is not called unless world->submarines_left_col is not 0, so it
is not necessary to check that.

*/

int assign_submarines_cols( /* ARGUMENTS                                 */
 int * total_assigned,      /* total newly assigned submarines, set here */
 struct bap_world * world)  /* puzzle model                              */
{
  const char * name SET_TO "assign_submarines_cols";
  int row;                        /* index of row being processed           */
  int col;                        /* index of column being processed        */
  int assign;
  int fits;                       /* number that fit in a strip             */
  int cols_fits[MAX_NUMBER_COLS]; /* number unassigned that fit in each col */
  int total_fits;                 /* total number that fit in all columns   */
  int spares;
  int singles;                    /* 1= no two consecutive spaces           */
  struct strip * coln;

  total_fits SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      coln SET_TO (world->cols + col);
      cols_fits[col] SET_TO 0;
      if (coln->done)
	continue;
      spares SET_TO
	((coln->aim) -
	 ((coln->battleships * 4) +
	  (coln->cruisers * 3) +
	  (coln->destroyers * 2)));
      if (spares < 1)
	continue;
      IFF(ship_fits_col(col, 1, &fits, world));
      if ((fits - coln->submarines) > 0)
	{
	  cols_fits[col] SET_TO (fits - coln->submarines);
	  total_fits SET_TO (total_fits + cols_fits[col]);
	}
    }
  CHK((total_fits < world->submarines_left_col), "Puzzle has no answer");
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      coln SET_TO (world->cols + col);
      assign SET_TO
	(world->submarines_left_col - (total_fits - cols_fits[col]));
      if (assign > 0)
	{
	  coln->submarines SET_TO (coln->submarines + assign);
	  *total_assigned SET_TO (*total_assigned + assign);
	  world->cols_in[world->number_in_cols++] SET_TO col;
	}
      else
	{
	  singles SET_TO 1;
	  for (row SET_TO 0; row < world->number_rows; row++)
	    {
	      if (square(row, col, world) IS ' ')
		{
		  if (((square(row - 1, col, world) ISNT '.') AND
		       (square(row - 1, col, world) ISNT ' ')) OR
		      ((square(row + 1, col, world) ISNT '.') AND
		       (square(row + 1, col, world) ISNT ' ')) OR
		      (square(row, col - 1, world) ISNT '.') OR
		      (square(row, col + 1, world) ISNT '.'))
		    break;
		  if ((square(row - 1, col, world) ISNT '.') OR
		      (square(row + 1, col, world) ISNT '.'))
		    singles SET_TO 0;
		}
	    }
	  if (row IS world->number_rows)
	    {
	      assign SET_TO 0;
	      if ((coln->aim - coln->got) IS 1)
		assign SET_TO (1 - coln->submarines);
	      else if (singles)
		assign SET_TO ((coln->aim - coln->got) - coln->submarines);
	      if (assign > 0)
		{
		  coln->submarines SET_TO (coln->submarines + assign);
		  *total_assigned SET_TO (*total_assigned + assign);
		  world->cols_in[world->number_in_cols++] SET_TO col;
		}
	    }
	}
    }
  return OK;
}

/*************************************************************************/

/* assign_submarines_rows

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_row returns ERROR.
  2. The unassigned submarines do not all fit somewhere in the puzzle:
     "Puzzle has no answer".

Called By:  assign_submarines

Side Effects: This assigns one or more submarines to a row if there is
nowhere else to put them.

If

1. no submarine is assigned to a row,
2. only one more ship part is needed to fill the row, and
3. every blank square of the row has water above and below it and
   water or blank before and after it,

then the only way to fill the row is with a submarine, so a submarine
is assigned to the row.

Notes:

This is not called unless world->submarines_left_row is not 0, so it
is not necessary to check that.

The ship_fits_row function sets the value of fits.

*/

int assign_submarines_rows( /* ARGUMENTS                                 */
 int * total_assigned,      /* total newly assigned submarines, set here */
 struct bap_world * world)  /* puzzle model                              */
{
  const char * name SET_TO "assign_submarines_rows";
  int row;                        /* index of row being processed           */
  int col;                        /* index of column being processed        */
  int assign;
  int fits;                       /* number that fit in a strip             */
  int rows_fits[MAX_NUMBER_ROWS]; /* number unassigned that fit in each row */
  int total_fits;                 /* total number that fit in all rows      */
  int spares;
  int singles;                    /* 1= no two consecutive spaces           */
  struct strip * rown;

  total_fits SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      rown SET_TO (world->rows + row);
      rows_fits[row] SET_TO 0;
      if (rown->done)
	continue;
      spares SET_TO
	((rown->aim) -
	 ((rown->battleships * 4) +
	  (rown->cruisers * 3) +
	  (rown->destroyers * 2)));
      if (spares < 1)
	continue;
      IFF(ship_fits_row(row, 1, &fits, world));
      if ((fits - rown->submarines) > 0)
	{
	  rows_fits[row] SET_TO (fits - rown->submarines);
	  total_fits SET_TO (total_fits + rows_fits[row]);
	}
    }
  CHK((total_fits < world->submarines_left_row), "Puzzle has no answer");
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      rown SET_TO (world->rows + row);
      assign SET_TO
	(world->submarines_left_row - (total_fits - rows_fits[row]));
      if (assign > 0)
	{
	  rown->submarines SET_TO (rown->submarines + assign);
	  *total_assigned SET_TO (*total_assigned + assign);
	  world->rows_in[world->number_in_rows++] SET_TO row;
	}
      else
	{
	  singles SET_TO 1;
	  for (col SET_TO 0; col < world->number_cols; col++)
	    {
	      if (square(row, col, world) IS ' ')
		{
		  if (((square(row, col - 1, world) ISNT '.') AND
		       (square(row, col - 1, world) ISNT ' ')) OR
		      ((square(row, col + 1, world) ISNT '.') AND
		       (square(row, col + 1, world) ISNT ' ')) OR
		      (square(row - 1, col, world) ISNT '.') OR
		      (square(row + 1, col, world) ISNT '.'))
		    break;
		  if ((square(row, col - 1, world) ISNT '.') OR
		      (square(row, col + 1, world) ISNT '.'))
		    singles SET_TO 0;
		}
	    }
	  if (col IS world->number_cols)
	    {
	      assign SET_TO 0;
	      if ((rown->aim - rown->got) IS 1)
		assign SET_TO (1 - rown->submarines);
	      else if (singles)
		assign SET_TO ((rown->aim - rown->got) - rown->submarines);
	      if (assign > 0)
		{
		  rown->submarines SET_TO (rown->submarines + assign);
		  *total_assigned SET_TO (*total_assigned + assign);
		  world->rows_in[world->number_in_rows++] SET_TO row;
		}
	    }
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
  2. copy_guesses returns ERROR.

Called By:  try_guessing

Side Effects: This copies the world model into the world_copy model,
except that rows_in and cols_in are not copied.

*/

int copy_bap(                   /* ARGUMENTS                      */
 struct bap_world * world_copy, /* puzzle model being copied into */
 struct bap_world * world)      /* puzzle model being copied from */
{
  const char * name SET_TO "copy_bap";
  int row;         /* index of row being processed         */
  int col;         /* index of column being processed      */
  int n;           /* index of other array being processed */

  world_copy->battleships_ass SET_TO world->battleships_ass;
  world_copy->battleships_left SET_TO world->battleships_left;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      IFF(copy_strip(&(world_copy->cols[col]), &(world->cols[col])));
    }
  world_copy->cruisers_ass SET_TO world->cruisers_ass;
  world_copy->cruisers_left SET_TO world->cruisers_left;
  world_copy->destroyers_ass SET_TO world->destroyers_ass;
  world_copy->destroyers_left SET_TO world->destroyers_left;
  world_copy->exes SET_TO world->exes;
  world_copy->find_all SET_TO world->find_all;
  for (n SET_TO 0; n < world->logic_line; n++)
    {
      strcpy(world_copy->logic[n], world->logic[n]);
    }
  IFF(copy_guesses(world_copy->guesses_tried, world->guesses_tried,
		   world->number_guesses_tried));
  world_copy->logic_line SET_TO world->logic_line;
  world_copy->number_cols SET_TO world->number_cols;
  world_copy->number_in_cols SET_TO 0;
  world_copy->number_guesses_tried SET_TO world->number_guesses_tried;
  world_copy->number_rows SET_TO world->number_rows;
  world_copy->number_in_rows SET_TO 0;
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
  world_copy->submarines_ass_col SET_TO world->submarines_ass_col;
  world_copy->submarines_ass_row SET_TO world->submarines_ass_row;
  world_copy->submarines_left_col SET_TO world->submarines_left_col;
  world_copy->submarines_left_row SET_TO world->submarines_left_row;
  world_copy->undone SET_TO world->undone;
  world_copy->verbose SET_TO world->verbose;
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
  const char * name SET_TO "copy_guess";

  guess_copy->row SET_TO guess_original->row;
  guess_copy->col SET_TO guess_original->col;
  guess_copy->size SET_TO guess_original->size;
  guess_copy->in_row SET_TO guess_original->in_row;
  return OK;
}

/*************************************************************************/

/* copy_guesses

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. copy_guess returns ERROR. (not currently checked)

Called By:
  copy_bap
  make_guesses_battleships
  make_guesses_cruisers
  make_guesses_destroyers
  make_guesses_submarines

Side Effects: This copies the first number_guesses entries from the
guesses array into the guesses_copy array.

*/

int copy_guesses(             /* ARGUMENTS                     */
 struct guess * guesses_copy, /* array of guesses to copy into */
 struct guess * guesses,      /* array of guesses to copy from */
 int number_guesses)          /* number of guesses to copy     */
{
  const char * name SET_TO "copy_guesses";
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

Called By:  copy_bap

Side Effects: This copies the strip_old into the strip_copy.

*/

int copy_strip(             /* ARGUMENTS          */
 struct strip * strip_copy, /* strip to copy into */
 struct strip * strip_old)  /* strip to copy from */
{
  const char * name SET_TO "copy_strip";

  strip_copy->aim SET_TO strip_old->aim;
  strip_copy->battleships SET_TO strip_old->battleships;
  strip_copy->cruisers SET_TO strip_old->cruisers;
  strip_copy->destroyers SET_TO strip_old->destroyers;
  strip_copy->done SET_TO strip_old->done;
  strip_copy->dry SET_TO strip_old->dry;
  strip_copy->exes SET_TO strip_old->exes;
  strip_copy->got SET_TO strip_old->got;
  strip_copy->submarines SET_TO strip_old->submarines;
  return OK;
}

/*************************************************************************/

/* dex_ends_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. make_water returns ERROR.

Called By:  dex_squares

Side Effects: This puts water at the ends of sets of consecutive ship
parts if they are ships.

Notes:

See documentation of dex_squares.

This iterates beyond the end of the column so that no special code is
required for what to do at the end.

*/

int dex_ends_col(          /* ARGUMENTS                        */
 int col,                  /* index of column being processed  */
 struct bap_world * world) /* puzzle model                     */
{
  const char * name SET_TO "dex_ends_col";
  int row;        /* index of row being processed     */
  int length;     /* number of consecutive ship parts */

  length SET_TO 0;
  for (row SET_TO 0; row <= world->number_rows; row++)
    {
      if ((square(row, col, world) IS '.') OR (square(row, col, world) IS ' '))
	{
	  if ((length IS 4) OR
	      ((length IS 3) AND
	       (((world->cols[col].cruisers) AND
		 (world->cols[col].aim < 7)) OR
		((world->cols[col].battleships IS 0) AND
		 (world->battleships_left IS 0)))) OR
	      ((length IS 2) AND
	       (((world->cols[col].destroyers) AND
		 (world->cols[col].aim < 5)) OR
		((world->cols[col].battleships IS 0) AND
		 (world->battleships_left IS 0) AND
		 (world->cols[col].cruisers IS 0) AND
		 (world->cruisers_left IS 0)))))
	    {
	      if (square(row, col, world) IS ' ')
		IFF(make_water(row, col, world));
	      if (square((row - length - 1), col, world) IS ' ')
		IFF(make_water((row - length - 1), col, world));
	    }
	  length SET_TO 0;
	}
      else
	length++;
    }
  return OK;
}

/*************************************************************************/

/* dex_ends_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. make_water returns ERROR.

Called By:  dex_squares

Side Effects: This puts water at the ends of sets of consecutive ship
parts if they are ships.

Notes:

See documentation of dex_squares.

This iterates beyond the end of the row so that no special code is
required for what to do at the end.

*/

int dex_ends_row(          /* ARGUMENTS                     */
 int row,                  /* index of row being processed  */
 struct bap_world * world) /* puzzle model                  */
{
  const char * name SET_TO "dex_ends_row";
  int col;        /* index of column being processed  */
  int length;     /* number of consecutive ship parts */

  length SET_TO 0;
  for (col SET_TO 0; col <= world->number_cols; col++)
    {
      if ((square(row, col, world) IS '.') OR (square(row, col, world) IS ' '))
	{
	  if ((length IS 4) OR
	      ((length IS 3) AND
	       (((world->rows[row].cruisers) AND
		 (world->rows[row].aim < 7)) OR
		((world->rows[row].battleships IS 0) AND
		 (world->battleships_left IS 0)))) OR
	      ((length IS 2) AND
	       (((world->rows[row].destroyers) AND
		 (world->rows[row].aim < 5)) OR
		((world->rows[row].battleships IS 0) AND
		 (world->battleships_left IS 0) AND
		 (world->rows[row].cruisers IS 0) AND
		 (world->cruisers_left IS 0)))))
	    {
	      if (square(row, col, world) IS ' ')
		IFF(make_water(row, col, world));
	      if (square(row, (col - length - 1), world) IS ' ')
		IFF(make_water(row, (col - length - 1), world));
	    }
	  length SET_TO 0;
	}
      else
	length++;
    }
  return OK;
}

/*************************************************************************/

/* dex_square

Returned Value: int (OK)

Called By:  dex_squares

Side Effects: This changes an X to a known ship part if it is possible
to infer what the X must be by looking at the squares above, below,
right, and left of the given square. If the X is changed, exes for the
row, column and whole puzzle are updated, and world->progress is set to 1.

*/

int dex_square(            /* ARGUMENTS                        */
 int row,                  /* index of row being processed     */
 int col,                  /* index of column being processed  */
 struct bap_world * world) /* puzzle model                     */
{
  const char * name SET_TO "dex_square";
  char up;
  char down;
  char left;
  char right;

  up SET_TO square(row - 1, col, world);
  down SET_TO square(row + 1, col, world);
  left SET_TO square(row, col - 1, world);
  right SET_TO square(row, col + 1, world);
  if ((up IS '.') AND (down IS '.') AND (left IS '.') AND (right IS '.'))
    {
      world->squares[row][col] SET_TO 'O';
      world->rows[row].exes--;
      world->cols[col].exes--;
      world->exes--;
      world->progress SET_TO 1;
    }
  else if ((up IS '.') AND ((down IS 'v') OR (down IS '+') OR (down IS 'X')))
    {
      world->squares[row][col] SET_TO '^';
      world->rows[row].exes--;
      world->cols[col].exes--;
      world->exes--;
      world->progress SET_TO 1;
    }
  else if ((down IS '.') AND ((up IS '^') OR (up IS '+') OR (up IS 'X')))
    {
      world->squares[row][col] SET_TO 'v';
      world->rows[row].exes--;
      world->cols[col].exes--;
      world->exes--;
      world->progress SET_TO 1;
    }
  else if ((left IS '.') AND
	   ((right IS '>') OR (right IS '+') OR (right IS 'X')))
    {
      world->squares[row][col] SET_TO '<';
      world->rows[row].exes--;
      world->cols[col].exes--;
      world->exes--;
      world->progress SET_TO 1;
    }
  else if ((right IS '.') AND
	   ((left IS '<') OR (left IS '+') OR (left IS 'X')))
    {
      world->squares[row][col] SET_TO '>';
      world->rows[row].exes--;
      world->cols[col].exes--;
      world->exes--;
      world->progress SET_TO 1;
    }
  else if ((((up IS '^') OR (up IS 'X') OR (up IS '+')) AND
	    ((down IS 'v') OR (down IS 'X') OR (down IS '+'))) OR
	   (((left IS '<') OR (left IS 'X') OR (left IS '+')) AND
	    ((right IS '>') OR (right IS 'X') OR (right IS '+'))))
    {
      world->squares[row][col] SET_TO '+';
      world->rows[row].exes--;
      world->cols[col].exes--;
      world->exes--;
      world->progress SET_TO 1;
    }
  return OK;
}

/*************************************************************************/

/* dex_squares

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. dex_square returns ERROR.
  2. dex_ends_row returns ERROR.
  3. dex_ends_col returns ERROR.

Called By:  solve_problem

Side Effects: This changes all X's in the puzzle to known ship parts,
if possible, and puts water next to any ship ends it makes from Xs, if
water is not already there. See dex_square and the Notes here for
details. If any squares are de-exed, this reports that fact.

Notes:

There may be situations in which calling dex_squares twice in a row will
produce changes both times. Have not yet found an example or disproved.

For the purpose of reporting progress, this assumes world->progress is
set to 0 before it starts.

This checks if several ship parts next to each other along a strip
make a ship. If so, it changes the ends and middles if necessary, and
puts water next to the ends if water is not already there.

A set of adjacent ship parts in a strip makes a ship if any of
the following apply:
1. there are four ship parts - must be a battleship.
2. there are three ship parts and either:
   a. a cruiser is assigned to the strip, and the aim for strip is
      less than 7, or
   b. no larger ship is available to go in the strip,
   in both cases, there cannot be a larger ship covering the three ship parts.
3. there are two ship parts and either:
   a. a destroyer is assigned to the strip, and the aim for strip is
      less than 5, or
   b. no larger ship is available to go in the strip,
   in both cases there cannot be a larger ship covering the two ship parts.

There are other cases in which the adjacent squares must be a ship.
The simple ones are picked up by other functions if the strip is full
(try_full) or only one more ship part is needed in the strip
(try_full_but_one).

In the configurations: "X XXX" and "XX XX", the blank space in the
middle will be made water by try_square_blank. Then dex_square will
recognize that the Xs on either side of the blank square that are part
of XXX or XX are ship ends.

The two dex_ends_... functions just put water at the ends. Then dex_square
changes the Xs.

*/

int dex_squares(           /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "dex_squares";
  int row;       /* index of row being processed     */
  int col;       /* index of column being processed  */

  if (world->exes)
    {
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  if (world->rows[row].exes)
	    IFF(dex_ends_row(row, world));
	}
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  if (world->cols[col].exes)
	    IFF(dex_ends_col(col, world));
	}
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  if (world->rows[row].exes)
	    {
	      for (col SET_TO 0; col < world->number_cols; col++)
		{
		  if (world->squares[row][col] IS 'X')
		    IFF(dex_square(row, col, world));
		}
	    }
	}
    }
  if (world->progress)
    {
      strcpy(world->logic[world->logic_line++], "identify ship parts");
      if (world->print_all)
	{
	  print_bap(world);
	  printf("%s\n\n", world->logic[world->logic_line - 1]);
	}
    }
  return OK;
}

/*************************************************************************/

/* enx_square

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. make_water returns ERROR.

Called By:
  extend_ends
  extend_horizontal
  extend_vertical
  place_ships_col
  place_ships_row
  try_full
  try_full_but_one_col
  try_full_but_one_row

Side Effects: This puts an X in the square world->squares[row][col],
puts water in the squares adjacent to its corners, updates the
row and column got for the square, updates the row and column exes
for the square, updates the exes for the puzzle, and sets world->progress
to 1.

*/

int enx_square(            /* ARGUMENTS                        */
 int row,                  /* index of row being processed     */
 int col,                  /* index of column being processed  */
 struct bap_world * world) /* puzzle model                     */
{
  const char * name SET_TO "enx_square";

  world->squares[row][col] SET_TO 'X';
  world->rows[row].exes++;
  world->cols[col].exes++;
  world->exes++;
  world->rows[row].got++;
  world->cols[col].got++;
  IFF(make_water(row - 1, col - 1, world));
  IFF(make_water(row - 1, col + 1, world));
  IFF(make_water(row + 1, col - 1, world));
  IFF(make_water(row + 1, col + 1, world));
  world->progress SET_TO 1;
  return OK;
}

/*************************************************************************/

/* explain_bap

Returned Value: int (OK)

Called By:  solve_problem

Side Effects: This prints the explanation of the steps in solving the
puzzle, which has been accumulated in the world->logic array of strings.

*/

int explain_bap(           /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "explain_bap";
  int line;      /* index of logic line */

  printf("\nEXPLANATION\n-----------\n");
  for (line SET_TO 0; line < world->logic_line; line++)
    printf("%s\n", world->logic[line]);
  return OK;
}

/*************************************************************************/

/* extend_ends

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. enx_square returns ERROR.

Called By:  init_solution

Side Effects: This looks at ship ends and extends them if
possible. If any ship end is extended, this reports that fact.

Notes:

This needs to be called only once, since an end with a blank square
by its open end can only occur in the original puzzle statement. The
solution process does not create an end with a blank by its open end.

*/

int extend_ends(           /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "extend_ends";
  int row;
  int col;
  char item;

  world->progress SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  item SET_TO world->squares[row][col];
	  if (item IS '<')
	    {
	      if (square(row, col + 1, world) IS ' ')
		IFF(enx_square(row, col + 1, world));
	    }
	  else if (item IS '>')
	    {
	      if (square(row, col - 1, world) IS ' ')
		IFF(enx_square(row, col - 1, world));
	    }
	  else if (item IS 'v')
	    {
	      if (square(row - 1, col, world) IS ' ')
		IFF(enx_square(row - 1, col, world));
	    }
	  else if (item IS '^')
	    {
	      if (square(row + 1, col, world) IS ' ')
		IFF(enx_square(row + 1, col, world));
	    }
	}
    }
  if (world->progress)
    {
      strcpy(world->logic[world->logic_line++], "extend ship ends");
      if (world->print_all)
	{
	  print_bap(world);
	  printf("%s\n\n", world->logic[world->logic_line - 1]);
	}
    }
  return OK;
}

/*************************************************************************/

/* extend_horizontal

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. make_water returns ERROR.
  2. enx_square returns ERROR.
  3. record_progress returns ERROR.

Called By:  extend_middle

Side Effects: This extends a ship middle horizontally. The squares
above and below the ship middle are set to water if not already
water. The squares right and left, if blank, are set to unknown ship
part and are padded with water.

*/

int extend_horizontal(     /* ARGUMENTS                                      */
 int row,                  /* the row in which the ship middle is located    */
 int col,                  /* the column in which the ship middle is located */
 struct bap_world * world) /* puzzle model                                   */
{
  const char * name SET_TO "extend_horizontal";

  CHK(((square(row, col + 1, world) IS '.') OR
       (square(row, col - 1, world) IS '.')), "Puzzle has no answer");
  IFF(make_water(row - 1, col, world));
  IFF(make_water(row + 1, col, world));
  if (square(row, col + 1, world) IS ' ')
    IFF(enx_square(row, col + 1, world));
  if (square(row, col - 1, world) IS ' ')
    IFF(enx_square(row, col - 1, world));
  if (world->progress)
    {
      world->number_in_rows SET_TO 1;
      world->number_in_cols SET_TO 1;
      world->rows_in[0] SET_TO row;
      world->cols_in[0] SET_TO col;
      IFF(record_progress("extend in row middle at ", world));
    }
  return OK;
}

/*************************************************************************/

/* extend_middle

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. extend_horizontal returns ERROR.
  2. extend_vertical returns ERROR.

Called By:  extend_middles

Side Effects: This extends one middle if possible. If a ship middle is
extended, this reports that fact.

*/

int extend_middle(         /* ARGUMENTS                 */
 int row,                  /* index of row of middle    */
 int col,                  /* index of column of middle */
 struct bap_world * world) /* puzzle model              */
{
  const char * name SET_TO "extend_middle";

  if ((square(row - 1, col, world) IS '.') OR
      (square(row + 1, col, world) IS '.'))
    IFF(extend_horizontal(row, col, world));
  else if ((square(row, col - 1, world) IS '.') OR
	   (square(row, col + 1, world) IS '.'))
    IFF(extend_vertical(row, col, world));
  else if ((world->cols[col].aim - world->cols[col].got) < 2)
    IFF(extend_horizontal(row, col, world));
  else if ((world->rows[row].aim - world->rows[row].got) < 2)
    IFF(extend_vertical(row, col, world));
  return OK;
}

/*************************************************************************/

/* extend_middles

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. extend_middle returns ERROR.

Called By:  solve_problem

Side Effects: This looks at ship middles and extends one middle if
possible. If a ship middle is extended, this reports that fact.

*/

int extend_middles(        /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "extend_middles";
  int row;
  int col;

  for (row SET_TO 0; row < world->number_rows; row++)
    {
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  if (world->squares[row][col] IS '+')
	    IFF(extend_middle(row, col, world));
	  if (world->progress)
	    break;
	}
      if (world->progress)
	break;
    }
  return OK;
}

/*************************************************************************/

/* extend_vertical

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. make_water returns ERROR.
  2. enx_square returns ERROR.
  3. record_progress returns ERROR.

Called By:  extend_middle

Side Effects: This extends a ship middle vertically. The squares to
the right and left of the ship middle are set to water if not already
water. The squares above and below, if blank, are set to unknown ship
part and are padded with water.

*/

int extend_vertical(       /* ARGUMENTS                                      */
 int row,                  /* the row in which the ship middle is located    */
 int col,                  /* the column in which the ship middle is located */
 struct bap_world * world) /* puzzle model                                   */
{
  const char * name SET_TO "extend_vertical";

  IFF(make_water(row, col - 1, world));
  IFF(make_water(row, col + 1, world));
  if (square(row - 1, col, world) IS ' ')
    IFF(enx_square(row - 1, col, world));
  if (square(row + 1, col, world) IS ' ')
    IFF(enx_square(row + 1, col, world));
  if (world->progress)
    {
      world->number_in_rows SET_TO 1;
      world->number_in_cols SET_TO 1;
      world->rows_in[0] SET_TO row;
      world->cols_in[0] SET_TO col;
      IFF(record_progress("extend in column middle at ", world));
    }
  return OK;
}

/*************************************************************************/

/* fill_bap

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. make_water returns ERROR.
  2. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: If any strip (row or column) has (got = aim) but (dry >
aim), this fills the empty squares of the strip with water. This also
reports any strip that is filled.

Notes:

For the purposes of reporting, this assumes world->progress is 0 before it
starts.

*/

int fill_bap(              /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "fill_bap";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  world->number_in_cols SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if ((world->rows[row].aim IS world->rows[row].got) AND
	  (world->rows[row].aim ISNT world->rows[row].dry))
	{
	  for (col SET_TO 0; col < world->number_cols; col++)
	    {
	      if (world->squares[row][col] IS ' ')
		IFF(make_water(row, col, world));
	    }
	  world->rows_in[world->number_in_rows++] SET_TO row;
	}
    }
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if ((world->cols[col].aim IS world->cols[col].got) AND
	  (world->cols[col].aim ISNT world->cols[col].dry))
	{
	  for (row SET_TO 0; row < world->number_rows; row++)
	    {
	      if (world->squares[row][col] IS ' ')
		IFF(make_water(row, col, world));
	    }
	  world->cols_in[world->number_in_cols++] SET_TO col;
	}
    }
  IFF(record_progress("fill with water ", world));
  return OK;
}

/*************************************************************************/

/* find_max_starts_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_col_row returns ERROR.
  2. The assigned ships do not fit in the column: "Puzzle has no answer".

Called By:  place_ships_col

Side Effects:  The max_starts array is filled in with the row
numbers of squares where successive ships of the given size and not
already located can last be started.

Notes:

ys is used only because ship_fits_col_row requires it.

Where row is set to (row - size), we really want it set to (row - size - 1)
but row-- gets the 1 when the loop loops.

*/

int find_max_starts_col(   /* ARGUMENTS                                     */
 int col,                  /* column for which max_starts are being found   */
 int size,                 /* size of assigned ships                        */
 int assigned,             /* number of ships of given size assigned to col */
 int * max_starts,         /* array of maximum starting locations, set here */
 struct bap_world * world) /* puzzle model                                  */
{
  const char * name SET_TO "find_max_starts_col";
  int row;
  int places;
  int ys;
  int is_legal;

  places SET_TO assigned;
  for (row SET_TO (world->number_rows - size);
       ((row > -1) AND (places ISNT 0));
       row--)
    {
      IFF(ship_fits_col_row(col, row, size, &ys, &is_legal, world));
      if (is_legal)
	{
	  if (NOT (world->locations[row][col]))
	    {
	      places--;
	      max_starts[places] SET_TO row;
	    }
	  row SET_TO (row - size);
	}
    }
  CHK((places ISNT 0), "Puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* find_max_starts_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_row_col returns ERROR.
  2. The assigned ships do not fit in the row: "Puzzle has no answer"

Called By:  place_ships_row

Side Effects:  The max_starts array is filled in with the column
numbers of squares where successive ships of the given size and not
already located can last be started.

Notes:

ys is used only because ship_fits_row_col requires it.

Where col is set to (col - size), we really want it set to (col - size - 1)
but col-- gets the 1 when the loop loops.

*/

int find_max_starts_row(   /* ARGUMENTS                                     */
 int row,                  /* row for which max_starts are being found      */
 int size,                 /* size of assigned ships                        */
 int assigned,             /* number of ships of given size assigned to row */
 int * max_starts,         /* array of maximum starting locations, set here */
 struct bap_world * world) /* puzzle model                                  */
{
  const char * name SET_TO "find_max_starts_row";
  int col;
  int places;
  int ys;
  int is_legal;

  places SET_TO assigned;
  for (col SET_TO (world->number_cols - size);
       ((col > -1) AND (places ISNT 0));
       col--)
    {
      IFF(ship_fits_row_col(row, col, size, &ys, &is_legal, world));
      if (is_legal)
	{
	  if (NOT (world->locations[row][col]))
	    {
	      places--;
	      max_starts[places] SET_TO col;
	    }
	  col SET_TO (col - size);
	}
    }
  CHK((places ISNT 0), "Puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* find_min_starts_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_col_row returns ERROR.
  2. The assigned ships do not fit in the column: "Puzzle has no answer".

Called By:  place_ships_col

Side Effects: The min_starts array is filled in with the row
numbers of squares where successive ships of the given size and not
already located can first be started.

Notes:

ys is used only because ship_fits_col_row requires it.

Where row is set to (row + size), we really want it set to (row + size + 1)
but row++ gets the 1 when the loop loops.

*/

int find_min_starts_col(   /* ARGUMENTS                                     */
 int col,                  /* column for which min_starts are being found   */
 int size,                 /* size of assigned ships                        */
 int assigned,             /* number of ships of given size assigned to col */
 int * min_starts,         /* array of minimum starting locations, set here */
 struct bap_world * world) /* puzzle model                                  */
{
  const char * name SET_TO "find_min_starts_col";
  int row;
  int stop;
  int places;
  int ys;
  int is_legal;

  stop SET_TO (world->number_rows - (size - 1));
  places SET_TO 0;
  for (row SET_TO 0; ((row < stop) AND (places < assigned)); row++)
    {
      IFF(ship_fits_col_row(col, row, size, &ys, &is_legal, world));
      if (is_legal)
	{
	  if (NOT (world->locations[row][col]))
	    min_starts[places++] SET_TO row;
	  row SET_TO (row + size);
	}
    }
  CHK((places < assigned), "Puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* find_min_starts_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_row_col returns ERROR.
  2. The assigned ships do not fit in the row: "Puzzle has no answer".

Called By:  place_ships_row

Side Effects: The min_starts array is filled in with the column
numbers of squares where successive ships of the given size and not
already located can first be started.

Notes:

ys is used only because ship_fits_row_col requires it.

Where col is set to (col + size), we really want it set to (col + size + 1)
but col++ gets the 1 when the loop loops.

*/

int find_min_starts_row(   /* ARGUMENTS                                     */
 int row,                  /* row for which min_starts are being found      */
 int size,                 /* size of assigned ships                        */
 int assigned,             /* number of ships of given size assigned to row */
 int * min_starts,         /* array of minimum starting locations, set here */
 struct bap_world * world) /* puzzle model                                  */
{
  const char * name SET_TO "find_min_starts_row";
  int col;
  int stop;
  int places;
  int ys;
  int is_legal;

  stop SET_TO (world->number_cols - (size - 1));
  places SET_TO 0;
  for (col SET_TO 0; ((col < stop) AND (places < assigned)); col++)
    {
      IFF(ship_fits_row_col(row, col, size, &ys, &is_legal, world));
      if (is_legal)
	{
	  if (NOT (world->locations[row][col]))
	    min_starts[places++] SET_TO col;
	  col SET_TO (col + size);
	}
    }
  CHK((places < assigned), "Puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* init_solution

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. extend_ends returns ERROR.
  2. pad_bap returns ERROR.

Called By:  main

Side Effects: Ship ends given in the puzzle statement are extended, and
water is marked around any given ship parts.

*/

int init_solution(         /* ARGUMENTS     */
 struct bap_world * world) /* puzzle model  */
{
  const char * name SET_TO "init_solution";

  IFF(extend_ends(world));
  IFF(pad_bap(world));
  return OK;
}

/*************************************************************************/

/* init_world

Returned Value: int (OK)

Called By:  main

Side Effects: This sets all ints and chars in the world model and
its components to zero, except that the squares are set to blanks.

*/

int init_world(            /* ARGUMENTS                     */
 struct bap_world * world) /* puzzle model, filled in here  */
{
  const char * name SET_TO "init_world";
  int row;
  int col;
  int n;
  int m;

  world->battleships_ass SET_TO 0;
  world->battleships_left SET_TO 0;
  for (col SET_TO 0; col < MAX_NUMBER_COLS; col++)
    {
      world->cols[col].aim SET_TO 0;
      world->cols[col].battleships SET_TO 0;
      world->cols[col].cruisers SET_TO 0;
      world->cols[col].destroyers SET_TO 0;
      world->cols[col].done SET_TO 0;
      world->cols[col].dry SET_TO 0;
      world->cols[col].exes SET_TO 0;
      world->cols[col].got SET_TO 0;
      world->cols[col].submarines SET_TO 0;
      world->cols_in[col] SET_TO 0;
    }
  world->cruisers_ass SET_TO 0;
  world->cruisers_left SET_TO 0;
  world->destroyers_ass SET_TO 0;
  world->destroyers_left SET_TO 0;
  world->exes SET_TO 0;
  world->find_all SET_TO 0;
  for (n SET_TO 0; n < MAX_GUESSES; n++)
    {
      world->guesses_tried[n].row SET_TO 0;
      world->guesses_tried[n].col SET_TO 0;
      world->guesses_tried[n].size SET_TO 0;
      world->guesses_tried[n].in_row SET_TO 0;
    }
  for (n SET_TO 0; n < TEXT_SIZE; n++)
    {
      for (m SET_TO 0; m < TEXT_SIZE; m++)
	{
	  world->logic[n][m] SET_TO 0;
	}
    }
  world->logic_line SET_TO 0;
  world->number_cols SET_TO 0;
  world->number_in_cols SET_TO 0;
  world->number_guesses_tried SET_TO 0;
  world->number_rows SET_TO 0;
  world->number_in_rows SET_TO 0;
  world->print_all SET_TO 0;
  world->progress SET_TO 0;
  for (row SET_TO 0; row < MAX_NUMBER_ROWS; row++)
    {
      world->rows[row].aim SET_TO 0;
      world->rows[row].battleships SET_TO 0;
      world->rows[row].cruisers SET_TO 0;
      world->rows[row].destroyers SET_TO 0;
      world->rows[row].done SET_TO 0;
      world->rows[row].dry SET_TO 0;
      world->rows[row].exes SET_TO 0;
      world->rows[row].got SET_TO 0;
      world->rows[row].submarines SET_TO 0;
      for (col SET_TO 0; col < MAX_NUMBER_COLS; col++)
	{
	  world->locations[row][col] SET_TO 0;
	  world->squares[row][col] SET_TO ' ';
	}
      world->rows_in[row] SET_TO 0;
    }
  world->submarines_ass_col SET_TO 0;
  world->submarines_ass_row SET_TO 0;
  world->submarines_left_col SET_TO 0;
  world->submarines_left_row SET_TO 0;
  world->undone SET_TO 0;
  world->verbose SET_TO 0;
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
  const char * name SET_TO "insert_guess_col";
  int row;
  int col;
  char buffer[TEXT_SIZE];

  row SET_TO the_guess->row;
  col SET_TO the_guess->col;
  CHB(((row < 0) OR
       (row >= world->number_rows) OR
       (col < 0) OR
       (col >= world->number_cols)), "Bug bad guess location");
  if (the_guess->size IS 1)
    {
      strcpy(buffer, "guess submarine is at ");
      IFF(insert_guess_square(row, col, 'O', world));
    }
  else if (the_guess->size IS 2)
    {
      strcpy(buffer, "guess destroyer is in column at ");
      IFF(insert_guess_square(row, col, '^', world));
      IFF(insert_guess_square((row + 1), col, 'v', world));
    }
  else if (the_guess->size IS 3)
    {
      strcpy(buffer, "guess cruiser is in column at ");
      IFF(insert_guess_square(row, col, '^', world));
      IFF(insert_guess_square((row + 1), col, '+', world));
      IFF(insert_guess_square((row + 2), col, 'v', world));
    }
  else if (the_guess->size IS 4)
    {
      strcpy(buffer, "guess battleship is in column at ");
      IFF(insert_guess_square(row, col, '^', world));
      IFF(insert_guess_square((row + 1), col, '+', world));
      IFF(insert_guess_square((row + 2), col, '+', world));
      IFF(insert_guess_square((row + 3), col, 'v', world));
    }
  else
    CHB(1, "Bug bad guess size");
  CHB((world->progress IS 0), "Bug guess changed nothing");
  world->cols_in[0] SET_TO col;
  world->number_in_cols SET_TO 1;
  world->rows_in[0] SET_TO row;
  world->number_in_rows SET_TO 1;
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
  const char * name SET_TO "insert_guess_row";
  int row;
  int col;
  char buffer[TEXT_SIZE];

  row SET_TO the_guess->row;
  col SET_TO the_guess->col;
  CHB(((row < 0) OR
       (row >= world->number_rows) OR
       (col < 0) OR
       (col >= world->number_cols)), "Bug bad guess location");
  if (the_guess->size IS 1)
    {
      strcpy(buffer, "guess submarine is at ");
      IFF(insert_guess_square(row, col, 'O', world));
    }
  else if (the_guess->size IS 2)
    {
      strcpy(buffer, "guess destroyer is in row at ");
      IFF(insert_guess_square(row, col, '<', world));
      IFF(insert_guess_square(row, (col + 1), '>', world));
    }
  else if (the_guess->size IS 3)
    {
      strcpy(buffer, "guess cruiser is in row at ");
      IFF(insert_guess_square(row, col, '<', world));
      IFF(insert_guess_square(row, (col + 1), '+', world));
      IFF(insert_guess_square(row, (col + 2), '>', world));
    }
  else if (the_guess->size IS 4)
    {
      strcpy(buffer, "guess battleship is in row at ");
      IFF(insert_guess_square(row, col, '<', world));
      IFF(insert_guess_square(row, (col + 1), '+', world));
      IFF(insert_guess_square(row, (col + 2), '+', world));
      IFF(insert_guess_square(row, (col + 3), '>', world));
    }
  else
    CHB(1, "Bug bad guess size");
  CHB((world->progress IS 0), "Bug guess changed nothing");
  world->cols_in[0] SET_TO col;
  world->number_in_cols SET_TO 1;
  world->rows_in[0] SET_TO row;
  world->number_in_rows SET_TO 1;
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

Side Effects: 

*/

int insert_guess_square(   /* ARGUMENTS                     */
 int row,                  /* row index of square           */
 int col,                  /* column index of square        */
 char item1,               /* character to insert in square */
 struct bap_world * world) /* puzzle model                  */
{
  const char * name SET_TO "insert_guess_square";
  char item2;

  item2 SET_TO world->squares[row][col];
  if (item2 IS item1);
  else if ((item2 IS ' ') OR (item2 IS 'X'))
    {
      world->squares[row][col] SET_TO item1;
      world->progress SET_TO 1;
      if (item2 IS 'X')
	{
	  world->rows[row].exes--;
	  world->cols[col].exes--;
	  world->exes--;
	}
      else
	{
	  world->rows[row].got++;
	  world->cols[col].got++;
	}
      IFF(make_water(row - 1, col - 1, world));
      IFF(make_water(row - 1, col + 1, world));
      IFF(make_water(row + 1, col - 1, world));
      IFF(make_water(row + 1, col + 1, world));
      if (item1 IS 'O')
	{
	  IFF(make_water(row + 1, col, world));
	  IFF(make_water(row - 1, col, world));
	  IFF(make_water(row, col + 1, world));
	  IFF(make_water(row, col - 1, world));
	}
      else if (item1 IS '<')
	IFF(make_water(row, col - 1, world));
      else if (item1 IS '^')
	IFF(make_water(row - 1, col, world));
      else if (item1 IS '>')
	IFF(make_water(row, col + 1, world));
      else if (item1 IS 'v')
	IFF(make_water(row + 1, col, world));
      else
	CHB((item1 ISNT '+'), "Bug bad character to insert");
    }
  else
    CHB(1, "Bug bad character at guess location");
  return OK;
}

/*************************************************************************/

/* legal_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. legal_col_ships returns ERROR.
  2. legal_col_across returns ERROR.
  3. legal_col_beside returns ERROR.

Called By:  try_full_but_one_col

Side Effects: *is_legal is set to 1 if the column is legal and to 0 if not.

Notes:

This checks whether the test column is legal and, if so, whether the
test column can be inserted into the puzzle without creating an
illegal condition in either the two columns on either side or any
crossing row.

A column is illegal if any of the following occurs:

1. A ship is found that is longer than 4.

2. More ships of a given size are needed than are available.

3. The column on the right or left is left with fewer dry than its aim
so that it cannot be filled.

4. Any crossing row is left with fewer dry than its aim so that
it cannot be filled.

5. A crossing row is overfilled. This could happen if try_full_but_one
previously filled another column of the row.

6. A ship middle occurs with water on one side and ship part on the other.

The column being tested has been written in the test array, so that
indexing is easy. It helps to imagine that the test array is written
vertically, top down.

Y is used to fill test columns so that the squares to be tested are not
confused with squares previously set to X.

This assumes that all ship parts other than X's and Y's have been
padded with water, so O^<> should not be encountered while
reading_ship. The column should have no blanks in it, so a blank should
never be encountered.

*/

int legal_col(             /* ARGUMENTS                        */
 char * test,              /* column being tested              */
 int col,                  /* index of the column being tested */
 int * is_legal,           /* set here, 1=line legal, 0=not    */
 struct bap_world * world) /* puzzle model                     */
{
  const char * name SET_TO "legal_col";

  *is_legal SET_TO 1;
  IFF(legal_col_ships(test, col, is_legal, world));
  if (*is_legal)
    IFF(legal_col_beside(test, col, is_legal, world));
  if (*is_legal)
    IFF(legal_col_across(test, col, is_legal, world));
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
 int col,                  /* index of the column being tested  */
 int * is_legal,           /* set here to 0 if column not legal */
 struct bap_world * world) /* puzzle model                      */
{
  const char * name SET_TO "legal_col_across";
  int row;
  int water;

  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if ((test[row] IS 'Y') AND
	  (world->rows[row].aim < (world->rows[row].got + 1)))
	{
	  *is_legal SET_TO 0;
	  break;
	}
      water SET_TO 0;
      if ((square(row, (col - 1), world) IS ' ') AND
	  ((test[row - 1] ISNT '.') OR (test[row + 1] ISNT '.')))
	water++;
      if ((square(row, (col + 1), world) IS ' ') AND
	  ((test[row - 1] ISNT '.') OR (test[row + 1] ISNT '.')))
	water++;
      if ((world->rows[row].dry - water) < world->rows[row].aim)
	{
	  *is_legal SET_TO 0;
	  break;
	}
    }
  return OK;
}

/*************************************************************************/

/* legal_col_beside

Returned Value: int (OK)

Called By:
  legal_col
  ship_fits_col_row

Side Effects: See legal_row_beside. This is analogous.

Notes:

*is_legal should be set to 1 when this is called.

*/

int legal_col_beside(      /* ARGUMENTS                         */
 char * test,              /* column being tested               */
 int col,                  /* index of the column being tested  */
 int * is_legal,           /* set here to 0 if column not legal */
 struct bap_world * world) /* puzzle model                      */
{
  const char * name SET_TO "legal_col_beside";
  int row;
  int water1;
  int water2;
  int no_ships;

  no_ships SET_TO
    ((world->battleships_left IS 0) AND
     (world->cruisers_left IS 0) AND
     (world->destroyers_left IS 0));
  water1 SET_TO 0;
  water2 SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if ((square(row, (col - 1), world) IS ' ') AND
	  ((test[row - 1] ISNT '.') OR
	   (test[row + 1] ISNT '.') OR
	   (no_ships AND
	    (test[row] ISNT '.') AND
	    (world->rows[row].battleships IS 0) AND
	    (world->rows[row].cruisers IS 0) AND
	    (world->rows[row].destroyers IS 0)) OR
	   ((test[row] IS 'Y') AND
	    (world->rows[row].aim IS (world->rows[row].got + 1)))))
	water1++;
      if ((square(row, (col + 1), world) IS ' ') AND
	  ((test[row - 1] ISNT '.') OR
	   (test[row + 1] ISNT '.') OR
	   (no_ships AND
	    (test[row] ISNT '.') AND
	    (world->rows[row].battleships IS 0) AND
	    (world->rows[row].cruisers IS 0) AND
	    (world->rows[row].destroyers IS 0)) OR
	   ((test[row] IS 'Y') AND
	    (world->rows[row].aim IS (world->rows[row].got + 1)))))
	water2++;
    }
  if ((col ISNT 0) AND
      ((world->cols[col - 1].dry - water1) < world->cols[col - 1].aim))
    *is_legal SET_TO 0;
  if ((col ISNT (world->number_cols - 1)) AND
      ((world->cols[col + 1].dry - water2) < world->cols[col + 1].aim))
    *is_legal SET_TO 0;
  return OK;
}

/*************************************************************************/

/* legal_col_ships

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. an illegal character is found while reading a ship:
     "Puzzle has no answer bad ship".
  2. an illegal character is found while not reading a ship:
     "Bug_in_solving".

Called By:  legal_col

Side Effects: This sets is_legal to 0 if any of the following occurs:
1. A ship is found that is longer than 4.
2. More ships of a given size are needed than are available.
3. A ship middle occurs with water on one side and ship part on the other.

Notes:

The column is known to be completely filled, so no spaces should occur.

Ship middle cannot occur on the top or bottom of the puzzle with a
ship part in the same column of the row below or above it.

is_legal should be set to 1 before this starts, since it only sets
is_legal to zero for illegal configurations.

At the last "else" in this function (test[row] IS 'v') OR (test[row] IS ' ')

This is currently not testing submarines.

*/

int legal_col_ships(       /* ARGUMENTS                         */
 char * test,              /* column being tested               */
 int col,                  /* index of the column being tested  */
 int * is_legal,           /* set here to 0 if not legal        */
 struct bap_world * world) /* puzzle model                      */
{
  const char * name SET_TO "legal_col_ships";
  int reading_ship; /* 1=have started reading a ship, 0=not */
  int length;       /* length of ship being read            */
  int row;
  int battleships;
  int cruisers;
  int destroyers;
  int submarines;

  battleships SET_TO 0;
  cruisers SET_TO 0;
  destroyers SET_TO 0;
  submarines SET_TO 0;
  length SET_TO 0;
  reading_ship SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if (reading_ship)
	{
	  if ((test[row] IS 'v') OR
	      (test[row] IS '+') OR
	      (test[row] IS 'X') OR
	      (test[row] IS 'Y'))
	    length++;
	  else
	    CHK(1, "Puzzle has no answer bad ship");
	  if ((row IS (world->number_rows - 1)) OR (test[row + 1] IS '.'))
	    {
	      if ((length > 4) OR (test[row] IS '+'))
		{
		  *is_legal SET_TO 0;
		  break;
		}
	      else if (length IS 4)
		{
		  if (world->locations[row - 3][col]);
		  else
		    battleships++;
		}
	      else if (length IS 3)
		{
		  if (world->locations[row - 2][col]);
		  else
		    cruisers++;
		}
	      else if (length IS 2)
		{
		  if (world->locations[row - 1][col]);
		  else
		    destroyers++;
		}
	      reading_ship SET_TO 0;
	      length SET_TO 0;
	      row++;
	    }
	}
      else
	{
	  if ((test[row] IS 'X') OR (test[row] IS 'Y'))
	    {
	      if ((row IS (world->number_rows - 1)) OR (test[row + 1] IS '.'));
	      else
		{
		  reading_ship SET_TO 1;
		  length SET_TO 1;
		}
	    }
	  else if (test[row] IS '^')
	    {
	      reading_ship SET_TO 1;
	      length SET_TO 1;
	    }
	  else if (test[row] IS '+')
	    {
	      if ((row IS (world->number_rows - 1)) OR (test[row + 1] IS '.'));
	      else
		{
		  *is_legal SET_TO 0;
		  break;
		}
	    }
	  else if ((test[row] IS '<') OR
		   (test[row] IS '>') OR
		   (test[row] IS '.') OR
		   (test[row] IS 'O'));
	  else
	    CHK(1, "Bug in solving");
	}
    }
  if (battleships > (world->battleships_left + world->cols[col].battleships))
    *is_legal SET_TO 0;
  if (cruisers > (world->cruisers_left + world->cols[col].cruisers))
    *is_legal SET_TO 0;
  if (destroyers > (world->destroyers_left + world->cols[col].destroyers))
    *is_legal SET_TO 0;
  if (submarines > (world->submarines_left_col + world->cols[col].submarines))
    *is_legal SET_TO 0;
  return OK;
}

/*************************************************************************/

/* legal_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. legal_row_ships returns ERROR.
  2. legal_row_across returns ERROR.
  3. legal_row_beside returns ERROR.

Called By:  try_full_but_one_row

Side Effects: *is_legal is set to 1 if the row is legal and to 0 if not.

This checks whether the test row is legal and, if so, whether it can
be inserted into the puzzle without creating an illegal condition in
either the two rows above and below or any crossing column.

A row is illegal if any of the following occurs:

1. A ship is found that is longer than 4.

2. More ships of a given size are needed than are available.

3. The row above or below is left with fewer dry than its aim so that
it cannot be filled.

4. Any crossing column is left with fewer dry than its aim so that
it cannot be filled.

5. A crossing column is overfilled. This could happen if try_full_but_one
previously filled another row of the column.

6. A ship middle occurs with water on one side and ship part on the other.

Notes:

Y is used to fill test rows so that the squares to be tested are not
confused with squares previously set to X.

This assumes that all ship parts other than X's and Y's have been
padded with water, so Ov^< should not be encountered while
reading_ship. The row should have no blanks in it, so a blank should
never be encountered.

*/

int legal_row(             /* ARGUMENTS                     */
 char * test,              /* the row being tested          */
 int row,                  /* index of the row being tested */
 int * is_legal,           /* set here, 1=line legal, 0=not */
 struct bap_world * world) /* puzzle model                  */
{
  const char * name SET_TO "legal_row";

  *is_legal SET_TO 1;
  IFF(legal_row_ships(test, row, is_legal, world));
  if (*is_legal)
    IFF(legal_row_beside(test, row, is_legal, world));
  if (*is_legal)
    IFF(legal_row_across(test, row, is_legal, world));
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
 int row,                  /* index of the row being tested */
 int * is_legal,           /* set here to 0 if not legal    */
 struct bap_world * world) /* puzzle model                  */
{
  const char * name SET_TO "legal_row_across";
  int col;
  int water;

  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if ((test[col] IS 'Y') AND
	  (world->cols[col].aim < (world->cols[col].got + 1)))
	{
	  *is_legal SET_TO 0;
	  break;
	}
      water SET_TO 0;
      if ((square((row - 1), col, world) IS ' ') AND
	  ((test[col - 1] ISNT '.') OR (test[col + 1] ISNT '.')))
	water++;
      if ((square((row + 1), col, world) IS ' ') AND
	  ((test[col - 1] ISNT '.') OR (test[col + 1] ISNT '.')))
	water++;
      if ((world->cols[col].dry - water) < world->cols[col].aim)
	{
	  *is_legal SET_TO 0;
	  break;
	}
    }
  return OK;
}

/*************************************************************************/

/* legal_row_beside

Returned Value: int (OK)

Called By:
  legal_row
  ship_fits_row_col

Side Effects:

For the row above the given row, this counts the blank squares that would
have water in them if the given row were as given. Then it checks that
the number of now dry squares is less than the aim for the row. If so,
it sets is_legal to 0. Then it does the same thing for the row below
the given row.

To see if a blank square in the row above would be water, this checks
the diagonally adjacent squares in the given row. If either one is
not water, the blank square being checked must be water. Also, if
the only remaining ships are submarines, this looks at the square
directly below the one being checked. If it is not water, the square
being checked must be water.

Notes:

Is_legal should be set to 1 when this is called.

The test string does not have any spaces in it, so it is not necessary
to check for them. The test string has extra water at both ends, so it
is OK to index one space beyond the ends.

This works for the top and bottom rows without any special checking.

*/

int legal_row_beside(      /* ARGUMENTS                     */
 char * test,              /* the row being tested          */
 int row,                  /* index of the row being tested */
 int * is_legal,           /* set here to 0 if not legal    */
 struct bap_world * world) /* puzzle model                  */
{
  const char * name SET_TO "legal_row_beside";
  int col;
  int water1;
  int water2;
  int no_ships;

  no_ships SET_TO
    ((world->battleships_left IS 0) AND
     (world->cruisers_left IS 0) AND
     (world->destroyers_left IS 0));
  water1 SET_TO 0;
  water2 SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if ((square((row - 1), col, world) IS ' ') AND
	  ((test[col - 1] ISNT '.') OR
	   (test[col + 1] ISNT '.') OR
	   (no_ships AND
	    (test[col] ISNT '.') AND
	    (world->cols[col].battleships IS 0) AND
	    (world->cols[col].cruisers IS 0) AND
	    (world->cols[col].destroyers IS 0)) OR
	   ((test[col] IS 'Y') AND
	    (world->cols[col].aim IS (world->cols[col].got + 1)))))
	water1++;
      if ((square((row + 1), col, world) IS ' ') AND
	  ((test[col - 1] ISNT '.') OR
	   (test[col + 1] ISNT '.') OR
	   (no_ships AND
	    (test[col] ISNT '.') AND
	    (world->cols[col].battleships IS 0) AND
	    (world->cols[col].cruisers IS 0) AND
	    (world->cols[col].destroyers IS 0)) OR
	   ((test[col] IS 'Y') AND
	    (world->cols[col].aim IS (world->cols[col].got + 1)))))
	water2++;
    }
  if ((row ISNT 0) AND
      ((world->rows[row - 1].dry - water1) < world->rows[row - 1].aim))
    *is_legal SET_TO 0;
  if ((row ISNT (world->number_rows - 1)) AND
      ((world->rows[row + 1].dry - water2) < world->rows[row + 1].aim))
    *is_legal SET_TO 0;
  return OK;
}

/*************************************************************************/

/* legal_row_ships

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. an illegal character is found while reading a ship:
     "Puzzle has no answer bad ship".
  2. an illegal character is found while not reading a ship:
     "Bug in solving".

Called By:  legal_row

Side Effects: This sets *is_legal to 0 if any of the following occurs:
1. A ship is found that is longer than 4.
2. More ships of a given size are needed than are available.
3. A ship middle occurs with water on one side and ship part on the other.

Notes:

The row is known to be completely filled, so no spaces should occur.

Ship middle cannot occur on the side of the puzzle with a ship part
in the row next to it because, since it is on the side of the
puzzle, the next square in the row will have been marked water
when extend_middle ran.

At the last "else" in this function (test[col] IS '>') OR (test[col] IS ' ')

This is currently not testing submarines.

*/

int legal_row_ships(       /* ARGUMENTS                     */
 char * test,              /* the row being tested          */
 int row,                  /* index of the row being tested */
 int * is_legal,           /* set here to 0 if not legal    */
 struct bap_world * world) /* puzzle model                  */
{
  const char * name SET_TO "legal_row_ships";
  int reading_ship; /* 1=have started reading a ship, 0=not */
  int length;       /* length of ship being read */
  int col;
  int battleships;
  int cruisers;
  int destroyers;
  int submarines;

  battleships SET_TO 0;
  cruisers SET_TO 0;
  destroyers SET_TO 0;
  submarines SET_TO 0;
  length SET_TO 0;
  reading_ship SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if (reading_ship)
	{
	  if ((test[col] IS '>') OR
	      (test[col] IS '+') OR
	      (test[col] IS 'X') OR
	      (test[col] IS 'Y'))
	    length++;
	  else
	    CHK(1, "Puzzle has no answer bad ship");
	  if ((col IS (world->number_cols - 1)) OR (test[col + 1] IS '.'))
	    {
	      if ((length > 4) OR (test[col] IS '+'))
		{
		  *is_legal SET_TO 0;
		  break;
		}
	      else if (length IS 4)
		{
		  if (world->locations[row][col - 3]);
		  else
		    battleships++;
		}
	      else if (length IS 3)
		{
		  if (world->locations[row][col - 2]);
		  else
		    cruisers++;
		}
	      else if (length IS 2)
		{
		  if (world->locations[row][col - 1]);
		  else
		    destroyers++;
		}
	      reading_ship SET_TO 0;
	      length SET_TO 0;
	      col++;
	    }
	}
      else
	{
	  if ((test[col] IS 'X') OR (test[col] IS 'Y'))
	    {
	      if ((col IS (world->number_cols - 1)) OR (test[col + 1] IS '.'));
	      else
		{
		  reading_ship SET_TO 1;
		  length SET_TO 1;
		}
	    }
	  else if (test[col] IS '<')
	    {
	      reading_ship SET_TO 1;
	      length SET_TO 1;
	    }
	  else if (test[col] IS '+')
	    {
	      if ((col IS (world->number_cols - 1)) OR (test[col + 1] IS '.'));
	      else
		{
		  *is_legal SET_TO 0;
		  break;
		}
	    }
	  else if ((test[col] IS 'v') OR
		   (test[col] IS '^') OR
		   (test[col] IS '.') OR
		   (test[col] IS 'O'));
	  else
	    CHK(1, "Bug in solving");
	}
    }
  if (battleships > (world->battleships_left + world->rows[row].battleships))
    *is_legal SET_TO 0;
  if (cruisers > (world->cruisers_left + world->rows[row].cruisers))
    *is_legal SET_TO 0;
  if (destroyers > (world->destroyers_left + world->rows[row].destroyers))
    *is_legal SET_TO 0;
  if (submarines > (world->submarines_left_row + world->rows[row].submarines))
    *is_legal SET_TO 0;
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

Called By:  solve_problem

Side Effects: This reads the rows and columns looking for whole ships
with no X's. Each whole ship is allocated by allocate_ship_row or
allocate_ship_col.

Submarines are allocated from rows in this function but not from
columns, since they should be allocated only once.

*/

int locate_ships(          /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "locate_ships";
  int row;
  int col;
  int reading_ship; /* 1=have started reading a ship, 0=not */
  int length;       /* length of ship being read */
  char switcher;

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
	      switcher SET_TO square(row, col, world);
	      if (switcher IS '>')
		{
		  IFF(allocate_ship_row
		      (row, (col - length), length + 1, world));
		  reading_ship SET_TO 0;
		}
	      else if (switcher IS '+')
		length++;
	      else if ((switcher IS 'X') OR (switcher IS ' '))
		reading_ship SET_TO 0;
	      else
		CHK(1, "Puzzle has no answer bad ship");
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
	      switcher SET_TO square(row, col, world);
	      if (switcher IS 'v')
		{
		  IFF(allocate_ship_col
		      ((row - length), col, length + 1, world));
		  reading_ship SET_TO 0;
		}
	      else if (switcher IS '+')
		length++;
	      else if ((switcher IS 'X') OR (switcher IS ' '))
		reading_ship SET_TO 0;
	      else
		CHK(1, "Puzzle has no answer bad ship");
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

Side Effects: This:
1. calls read_arguments to read the arguments.
2. calls init_world to initialize the world puzzle model.
3. if needed, calls read_problem to read the problem.
4. calls init_solution to initialize the solving process.
5. calls solve_problem to solve the problem.

Notes:

This main follows the model for all mains for solving squares problems.
The model always has the five steps listed immediately above.

It is expected that solve_XXX may be called recursively for search,
but that some initial steps in the solution will need to be done only
once. The init_solution function takes these steps.

The IFFs (which contain "return" statements) stop execution in case an
error occurs (as intended) because in a "main" function, "return"
stops execution.

*/

int main(      /* ARGUMENTS                             */
 int argc,     /* one more than the number of arguments */
 char ** argv) /* function name and arguments           */
{
  const char * name SET_TO "main";
  struct bap_world world1;
  struct bap_world * world;

  world SET_TO &world1;
  IFF(init_world(world));
  IFF(read_arguments(argc, argv, world));
  IFF(read_problem(argv[1], world));
  IFF(init_solution(world));
  CHB((solve_problem(world) ISNT OK), "Puzzle has no answer");
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
  make_guesses_battleships
  make_guesses_cruisers
  make_guesses_destroyers
  make_guesses_submarines

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
  const char * name SET_TO "make_guess_ship_col";
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
  make_guesses_battleships
  make_guesses_cruisers
  make_guesses_destroyers
  make_guesses_submarines

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
  const char * name SET_TO "make_guess_ship_row";
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

/* make_guesses_battleships

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. A battleship is assigned to a strip but no battleship fits in the
     strip: "Cannot solve puzzle"
  2. At least one battleship remains to be located but it is not possible
     to put it anywhere in the puzzle: "Cannot solve puzzle"
  3. make_guess_ship_row returns ERROR.
  4. make_guess_ship_col returns ERROR.
  5. copy_guesses returns ERROR.

Called By:  try_guessing

Side Effects: If any battleship is assigned, this makes a set of
guesses for each strip to which a battleship is assigned and saves the
smallest set.  In the case of equally small sets, the first one found
is saved.  If no battleship is assigned, this makes one set of guesses
for the whole puzzle.

Each set of guesses is exhaustive (at least one must be correct), but
the guesses are not mutually exclusive (which would be nice) unless
only one battleship has not yet been located.

Notes:

This assumes there may be more than one battleship.

*/

int make_guesses_battleships( /* ARGUMENTS                    */
 int * number_guesses,        /* number of guesses, set here  */
 struct guess * guesses,      /* a set of guesses, set here   */
 struct bap_world * world)    /* puzzle model                 */
{
  const char * name SET_TO "make_guesses_battleships";
  int row;
  int col;
  int n;
  struct guess guesses2[MAX_GUESSES]; /* new set of guesses */

  if (world->battleships_ass)
    {
      *number_guesses SET_TO (MAX_GUESSES + 1);
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  if (world->rows[row].battleships)
	    {
	      n SET_TO 0;
	      IFF(make_guess_ship_row
		  (4, row, &n, *number_guesses, guesses2, world));
	      CHK((n IS 0), "Cannot solve puzzle");
	      if (n < *number_guesses)
		{
		  IFF(copy_guesses(guesses, guesses2, n));
		  *number_guesses SET_TO n;
		}
	    }
	}
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  if (world->cols[col].battleships)
	    {
	      n SET_TO 0;
	      IFF(make_guess_ship_col
		  (4, col, &n, *number_guesses, guesses2, world));
	      CHK((n IS 0), "Cannot solve puzzle");
	      if (n < *number_guesses)
		{
		  IFF(copy_guesses(guesses, guesses2, n));
		  *number_guesses SET_TO n;
		}
	    }
	}
    }
  else if (world->battleships_left)
    {
      *number_guesses SET_TO 0;
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  IFF(make_guess_ship_row
	      (4, row, number_guesses, (MAX_GUESSES + 1), guesses, world));
	}
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  IFF(make_guess_ship_col
	      (4, col, number_guesses, (MAX_GUESSES + 1), guesses, world));
	}
      CHK((*number_guesses IS 0), "Cannot solve puzzle");
    }
  else
    CHB(1, "Bug make_guesses_battleships called when no battleships left");
  return OK;
}

/*************************************************************************/

/* make_guesses_cruisers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. A cruiser is assigned to a strip but no cruiser fits in the
     strip: "Cannot solve puzzle"
  2. At least one cruiser remains to be located but it is not possible
     to put it anywhere in the puzzle: "Cannot solve puzzle"
  3. make_guess_ship_row returns ERROR.
  4. make_guess_ship_col returns ERROR.
  5. copy_guesses returns ERROR.

Called By:  try_guessing

Side Effects: See make_guesses_battleships

*/

int make_guesses_cruisers( /* ARGUMENTS                    */
 int * number_guesses,     /* number of guesses, set here  */
 struct guess * guesses,   /* a set of guesses, set here   */
 struct bap_world * world) /* puzzle model                 */
{
  const char * name SET_TO "make_guesses_cruisers";
  int row;
  int col;
  int n;
  struct guess guesses2[MAX_GUESSES]; /* new set of guesses */

  if (world->cruisers_ass)
    {
      *number_guesses SET_TO (MAX_GUESSES + 1);
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  if (world->rows[row].cruisers)
	    {
	      n SET_TO 0;
	      IFF(make_guess_ship_row
		  (3, row, &n, *number_guesses, guesses2, world));
	      CHK((n IS 0), "Cannot solve puzzle");
	      if (n < *number_guesses)
		{
		  IFF(copy_guesses(guesses, guesses2, n));
		  *number_guesses SET_TO n;
		}
	    }
	}
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  if (world->cols[col].cruisers)
	    {
	      n SET_TO 0;
	      IFF(make_guess_ship_col
		  (3, col, &n, *number_guesses, guesses2, world));
	      CHK((n IS 0), "Cannot solve puzzle");
	      if (n < *number_guesses)
		{
		  IFF(copy_guesses(guesses, guesses2, n));
		  *number_guesses SET_TO n;
		}
	    }
	}
    }
  else if (world->cruisers_left)
    {
      *number_guesses SET_TO 0;
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  IFF(make_guess_ship_row
	      (3, row, number_guesses, (MAX_GUESSES + 1), guesses, world));
	}
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  IFF(make_guess_ship_col
	      (3, col, number_guesses, (MAX_GUESSES + 1), guesses, world));
	}
      CHK((*number_guesses IS 0), "Cannot solve puzzle");
    }
  else
    CHB(1, "Bug make_guesses_cruisers called when no cruisers left");
  return OK;
}

/*************************************************************************/

/* make_guesses_destroyers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. A destroyer is assigned to a strip but no destroyer fits in the
     strip: "Cannot solve puzzle".
  2. At least one destroyer remains to be located but it is not possible
     to put it anywhere in the puzzle: "Cannot solve puzzle".
  3. make_guess_ship_row returns ERROR.
  4. make_guess_ship_col returns ERROR.
  5. copy_guesses returns ERROR.

Called By:  try_guessing

Side Effects: See make_guesses_battleships

*/

int make_guesses_destroyers( /* ARGUMENTS                    */
 int * number_guesses,       /* number of guesses, set here  */
 struct guess * guesses,     /* a set of guesses, set here   */
 struct bap_world * world)   /* puzzle model                 */
{
  const char * name SET_TO "make_guesses_destroyers";
  int row;
  int col;
  int n;
  struct guess guesses2[MAX_GUESSES]; /* new set of guesses */

  if (world->destroyers_ass)
    {
      *number_guesses SET_TO (MAX_GUESSES + 1);
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  if (world->rows[row].destroyers)
	    {
	      n SET_TO 0;
	      IFF(make_guess_ship_row
		  (2, row, &n, *number_guesses, guesses2, world));
	      CHK((n IS 0), "Cannot solve puzzle");
	      if (n < *number_guesses)
		{
		  IFF(copy_guesses(guesses, guesses2, n));
		  *number_guesses SET_TO n;
		}
	    }
	}
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  if (world->cols[col].destroyers)
	    {
	      n SET_TO 0;
	      IFF(make_guess_ship_col
		  (2, col, &n, *number_guesses, guesses2, world));
	      CHK((n IS 0), "Cannot solve puzzle");
	      if (n < *number_guesses)
		{
		  IFF(copy_guesses(guesses, guesses2, n));
		  *number_guesses SET_TO n;
		}
	    }
	}
    }
  else if (world->destroyers_left)
    {
      *number_guesses SET_TO 0;
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  IFF(make_guess_ship_row
	      (2, row, number_guesses, (MAX_GUESSES + 1), guesses, world));
	}
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  IFF(make_guess_ship_col
	      (2, col, number_guesses, (MAX_GUESSES + 1), guesses, world));
	}
      CHK((*number_guesses IS 0), "Cannot solve puzzle");
    }
  else
    CHB(1, "Bug make_guesses_destroyers called when no destroyers left");
  return OK;
}

/*************************************************************************/

/* make_guesses_submarines

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. A submarine is assigned to a strip but no submarine fits in the
     strip: "Cannot solve puzzle".
  2. make_guess_ship_row returns ERROR.
  3. make_guess_ship_col returns ERROR.
  4. copy_guesses returns ERROR.

Called By:  try_guessing

Side Effects: See make_guesses_battleships

*/

int make_guesses_submarines( /* ARGUMENTS                    */
 int * number_guesses,       /* number of guesses, set here  */
 struct guess * guesses,     /* a set of guesses, set here   */
 struct bap_world * world)   /* puzzle model                 */
{
  const char * name SET_TO "make_guesses_submarines";
  int row;
  int col;
  int n;
  struct guess guesses2[MAX_GUESSES];

  if (world->submarines_ass_row OR world->submarines_ass_col)
    {
      *number_guesses SET_TO (MAX_GUESSES + 1);
      if (world->submarines_ass_row)
	{
	  for (row SET_TO 0; row < world->number_rows; row++)
	    {
	      if (world->rows[row].submarines)
		{
		  n SET_TO 0;
		  IFF(make_guess_ship_row
		      (1, row, &n, *number_guesses, guesses2, world));
		  CHK((n IS 0), "Cannot solve puzzle");
		  if (n < *number_guesses)
		    {
		      IFF(copy_guesses(guesses, guesses2, n));
		      *number_guesses SET_TO n;
		    }
		}
	    }
	}
      if (world->submarines_ass_col)
	{
	  for (col SET_TO 0; col < world->number_cols; col++)
	    {
	      if (world->cols[col].submarines)
		{
		  n SET_TO 0;
		  IFF(make_guess_ship_col
		      (1, col, &n, *number_guesses, guesses2, world));
		  CHK((n IS 0), "Cannot solve puzzle");
		  if (n < *number_guesses)
		    {
		      IFF(copy_guesses(guesses, guesses2, n));
		      *number_guesses SET_TO n;
		    }
		}
	    }
	}
    }
  else if (world->submarines_left_row)
    {
      *number_guesses SET_TO 0;
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  IFF(make_guess_ship_row
	      (1, row, number_guesses, (MAX_GUESSES + 1), guesses, world));
	}
      CHK((*number_guesses IS 0), "Cannot solve puzzle");
    }
  else if (world->submarines_left_col)
    {
      *number_guesses SET_TO 0;
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  IFF(make_guess_ship_col
	      (1, col, number_guesses, (MAX_GUESSES + 1), guesses, world));
	}
      CHK((*number_guesses IS 0), "Cannot solve puzzle");
    }
  else
    CHB(1, "Bug make_guesses_submarines called when no submarines left");
  return OK;
}

/*************************************************************************/

/* make_water

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The square at the given row and column is neither blank nor
     water: "Puzzle has no answer".

Called By:
  dex_ends_col
  dex_ends_row
  enx_square
  extend_horizontal
  extend_vertical
  fill_bap
  insert_guess_square
  pad_square
  place_ships_col_water
  place_ships_row_water
  try_square_blank

Side Effects: If the square in given row and column is blank, this
sets the square to water, updates dry in the given row and column, and
sets world->progress to 1.

Notes:

This function may be called with row or col out of bounds. In this
case, square(row, col, world) will be '.'. Thus the "if" serves to check
that the square at row,col is both in bounds and blank before it is
set to '.'.

*/

int make_water(            /* ARGUMENTS                                  */
 int row,                  /* index of row of square being made water    */
 int col,                  /* index of column of square being made water */
 struct bap_world * world) /* puzzle model                               */
{
  const char * name SET_TO "make_water";

  if (square(row, col, world) IS ' ')
    {
      world->squares[row][col] SET_TO '.';
      world->cols[col].dry--;
      world->rows[row].dry--;
      world->progress SET_TO 1;
    }
  else
    CHK((square(row, col, world) ISNT '.'), "Puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* mark_dones

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. A strip still has a ship assigned when done: "Bug in assigning ships".

Called By:  solve_problem

Side Effects: For each strip not already marked done in which (1) aim,
got, and dry are all equal and (2) exes is zero, the strip is marked
done and world->undone is decreased by one.

Notes: Locate_ships should normally be run before this so that no
strip which has unlocated ships is marked done.

*/

int mark_dones(            /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "mark_dones";
  int row;
  int col;

  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if (world->rows[row].done);
      else if ((world->rows[row].aim IS world->rows[row].got) AND
	       (world->rows[row].aim IS world->rows[row].dry) AND
	       (world->rows[row].exes IS 0))
	{
	  world->rows[row].done SET_TO 1;
	  world->undone--;
	  CHK(((world->rows[row].battleships) OR
	       (world->rows[row].cruisers) OR
	       (world->rows[row].destroyers) OR
	       (world->rows[row].submarines)), "Bug in assigning ships");
	}
    }
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if (world->cols[col].done);
      else if ((world->cols[col].aim IS world->cols[col].got) AND
	       (world->cols[col].aim IS world->cols[col].dry) AND
	       (world->cols[col].exes IS 0))
	{
	  world->cols[col].done SET_TO 1;
	  world->undone--;
	  CHK(((world->cols[col].battleships) OR
	       (world->cols[col].cruisers) OR
	       (world->cols[col].destroyers) OR
	       (world->cols[col].submarines)), "Bug in assigning ships");
	}
    }
  return OK;
}

/*************************************************************************/

/* pad_bap

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. pad_square returns ERROR.

Called By:  init_solution

Side Effects: This adds any missing water at the adjacent squares sure
to be water of each square of the puzzle that has a ship part in
it. If any missing water is added, that fact is reported.

*/

int pad_bap(               /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "pad_bap";
  int n;
  int k;
  char item;

  for (n SET_TO 0; n < world->number_rows; n++)
    {
      for (k SET_TO 0; k < world->number_cols; k++)
	{
	  item SET_TO world->squares[n][k];
	  if ((item ISNT ' ') AND (item ISNT '.'))
	    IFF(pad_square(item, n, k, world));
	}
    }
  if (world->progress)
    {
      strcpy
	(world->logic[world->logic_line++], "pad given ship parts with water");
      if (world->print_all)
	{
	  print_bap(world);
	  printf("%s\n\n", world->logic[world->logic_line - 1]);
	}
      world->progress SET_TO 0;
    }
  return OK;
}

/*************************************************************************/

/* pad_square

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. make_water returns ERROR.

Called By:  pad_bap

Side Effects:

This adds any missing water at the adjacent squares sure to be water
for one square of the puzzle that has a ship part (item) in it.

*/

int pad_square(            /* ARGUMENTS                 */
 char item,                /* ship part in square       */
 int row,                  /* index of row of square    */
 int col,                  /* index of column of square */
 struct bap_world * world) /* puzzle model              */
{
  const char * name SET_TO "pad_square";

  IFF(make_water(row - 1, col - 1, world));
  IFF(make_water(row - 1, col + 1, world));
  IFF(make_water(row + 1, col - 1, world));
  IFF(make_water(row + 1, col + 1, world));
  if ((item IS '<') OR
      (item IS '>') OR
      (item IS 'v') OR
      (item IS '^') OR
      (item IS 'O'))
    {
      if (item ISNT 'v')
	IFF(make_water(row - 1, col, world));
      if (item ISNT '<')
	IFF(make_water(row, col + 1, world));
      if (item ISNT '^')
	IFF(make_water(row + 1, col, world));
      if (item ISNT '>')
	IFF(make_water(row, col - 1, world));
    }
  return OK;
}

/*************************************************************************/

/* place_battleships

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. place_ships_row returns ERROR.
  2. place_ships_col returns ERROR.
  3. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: Parts of battleships assigned to rows and columns are
placed in the puzzle, to the extent the location can be determined.

*/

int place_battleships(     /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "place_battleships";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if (world->rows[row].battleships)
	IFF(place_ships_row(row, 4, world->rows[row].battleships, world));
    }
  world->number_in_cols SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if (world->cols[col].battleships)
	IFF(place_ships_col(col, 4, world->cols[col].battleships, world));
    }
  IFF(record_progress("place battleship parts and/or water in ", world));
  return OK;
}

/*************************************************************************/

/* place_cruisers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. place_ships_row returns ERROR.
  2. place_ships_col returns ERROR.
  3. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: Parts of cruisers assigned to rows and columns are
placed in the puzzle, to the extent the location can be determined.


*/

int place_cruisers(        /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "place_cruisers";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if (world->rows[row].cruisers)
	IFF(place_ships_row(row, 3, world->rows[row].cruisers, world));
    }
  world->number_in_cols SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if (world->cols[col].cruisers)
	IFF(place_ships_col(col, 3, world->cols[col].cruisers, world));
    }
  IFF(record_progress("place cruiser parts and/or water in ", world));
  return OK;
}

/*************************************************************************/

/* place_destroyers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. place_ships_row returns ERROR.
  2. place_ships_col returns ERROR.
  3. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: Parts of destroyers assigned to rows and columns are
placed in the puzzle, to the extent the location can be determined.

*/

int place_destroyers(      /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "place_destroyers";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if (world->rows[row].destroyers)
	IFF(place_ships_row(row, 2, world->rows[row].destroyers, world));
    }
  world->number_in_cols SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if (world->cols[col].destroyers)
	IFF(place_ships_col(col, 2, world->cols[col].destroyers, world));
    }
  IFF(record_progress("place destroyer parts and/or water in ", world));
  return OK;
}

/*************************************************************************/

/* place_ships_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_min_starts_col returns ERROR.
  2. find_max_starts_col returns ERROR.
  3. enx_square returns ERROR.
  4. place_ships_col_water returns ERROR.

Called By:
  place_battleships
  place_cruisers
  place_destroyers

Side Effects: See discussion of place_ships_row.

*/

int place_ships_col(       /* ARGUMENTS                                     */
 int col,                  /* index of column being processed               */
 int size,                 /* size of ship                                  */
 int assigned,             /* number ships of given size assigned to column */
 struct bap_world * world) /* puzzle model                                  */
{
  const char * name SET_TO "place_ships_col";
  int n;
  int k;
  int max_starts[MAX_NUMBER_ROWS]; /* array with minimum starting positions */
  int min_starts[MAX_NUMBER_ROWS]; /* array with maximum starting positions */
  int placed;

  IFF(find_min_starts_col(col, size, assigned, min_starts, world));
  IFF(find_max_starts_col(col, size, assigned, max_starts, world));
  placed SET_TO 0;
  for (n SET_TO 0; n < assigned; n++)
    {
      for (k SET_TO min_starts[n]; k < (min_starts[n] + size); k++)
	{
	  if ((k >= max_starts[n]) AND (square(k, col, world) IS ' '))
	    {
	      IFF(enx_square(k, col, world));
	      placed SET_TO 1;
	    }
	}
    }
  IFF(place_ships_col_water
      (col, size, assigned, min_starts, max_starts, &placed, world));
  if (placed)
    world->cols_in[world->number_in_cols++] SET_TO col;
  return OK;
}

/*************************************************************************/

/* place_ships_col_water

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The assigned ships of the given length (along with other ship
     parts in the row) do not fit in the column: "Puzzle has no answer".
  2. make_water returns ERROR.

Called By:  place_ships_col

Side Effects: This counts the number of squares filled with ship parts
that cannot be covered by the assigned ships of the given size in the
given column and adds to it the number of squares that will be used by
the assigned ships. If the total equals the aim number for the column,
then the empty squares in the column that cannot be covered by the
assigned ships must be water, so they are made water. Also, if min_start[n]
is the same as max_start[n], then there is only one place that ship can
go, so water is placed at both ends of the ship. If any square is
made water, *placed is set to 1.

Notes:

placed may already be 1 before this is called. That is OK.

*/

int place_ships_col_water( /* ARGUMENTS                                     */
 int col,                  /* index of column being processed               */
 int size,                 /* size of ship                                  */
 int assigned,             /* number ships of given size assigned to column */
 int * min_starts,         /* array of minimum starting positions           */
 int * max_starts,         /* array of maximum starting positions           */
 int * placed,             /* set to one if any ship parts placed           */
 struct bap_world * world) /* puzzle model                                  */
{
  const char * name SET_TO "place_ships_col_water";
  int row;
  int n;     /* counter for assigned ships */
  int total;

  n SET_TO 0;
  total SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; )
    {
      if ((row < min_starts[n]) OR (row > (max_starts[n] + size - 1)))
	{
	  if ((square(row, col, world) ISNT ' ') AND
	      (square(row, col, world) ISNT '.'))
	    total++;
	  row++;
	}
      else
	{
	  row SET_TO (max_starts[n] + size);
	  if (n < (assigned - 1))
	    n++;
	}
    }
  total SET_TO (total + (assigned * size));
  CHK((total > world->cols[col].aim), "Puzzle has no answer");
  n SET_TO 0;
  if (total IS world->cols[col].aim)
    {
      for (row SET_TO 0; row < world->number_rows; )
	{
	  if ((row < min_starts[n]) OR (row > (max_starts[n] + size - 1)))
	    {
	      if (square(row, col, world) IS ' ')
		{
		  IFF(make_water(row, col, world));
		  *placed SET_TO 1;
		}
	      row++;
	    }
	  else
	    {
	      row SET_TO (max_starts[n] + size);
	      if (n < (assigned - 1))
		n++;
	    }
	}
    }
  for (n SET_TO 0; n < assigned; n++)
    {
      if (min_starts[n] IS max_starts[n])
	{
	  if (square((min_starts[n] - 1), col, world) ISNT '.')
	    {
	      IFF(make_water((min_starts[n] - 1), col, world));
	      *placed SET_TO 1;
	    }
	  if (square((min_starts[n] + size), col, world) ISNT '.')
	    {
	      IFF(make_water((min_starts[n] + size), col, world));
	      *placed SET_TO 1;
	    }
	}
    }
  return OK;
}

/*************************************************************************/

/* place_ships_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_min_starts_row returns ERROR.
  2. find_max_starts_row returns ERROR.
  3. enx_square returns ERROR.
  4. place_ships_row_water returns ERROR.

Called By:
  place_battleships
  place_cruisers
  place_destroyers

Side Effects: This makes two placements of the ships of the given size
assigned to a row: as far left as possible, and as far right as
possible. Blank squares that are covered by the same ship in both
cases are marked with an X.  Both placements must be legal.

Notes:

min_starts is an array of the starting positions of assigned (hence
not located) ships pushed left. max_starts is an array of the starting
positions of assigned (hence not located) ships pushed right. The
length of both arrays equals the "assigned" argument.

max_starts[n] and min_starts[n] refer to the same ship.

To be complete, this would construct all possible fillings of the row
that are legal and include all assigned ships. Then it would
mark with an X all blank squares that are full in every case.

*/

int place_ships_row(       /* ARGUMENTS                                     */
 int row,                  /* index of row being processed                  */
 int size,                 /* size of ship                                  */
 int assigned,             /* number of ships of given size assigned to row */
 struct bap_world * world) /* puzzle model                                  */
{
  const char * name SET_TO "place_ships_row";
  int n;
  int k;
  int max_starts[MAX_NUMBER_COLS]; /* array with minimum starting positions */
  int min_starts[MAX_NUMBER_COLS]; /* array with maximum starting positions */
  int placed;

  IFF(find_min_starts_row(row, size, assigned, min_starts, world));
  IFF(find_max_starts_row(row, size, assigned, max_starts, world));
  placed SET_TO 0;
  for (n SET_TO 0; n < assigned; n++)
    {
      for (k SET_TO min_starts[n]; k < (min_starts[n] + size); k++)
	{
	  if ((k >= max_starts[n]) AND (square(row, k, world) IS ' '))
	    {
	      IFF(enx_square(row, k, world));
	      placed SET_TO 1;
	    }
	}
    }
  IFF(place_ships_row_water
      (row, size, assigned, min_starts, max_starts, &placed, world));
  if (placed)
    world->rows_in[world->number_in_rows++] SET_TO row;
  return OK;
}

/*************************************************************************/

/* place_ships_row_water

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The assigned ships of the given length (along with other ship
     parts in the row) do not fit in the row: "Puzzle has no answer".
  2. make_water returns ERROR.

Called By:  place_ships_row

Side Effects: This counts the number of squares filled with ship parts
that cannot be covered by the assigned ships of the given size in the
given row and adds to it the number of squares that will be used by
the assigned ships. If the total equals the aim number for the row,
then the empty squares in the row that cannot be covered by the
assigned ships must be water, so they are made water. If any square is
made water, *placed is set to 1.

Notes:

placed may already be 1 before this is called. That is OK.

*/

int place_ships_row_water( /* ARGUMENTS                                     */
 int row,                  /* index of row being processed                  */
 int size,                 /* size of ship                                  */
 int assigned,             /* number of ships of given size assigned to row */
 int * min_starts,         /* array of minimum starting positions           */
 int * max_starts,         /* array of maximum starting positions           */
 int * placed,             /* set to one if any ship parts placed           */
 struct bap_world * world) /* puzzle model                                  */
{
  const char * name SET_TO "place_ships_row_water";
  int col;
  int n;     /* counter for assigned ships */
  int total;

  n SET_TO 0;
  total SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; )
    {
      if ((col < min_starts[n]) OR (col > (max_starts[n] + size - 1)))
	{
	  if ((square(row, col, world) ISNT ' ') AND
	      (square(row, col, world) ISNT '.'))
	    total++;
	  col++;
	}
      else
	{
	  col SET_TO (max_starts[n] + size);
	  if (n < (assigned - 1))
	    n++;
	}
    }
  total SET_TO (total + (assigned * size));
  CHK((total > world->rows[row].aim), "Puzzle has no answer");
  n SET_TO 0;
  if (total IS world->rows[row].aim)
    {
      for (col SET_TO 0; col < world->number_cols; )
	{
	  if ((col < min_starts[n]) OR (col > (max_starts[n] + size - 1)))
	    {
	      if (square(row, col, world) IS ' ')
		{
		  IFF(make_water(row, col, world));
		  *placed SET_TO 1;
		}
	      col++;
	    }
	  else
	    {
	      col SET_TO (max_starts[n] + size);
	      if (n < (assigned - 1))
		n++;
	    }
	}
    }
  for (n SET_TO 0; n < assigned; n++)
    {
      if (min_starts[n] IS max_starts[n])
	{
	  if (square(row, (min_starts[n] - 1), world) ISNT '.')
	    {
	      IFF(make_water(row, (min_starts[n] - 1), world));
	      *placed SET_TO 1;
	    }
	  if (square(row, (min_starts[n] + size), world) ISNT '.')
	    {
	      IFF(make_water(row, (min_starts[n] + size), world));
	      *placed SET_TO 1;
	    }
	}
    }
  return OK;
}

/*************************************************************************/

/* print_bap

Returned Value: int (OK)

Called By:
  dex_squares
  extend_ends
  pad_bap
  read_problem
  record_progress
  solve_problem

Side Effects: This prints the puzzle from the bap_world. The puzzle
is printed in the same format as required for reading in, except that
the list of ships and end marker are omitted.

*/

int print_bap(             /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "print_bap";
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
  for (col SET_TO 0; col < world->number_cols; col++)
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
  1. argc is not 4 or 5.
  2. argv[2] is not "all" or "one.
  3. argv[3] is not "yes" or "no".
  4. argc is 5 and argv[4] is not "v".

Called By:  main

Side Effects: The values of world->find_all, world->print_all,
and world->verbose are set.

*/

int read_arguments(        /* ARGUMENTS                             */
 int argc,                 /* one more than the number of arguments */
 char ** argv,             /* function name and arguments           */
 struct bap_world * world) /* puzzle model                          */
{
  const char * name SET_TO "read_arguments";

  if (((argc IS 4) OR ((argc IS 5) AND (strcmp(argv[4], "v") IS 0))) AND
      ((strcmp(argv[2], "all") IS 0) OR (strcmp(argv[2], "one") IS 0)) AND
      ((strcmp(argv[3], "no") IS 0) OR (strcmp(argv[3], "yes") IS 0)))
    {
      if (strcmp(argv[2], "all") IS 0)
	world->find_all SET_TO 1;
      else
	world->find_all SET_TO 0;
      if (strcmp(argv[3], "yes") IS 0)
	world->print_all SET_TO 1;
      else
	world->print_all SET_TO 0;
      if (argc IS 5)
	world->verbose SET_TO 1;
      else
	world->verbose SET_TO 0;
    }
  else
    {
      printf("Usage: %s <file name> <all|one> <yes|no> [v]\n", argv[0]);
      printf("all = find all solutions\n");
      printf("one = find one solution\n");
      printf("yes = print puzzle when done and when a rule makes progress\n");
      printf("no = print puzzle only when done\n");
      printf("v = verbose (prints more messages)\n");
      CHB(1, " ");
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

Called By:  read_problem

Side Effects: This reads the row of column numbers and records them in
world->col_counts.  It also reads the blank line following that row. The
column numbers line follows the bottom line of dashes.

*/

int read_column_numbers(   /* ARGUMENTS                 */
 FILE * in_port,           /* port to file to read from */
 char * buffer,            /* buffer to read into       */
 struct bap_world * world) /* puzzle model              */
{
  const char * name SET_TO "read_column_numbers";
  int col;
  int n;

  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
      "File ends before line of column numbers");
  CHB(((buffer[0] ISNT ' ') OR (buffer[1] ISNT ' ')),
      "Bad line of column numbers");
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      n SET_TO
	sscanf((buffer + (2 * col) + 2), "%2d", &(world->cols[col].aim));
      CHB((n ISNT 1), "Bad line of column numbers");
      CHB((world->cols[col].aim > world->number_rows), "Column count too big");
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
  5. There are more than MAX_NUMBER_COLS columns: "Too many columns".

Called By:  read_problem

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
  const char * name SET_TO "read_dashes";
  int n;

  CHB((fgets(dashes, TEXT_SIZE, in_port) IS NULL),
      "File ends before first line of dashes");
  CHB((dashes[0] ISNT ' '), "Bad first line of dashes");
  for (n SET_TO 1; ; n SET_TO (n + 2))
    {
      CHB((n >= (TEXT_SIZE - 1)), "Line of dashes too long");
      if (dashes[n] IS '\n')
	break;
      CHB(((dashes[n] ISNT ' ') OR (dashes[n + 1] ISNT '-')),
	  "Bad first line of dashes");
    }
  *row_length SET_TO (n + 2);
  world->number_cols SET_TO ((n - 1) / 2);
  CHB((world->number_cols < 1), "Problem does not have at least one column");
  CHB((world->number_cols > MAX_NUMBER_COLS), "Too many columns");
  if (world->print_all)
    printf("%d columns\n", world->number_cols);
  return OK;
}

/*************************************************************************/

/* read_problem

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

int read_problem(          /* ARGUMENTS                    */
 char * file_name,         /* name of file to read         */
 struct bap_world * world) /* puzzle model, filled in here */
{
  const char * name SET_TO "read_problem";
  FILE * in_port;
  char dashes[TEXT_SIZE];
  char buffer[TEXT_SIZE];
  int row_length;

  in_port SET_TO fopen(file_name, "r");
  CHB((in_port IS NULL), "Could not open file");
  for (; ; )
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "File ends before first blank line");
      if (buffer[0] IS '\n')
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

/* read_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The row does not start with " |": "Bad problem row".
  2. An illegal character is included in the row: "Bad character".
  3. The number at the end of the row cannot be read: "Bad problem row".
  4. The number at the end of the row is larger than the number of
     places in the row: "Row count too big".
  5. An X is used: "Cannot handle x yet".
  6. The number of rows found is greater than MAX_NUMBER_ROWS:
     "Too many rows".

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
  const char * name SET_TO "read_row";
  int square_count;
  int col;
  int n;
  char c;

  CHB((world->number_rows IS MAX_NUMBER_ROWS), "Too many rows");
  world->rows[world->number_rows].dry SET_TO world->number_cols;
  CHB(((buffer[0] ISNT ' ') OR (buffer[1] ISNT '|')), "Bad problem row");
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      c SET_TO buffer[2 + (2 * col)];
      CHB(((c ISNT '+') AND
	   (c ISNT '<') AND
	   (c ISNT 'v') AND
	   (c ISNT 'O') AND
	   (c ISNT '>') AND
	   (c ISNT '^') AND
	   (c ISNT '.') AND
	   (c ISNT ' ') AND
	   (c ISNT 'X')), "Bad character");
      CHB((c IS 'X'), "Cannot handle x yet");
      if (col < (world->number_cols - 1))
	CHB((buffer[3 + (2 * col)] ISNT ' '), "Bad character");
      else
	CHB((buffer[3 + (2 * col)] ISNT '|'), "Bad character");
      world->squares[world->number_rows][col] SET_TO c;
      if (c IS '.')
	{
	  world->cols[col].dry--;
	  world->rows[world->number_rows].dry--;
	}
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

Called By:  read_problem

Side Effects: This reads the rows of the problem and builds parts of
the _world model. In verbose mode, this prints the number of rows.

Notes:

The row_length is set by read_dashes. When the bottom row of dashes is
read, the string length is less than row_length, so the "for" loop
reading rows is exited.

*/

int read_rows(             /* ARGUMENTS                 */
 FILE * in_port,           /* port to file to read from */
 char * buffer,            /* buffer to read rows into  */
 int row_length,           /* length of a row           */
 struct bap_world * world) /* puzzle model              */
{
  const char * name SET_TO "read_rows";
  int col;

  for (; ; )
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "File ends while reading rows");
      if (strlen(buffer) < row_length)
	break;
      IFF(read_row(buffer, world));
    }
  if (world->print_all)
    printf("%d rows\n", world->number_rows);
  for (col SET_TO 0; col < world->number_cols; col++)
    world->cols[col].dry SET_TO (world->cols[col].dry + world->number_rows);
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

Called By:  read_problem

Side Effects: This reads zero to four lines of ships, in order
(battleships, cruisers, destroyers, submarines) and the "end" line. It
also records the number of each type of ship.

Setting ship numbers to zero should not be necessary, since it is done
in init_world, but it is done here just to be sure.

This will report an error if a ships line has bad format in any way,
such as line spacing being incorrect (including extra spaces or tabs
after the last ship on the line).

In verbose mode, this prints the number of each type of ship.

Notes:

The first line after the column numbers must have nothing but a
newline, and it has been read before this function is called.
This starts reading on the second line after the column numbers
and reads through lines representing ships in decreasing size order.
Then it reads the line "end". There should be no blank lines before
the "end" line. It does not matter what comes after the "end" line.

This does not signal an error if there are no ships.

*/

int read_ships(            /* ARGUMENTS                 */
 FILE * in_port,           /* port to file to read from */
 char * buffer,            /* buffer to read ships into */
 struct bap_world * world) /* puzzle model              */
{
  const char * name SET_TO "read_ships";
  int n;
  int sum_aim;
  int sum_ship;

  world->battleships_left SET_TO 0;
  world->cruisers_left SET_TO 0;
  world->destroyers_left SET_TO 0;
  world->submarines_left_col SET_TO 0;
  world->submarines_left_row SET_TO 0;
  CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
      "File ends with no end marker");
  for (n SET_TO 0; ; n SET_TO (n + 6))
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
  for (n SET_TO 0; ; n SET_TO (n + 5))
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
  for (n SET_TO 0; ; n SET_TO (n + 4))
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
  for (n SET_TO 0; ; n SET_TO (n + 3))
    {
      if (strncmp((buffer + n), "  O", 3) IS 0)
	{
	  world->submarines_left_col++;
	  world->submarines_left_row++;
	}
      else
	break;
    }
  CHB(((n ISNT 0) AND (buffer[n] ISNT '\n')), "Bad submarines line");
  if (world->print_all)
    printf("%d submarines\n", world->submarines_left_col);
  if (world->submarines_left_col ISNT 0)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "File ends with no end marker");
    }
  CHB((strncmp(buffer, "end", 3) ISNT 0), "File ends with no end marker");
  sum_ship SET_TO
    ((4 * world->battleships_left) +
     (3 * world->cruisers_left) +
     (2 * world->destroyers_left) +
     (world->submarines_left_row));
  sum_aim SET_TO 0;
  for (n SET_TO 0; n < world->number_rows; n++)
    sum_aim SET_TO (sum_aim + world->rows[n].aim);
  CHB((sum_aim ISNT sum_ship), "Bad row count total");
  sum_aim SET_TO 0;
  for (n SET_TO 0; n < world->number_cols; n++)
    sum_aim SET_TO (sum_aim + world->cols[n].aim);
  CHB((sum_aim ISNT sum_ship), "Bad column count total");
  return OK;
}

/*************************************************************************/

/* record_progress

Returned Value: int (OK)

Called By:
  assign_battleships
  assign_cruisers
  assign_destroyers
  assign_submarines
  extend_horizontal
  extend_vertical
  fill_bap
  insert_guess_col
  insert_guess_row
  place_battleships
  place_cruisers
  place_destroyers
  try_full
  try_full_but_one
  try_square_blank

Side Effects: If progress has been made, a line of explanation is
recorded, and in verbose mode, the puzzle is printed.

*/

int record_progress(       /* ARGUMENTS       */
 char * tried,             /* text of message */
 struct bap_world * world) /* puzzle model    */
{
  const char * name SET_TO "record_progress";
  int numb;     /* number of characters printed       */
  int n;        /* counter for rows_in and columns_in */
  char * where; /* where to put next characters       */

  if (world->progress ISNT 0)
    {
      numb SET_TO sprintf(world->logic[world->logic_line], tried);
      where SET_TO (world->logic[world->logic_line] + numb);
      if (world->number_in_rows > 0)
	{
	  sprintf(where, "row");
	  where SET_TO (where + 3);
	  numb SET_TO
	    sprintf(where, ((world->number_in_rows > 1) ? "s " : " "));
	  where SET_TO (where + numb);
	  for (n SET_TO 0; n < world->number_in_rows; n++)
	    {
	      numb SET_TO sprintf(where, "%d ", (1 + world->rows_in[n]));
	      where SET_TO (where + numb);
	    }
	}
      if (world->number_in_cols > 0)
	{
	  if (world->number_in_rows > 0)
	    {
	      sprintf(where, "and ");
	      where SET_TO (where + 4);
	    }
	  sprintf(where, "column");
	  where SET_TO (where + 6);
	  numb SET_TO
	    sprintf(where, ((world->number_in_cols > 1) ? "s " : " "));
	  where SET_TO (where + numb);
	  for (n SET_TO 0; n < world->number_in_cols; n++)
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

/* ship_covers_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_col_row returns ERROR.

Called By:  try_square_blank

Side Effects: This tries to fit a ship of the given size into the given
column, covering the square in the given row, without creating any
illegal conditions. If it succeeds, it sets *cover to 1. Otherwise,
it sets *cover to 0.

*/

int ship_covers_col(       /* ARGUMENTS                 */
 int row,                  /* index of row of square    */
 int col,                  /* index of column of square */
 int size,                 /* size of ship              */
 int * cover,              /* set here                  */
 struct bap_world * world) /* puzzle model              */
{
  const char * name SET_TO "ship_covers_col";
  int start;
  int stop;
  int n;
  int ys;

  start SET_TO (row - (size - 1));
  if (start < 0)
    start SET_TO 0;
  stop SET_TO (row + 1);
  if (stop > (world->number_rows - (size - 1)))
    stop SET_TO (world->number_rows - (size - 1));
  for (n SET_TO start; n < stop; n++)
    {
      IFF(ship_fits_col_row(col, n, size, &ys, cover, world));
      if (ys + world->cols[col].got > world->cols[col].aim)
	*cover SET_TO 0;
      if (*cover)
	break;
    }
  return OK;
}

/*************************************************************************/

/* ship_covers_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_row_col returns ERROR.

Called By:  try_square_blank

Side Effects: This tries to fit a ship of the given size into the given
row, covering the square in the given column, without creating any
illegal conditions. If it succeeds, it sets *cover to 1. Otherwise,
it sets *cover to 0.

Notes:

If there are not enough columns for a ship of the given size to fit,
this fails. Might add a check when reading that there are at least
four rows and four columns.

*/

int ship_covers_row(       /* ARGUMENTS                 */
 int row,                  /* index of row of square    */
 int col,                  /* index of column of square */
 int size,                 /* size of ship              */
 int * cover,              /* set here                  */
 struct bap_world * world) /* puzzle model              */
{
  const char * name SET_TO "ship_covers_row";
  int start;
  int stop;
  int k;
  int ys;

  start SET_TO (col - (size - 1));
  if (start < 0)
    start SET_TO 0;
  stop SET_TO (col + 1);
  if (stop > (world->number_cols - (size - 1)))
    stop SET_TO (world->number_cols - (size - 1));
  for (k SET_TO start; k < stop; k++)
    {
      IFF(ship_fits_row_col(row, k, size, &ys, cover, world));
      if (ys + world->rows[row].got > world->rows[row].aim)
	*cover SET_TO 0;
      if (*cover)
	break;
    }
  return OK;
}

/*************************************************************************/

/* ship_fits_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_col_row returns ERROR.

Called By:
  assign_battleships
  assign_cruisers
  assign_destroyers
  assign_submarines_cols

Side Effects: This sets *fits to the number of ships of a given size
that can fit in a column in addition to the ones of that size already
located there.

*/

int ship_fits_col(         /* ARGUMENTS                         */
 int col,                  /* index of column being checked     */
 int size,                 /* size of ship                      */
 int * fits,               /* set here to number that fit newly */
 struct bap_world * world) /* puzzle model                      */
{
  const char * name SET_TO "ship_fits_col";
  int row;
  int rows;
  int is_legal;
  int ys;       /* number of additional ship parts in column from one ship  */
  int all_ys;   /* number of additional ship parts in column from all ships */

  *fits SET_TO 0;
  all_ys SET_TO 0;
  rows SET_TO world->number_rows;
  for (row SET_TO 0; row < (rows - (size - 1)); row++)
    {
      IFF(ship_fits_col_row(col, row, size, &ys, &is_legal, world));
      if (is_legal)
	{
	  if ((world->cols[col].got + ys + all_ys) > world->cols[col].aim)
	    is_legal SET_TO 0;
	}
      if (is_legal)
	{
	  if (NOT (world->locations[row][col]))
	    *fits SET_TO (*fits + 1);
	  row SET_TO (row + size);
	  all_ys SET_TO (all_ys + ys);
	}
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
  3. legal_col_beside returns ERROR.

Called By:
  find_max_starts_col
  find_min_starts_col
  make_guess_ship_col
  ship_covers_col
  ship_fits_col
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

This builds a test column used by legal_col_across and
legal_col_beside.  The ship to be tested is inserted in the test
column starting at the given row. Squares of the ship that were
previously blank are marked with Y. Blank squares in the test column
outside the ship are marked as water. This is only for the convenience
of legal_col_across and legal_col_beside, not because they are all
necessarily water (although the two at the ends of the ship are
necessarily water).

*/

int ship_fits_col_row(     /* ARGUMENTS                                 */
 int col,                  /* index of column being checked             */
 int row,                  /* index of row being checked                */
 int size,                 /* size of ship                              */
 int * ys,                 /* number of additional ship parts, set here */
 int * is_legal,           /* set here to 0 or 1                        */
 struct bap_world * world) /* puzzle model                              */
{
  const char * name SET_TO "ship_fits_col_row";
  int rows;
  char item1;
  char item2;
  char item3;
  char item4;
  char test[MAX_TEST];
  int n;
  int total;

  CHB(((size < 1) OR (size > 4)), "Bug bad size");
  *ys SET_TO 0;
  *is_legal SET_TO 1;
  item1 SET_TO world->squares[row][col];
  if (((square(row - 1, col - 1, world) ISNT '.') AND
       (square(row - 1, col - 1, world) ISNT ' ')) OR
      ((square(row - 1, col, world) ISNT '.') AND
       (square(row - 1, col, world) ISNT ' ')) OR
      ((square(row - 1, col + 1, world) ISNT '.') AND
       (square(row - 1, col + 1, world) ISNT ' ')) OR
      ((square(row, col - 1, world) ISNT '.') AND
       (square(row, col - 1, world) ISNT ' ')) OR
      ((square(row, col + 1, world) ISNT '.') AND
       (square(row, col + 1, world) ISNT ' ')) OR
      ((square(row + 1, col - 1, world) ISNT '.') AND
       (square(row + 1, col - 1, world) ISNT ' ')) OR
      ((square(row + 1, col + 1, world) ISNT '.') AND
       (square(row + 1, col + 1, world) ISNT ' ')))
    *is_legal SET_TO 0;
  if (*is_legal AND (size > 1))
    {
      item2 SET_TO world->squares[row + 1][col];
      if (((square(row + 2, col - 1, world) ISNT '.') AND
	   (square(row + 2, col - 1, world) ISNT ' ')) OR
	  ((square(row + 2, col + 1, world) ISNT '.') AND
	   (square(row + 2, col + 1, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal AND (size > 2))
    {
      item3 SET_TO world->squares[row + 2][col];
      if (((square(row + 3, col - 1, world) ISNT '.') AND
	   (square(row + 3, col - 1, world) ISNT ' ')) OR
	  ((square(row + 3, col + 1, world) ISNT '.') AND
	   (square(row + 3, col + 1, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal AND (size > 3))
    {
      item4 SET_TO world->squares[row + 3][col];
      if (((square(row + 4, col - 1, world) ISNT '.') AND
	   (square(row + 4, col - 1, world) ISNT ' ')) OR
	  ((square(row + 4, col + 1, world) ISNT '.') AND
	   (square(row + 4, col + 1, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal)
    {
      if (size IS 1)
	{
	  if (((item1 ISNT 'O') AND (item1 ISNT 'X') AND (item1 ISNT ' ')) OR
	      ((square(row + 1, col, world) ISNT '.') AND
	       (square(row + 1, col, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 2)
	{
	  if (((item1 ISNT '^') AND (item1 ISNT 'X') AND (item1 ISNT ' ')) OR
	      ((item2 ISNT 'v') AND (item2 ISNT 'X') AND (item2 ISNT ' ')) OR
	      ((square(row + 2, col, world) ISNT '.') AND
	       (square(row + 2, col, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 3)
	{
	  if (((item1 ISNT '^') AND (item1 ISNT 'X') AND (item1 ISNT ' ')) OR
	      ((item2 ISNT '+') AND (item2 ISNT 'X') AND (item2 ISNT ' ')) OR
	      ((item3 ISNT 'v') AND (item3 ISNT 'X') AND (item3 ISNT ' ')) OR
	      ((square(row + 3, col, world) ISNT '.') AND
	       (square(row + 3, col, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 4)
	{
	  if (((item1 ISNT '^') AND (item1 ISNT 'X') AND (item1 ISNT ' ')) OR
	      ((item2 ISNT '+') AND (item2 ISNT 'X') AND (item2 ISNT ' ')) OR
	      ((item3 ISNT '+') AND (item3 ISNT 'X') AND (item3 ISNT ' ')) OR
	      ((item4 ISNT 'v') AND (item4 ISNT 'X') AND (item4 ISNT ' ')) OR
	      ((square(row + 4, col, world) ISNT '.') AND
	       (square(row + 4, col, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
    }
  if (*is_legal)
    {
      rows SET_TO world->number_rows;
      test[0] SET_TO '.';
      test[rows + 1] SET_TO '.';
      total SET_TO 0;
      for (n SET_TO 0; n < rows; n++)
	{
	  if ((n < row) OR (n > (row + (size - 1))))
	    {
	      if ((square(n, col, world) IS ' ') OR
		  (square(n, col, world) IS '.'))
		test[n + 1] SET_TO '.';
	      else
		{
		  test[n + 1] SET_TO square(n, col, world);
		  total++;
		}
	    }
	  else
	    {
	      total++;
	      if (square(n, col, world) IS ' ')
		{
		  test[n + 1] SET_TO 'Y';
		  *ys SET_TO (*ys + 1);
		}
	      else
		test[n + 1] SET_TO square(n, col, world);
	    }
	}
      if (total > world->cols[col].aim)
	*is_legal SET_TO 0;
      if (*is_legal)
	IFF(legal_col_across(test + 1, col, is_legal, world));
      if (*is_legal)
	IFF(legal_col_beside(test + 1, col, is_legal, world));
    }
  return OK;
}

/*************************************************************************/

/* ship_fits_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. ship_fits_row_col returns ERROR.

Called By:
  assign_battleships
  assign_cruisers
  assign_destroyers
  assign_submarines_rows

Side Effects: This sets *fits to the number of the given size that can
fit in a row in addition to the ones already located there.

*/

int ship_fits_row(         /* ARGUMENTS                         */
 int row,                  /* index of row being checked        */
 int size,                 /* size of ship                      */
 int * fits,               /* set here to number that fit newly */
 struct bap_world * world) /* puzzle model                      */
{
  const char * name SET_TO "ship_fits_row";
  int col;
  int cols;
  int is_legal;
  int ys;
  int all_ys;

  *fits SET_TO 0;
  all_ys SET_TO 0;
  cols SET_TO world->number_cols;
  for (col SET_TO 0; col < (cols - (size - 1)); col++)
    {
      IFF(ship_fits_row_col(row, col, size, &ys, &is_legal, world));
      if (is_legal)
	{
	  if ((world->rows[row].got + ys + all_ys) > world->rows[row].aim)
	    is_legal SET_TO 0;
	}
      if (is_legal)
	{
	  if (NOT (world->locations[row][col]))
	    *fits SET_TO (*fits + 1);
	  col SET_TO (col + size);
	  all_ys SET_TO (all_ys + ys);
	}
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
  3. legal_row_beside returns ERROR.

Called By:
  find_max_starts_row
  find_min_starts_row
  make_guess_ship_row
  ship_covers_row
  ship_fits_row
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

This builds a test row used by legal_row_across and
legal_row_beside.  The ship to be tested is inserted in the test
row starting at the given col. Squares of the ship that were
previously blank are marked with Y. Blank squares in the test row
outside the ship are marked as water. This is only for the convenience
of legal_row_across and legal_row_beside, not because they are all
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
  const char * name SET_TO "ship_fits_row_col";
  int cols;
  char item1;
  char item2;
  char item3;
  char item4;
  char test[MAX_TEST];
  int n;
  int total;

  CHB(((size < 1) OR (size > 4)), "Bug bad size");
  *ys SET_TO 0;
  *is_legal SET_TO 1;
  item1 SET_TO world->squares[row][col];
  if (((square(row - 1, col - 1, world) ISNT '.') AND
       (square(row - 1, col - 1, world) ISNT ' ')) OR
      ((square(row, col - 1, world) ISNT '.') AND
       (square(row, col - 1, world) ISNT ' ')) OR
      ((square(row + 1, col - 1, world) ISNT '.') AND
       (square(row + 1, col - 1, world) ISNT ' ')) OR
      ((square(row - 1, col, world) ISNT '.') AND
       (square(row - 1, col, world) ISNT ' ')) OR
      ((square(row + 1, col, world) ISNT '.') AND
       (square(row + 1, col, world) ISNT ' ')) OR
      ((square(row - 1, col + 1, world) ISNT '.') AND
       (square(row - 1, col + 1, world) ISNT ' ')) OR
      ((square(row + 1, col + 1, world) ISNT '.') AND
       (square(row + 1, col + 1, world) ISNT ' ')))
    *is_legal SET_TO 0;
  if (*is_legal AND (size > 1))
    {
      item2 SET_TO world->squares[row][col + 1];
      if (((square(row - 1, col + 2, world) ISNT '.') AND
	   (square(row - 1, col + 2, world) ISNT ' ')) OR
	  ((square(row + 1, col + 2, world) ISNT '.') AND
	   (square(row + 1, col + 2, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal AND (size > 2))
    {
      item3 SET_TO world->squares[row][col + 2];
      if (((square(row - 1, col + 3, world) ISNT '.') AND
	   (square(row - 1, col + 3, world) ISNT ' ')) OR
	  ((square(row + 1, col + 3, world) ISNT '.') AND
	   (square(row + 1, col + 3, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal AND (size > 3))
    {
      item4 SET_TO world->squares[row][col + 3];
      if (((square(row - 1, col + 4, world) ISNT '.') AND
	   (square(row - 1, col + 4, world) ISNT ' ')) OR
	  ((square(row + 1, col + 4, world) ISNT '.') AND
	   (square(row + 1, col + 4, world) ISNT ' ')))
	*is_legal SET_TO 0;
    }
  if (*is_legal)
    {
      if (size IS 1)
	{
	  if (((item1 ISNT 'O') AND (item1 ISNT 'X') AND (item1 ISNT ' ')) OR
	      ((square(row, col + 1, world) ISNT '.') AND
	       (square(row, col + 1, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 2)
	{
	  if (((item1 ISNT '<') AND (item1 ISNT 'X') AND (item1 ISNT ' ')) OR
	      ((item2 ISNT '>') AND (item2 ISNT 'X') AND (item2 ISNT ' ')) OR
	      ((square(row, col + 2, world) ISNT '.') AND
	       (square(row, col + 2, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 3)
	{
	  if (((item1 ISNT '<') AND (item1 ISNT 'X') AND (item1 ISNT ' ')) OR
	      ((item2 ISNT '+') AND (item2 ISNT 'X') AND (item2 ISNT ' ')) OR
	      ((item3 ISNT '>') AND (item3 ISNT 'X') AND (item3 ISNT ' ')) OR
	      ((square(row, col + 3, world) ISNT '.') AND
	       (square(row, col + 3, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
      else if (size IS 4)
	{
	  if (((item1 ISNT '<') AND (item1 ISNT 'X') AND (item1 ISNT ' ')) OR
	      ((item2 ISNT '+') AND (item2 ISNT 'X') AND (item2 ISNT ' ')) OR
	      ((item3 ISNT '+') AND (item3 ISNT 'X') AND (item3 ISNT ' ')) OR
	      ((item4 ISNT '>') AND (item4 ISNT 'X') AND (item4 ISNT ' ')) OR
	      ((square(row, col + 4, world) ISNT '.') AND
	       (square(row, col + 4, world) ISNT ' ')))
	    *is_legal SET_TO 0;
	}
    }
  if (*is_legal)
    {
      cols SET_TO world->number_cols;
      test[0] SET_TO '.';
      test[cols + 1] SET_TO '.';
      total SET_TO 0;
      for (n SET_TO 0; n < cols; n++)
	{
	  if ((n < col) OR (n > (col + (size - 1))))
	    {
	      if ((square(row, n, world) IS ' ') OR
		  (square(row, n, world) IS '.'))
		test[n + 1] SET_TO '.';
	      else
		{
		  test[n + 1] SET_TO square(row, n, world);
		  total++;
		}
	    }
	  else
	    {
	      total++;
	      if (square(row, n, world) IS ' ')
		{
		  test[n + 1] SET_TO 'Y';
		  *ys SET_TO (*ys + 1);
		}
	      else
		test[n + 1] SET_TO square(row, n, world);
	    }
	}
      if (total > world->rows[row].aim)
	*is_legal SET_TO 0;
      if (*is_legal)
	IFF(legal_row_across(test + 1, row, is_legal, world));
      if (*is_legal)
	IFF(legal_row_beside(test + 1, row, is_legal, world));
    }
  return OK;
}

/*************************************************************************/

/* solve_problem

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. One of the following functions returns ERROR:
     assign_battleships, assign_cruisers, assign_destroyers,
     assign_submarines, dex_squares, explain_bap, extend_middles, fill_bap,
     locate_ships, mark_dones, place_battleships, place_cruisers,
     place_destroyers, print_bap, try_full, try_full_but_one, try_guessing,
     try_square_blank, verify_squares.

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
4.  Fill with water the blanks of any strip that has all the ship parts
    it needs but still has some blanks (fill_bap).
5.  Extend with X's (unknown ship parts) any ship middles that
    can be extended (extend_middles).
6.  Change to a known ship part any unknown ship part whose identify
    can be determined (dex_squares).
7.  Fill with ship parts any unfilled strip that has only as many dry
    squares as the number of ship parts it needs (try_full).
8.  Fill with ship parts any squares of [any strip that has only one
    fewer dry squares than the number of ship parts it needs] which must
    be filled no matter where the one additional water square is
    (legally) placed (try_full_but_one)
9.  Assign battleships to strips, if possible (assign_battleships).
10. Place battleship parts in strips where battleships are assigned,
    if possible (place_battleships).
11. Assign cruisers to strips, if possible (assign_cruisers).
12. Place cruiser parts in strips where cruisers are assigned,
    if possible (place_cruisers).
13. Assign destroyers to strips, if possible (assign_destroyers).
14. Place destroyer parts in strips where destroyers are assigned,
    if possible (place_destroyers).
15. Assign submarines to strips, if possible (assign_submarines).
16. Look at each blank square, one at a time, and determine if it can
    be covered by any available ship. If not, mark it water
    (try_square_blank).
17. If done, print the puzzle and the explanation of how it was solved.
    If not, return whatever try_guessing returns.

The function goes down the list of things to try. verify_squares and
locate_ships are always run. They do not fill in any squares. After
that, as soon as one of the functions succeeds in filling in even one
square, solve_problem goes back to the top of the list (skipping the
remaining, harder, things to try) and goes down it again. The easier
things to try (even the same thing that worked last time) may now work
because more of the puzzle is filled in.

If all the things to try do not solve the puzzle, call try_guessing,
which calls solve_problem recursively.

*/

int solve_problem(         /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "solve_problem";

  for (world->progress SET_TO 0; world->undone ISNT 0; world->progress SET_TO 0)
    {
      IFF(verify_squares(world));
      IFF(locate_ships(world));
      IFF(mark_dones(world));
      if (world->progress)
	continue;
      IFF(fill_bap(world));
      if (world->progress)
	continue;
      IFF(extend_middles(world));
      if (world->progress)
	continue;
      IFF(dex_squares(world));
      if (world->progress)
	continue;
      IFF(try_full(world));
      if (world->progress)
	continue;
      IFF(try_full_but_one(world));
      if (world->progress)
	continue;
      if (world->battleships_left)
	{
	  IFF(assign_battleships(world));
	  if (world->progress)
	    continue;
	}
      if (world->battleships_ass)
	{
	  IFF(place_battleships(world));
	  if (world->progress)
	    continue;
	}
      if (world->cruisers_left)
	{
	  IFF(assign_cruisers(world));
	  if (world->progress)
	    continue;
	}
      if (world->cruisers_ass)
	{
	  IFF(place_cruisers(world));
	  if (world->progress)
	    continue;
	}
      if (world->destroyers_left)
	{
	  IFF(assign_destroyers(world));
	  if (world->progress)
	    continue;
	}
      if (world->destroyers_ass)
	{
	  IFF(place_destroyers(world));
	  if (world->progress)
	    continue;
	}
      IFF(assign_submarines(world));
      if (world->progress)
	continue;
      IFF(try_square_blank(world));
      if (world->progress)
	continue;
      else
	break;
    }
  if (world->undone IS 0)
    {
      IFF(print_bap(world));
      IFF(explain_bap(world));
    }
  else
    {
      IFF(try_guessing(world));
    }
  return OK;
}

/*************************************************************************/

/* square

Returned Value: char

This returns the actual character of world->squares in the row, col
position if the square is inside the defined area, and returns water
('.') otherwise.

Called By:
  assign_submarines_cols
  assign_submarines_rows
  dex_ends_col
  dex_ends_row
  dex_square
  extend_ends
  extend_horizontal
  extend_middle
  extend_vertical
  legal_col_across
  legal_col_beside
  legal_row_across
  legal_row_beside
  locate_ships
  make_water
  place_ships_col
  place_ships_col_water
  place_ships_row
  place_ships_row_water
  ship_fits_col_row
  ship_fits_row_col
  try_full
  try_full_but_one_col
  try_full_but_one_row
  try_square_blank
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
  const char * name SET_TO "square";
  char return_value;

  if ((row > -1) AND
      (row < world->number_rows) AND
      (col > -1) AND
      (col < world->number_cols))
    return_value SET_TO world->squares[row][col];
  else
    return_value SET_TO '.';
  return return_value;
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
  try_square_blank

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
  const char * name SET_TO "submarine_covers";
  int ys;

  IFF(ship_fits_row_col(row, col, 1, &ys, cover, world));
  if (*cover IS 1)
    IFF(ship_fits_col_row(col, row, 1, &ys, cover, world));
  return OK;
}

/*************************************************************************/

/* try_full

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. enx_square returns ERROR.
  2. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: This reads the rows and columns looking for ones for
which the aim number is the same as the dry number (meaning all the
dry squares should be filled if not already filled), but for which the
got number is less than the aim number (meaning not all squares that
should be filled are filled). Whenever one is found, all the blank
squares are filled with X's. A line of explanation is written if any
row or column filled.

*/

int try_full(              /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "try_full";
  int n;
  int k;
  int reading_ship; /* 1=have started reading a ship, 0=not */
  int filled_one;

  world->number_in_rows SET_TO 0;
  world->number_in_cols SET_TO 0;
  reading_ship SET_TO 0;
  for (n SET_TO 0; n < world->number_rows; n++)
    {
      if ((world->rows[n].done) OR (world->rows[n].dry > world->rows[n].aim))
	continue;
      filled_one SET_TO 0;
      for (k SET_TO 0; k < world->number_cols; k++)
	{
	  if (square(n, k, world) IS ' ')
	    {
	      IFF(enx_square(n, k, world));
	      filled_one SET_TO 1;
	    }
	}
      if (filled_one)
	world->rows_in[world->number_in_rows++] SET_TO n;
    }
  reading_ship SET_TO 0;
  for (k SET_TO 0; k < world->number_cols; k++)
    {
      if ((world->cols[k].done) OR (world->cols[k].dry > world->cols[k].aim))
	continue;
      filled_one SET_TO 0;
      for (n SET_TO 0; n < world->number_rows; n++)
	{
	  if (square(n, k, world) IS ' ')
	    {
	      enx_square(n, k, world);
	      filled_one SET_TO 1;
	    }
	}
      if (filled_one)
	world->cols_in[world->number_in_cols++] SET_TO k;
    }
  IFF(record_progress("fill blanks with ship in ", world));
  return OK;
}

/*************************************************************************/

/* try_full_but_one

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. try_full_but_one_row returns ERROR.
  2. try_full_but_one_col returns ERROR.
  3. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: This reads the rows and columns looking for ones for
which the aim number is one less than the dry number (meaning one of the
dry squares should be water and the rest ship). For each such strip,
a copy of the strip is made with one blank filled with water and the
rest filled with ship. The copy is tested for being legal. Any position
in the strip that was blank but is filled with ship in every legal
copy of the strip must be ship, so an X is placed in that position.
A line of explanation is written if any X's are marked.

*/

int try_full_but_one(      /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "try_full_but_one";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  world->number_in_cols SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    if (world->rows[row].dry IS (world->rows[row].aim + 1))
      IFF(try_full_but_one_row(row, world));
  for (col SET_TO 0; col < world->number_cols; col++)
    if (world->cols[col].dry IS (world->cols[col].aim + 1))
      IFF(try_full_but_one_col(col, world));
  IFF(record_progress("fill blanks that must be ship in ", world));
  return OK;
}

/*************************************************************************/

/* try_full_but_one_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. legal_col returns ERROR.
  2. There is no legal way to fill the column: "Puzzle has no answer".

Called By:  try_full_but_one

Side Effects: See try_full_but_one_row. This is similar for columns.

Notes:

test is made with extra squares at both ends filled with water so that
checking one square beyond the ends will work.

*/

int try_full_but_one_col(  /* ARGUMENTS     */
 int col,                  /* column to try */
 struct bap_world * world) /* puzzle model  */
{
  const char * name SET_TO "try_full_but_one_col";
  char test[MAX_TEST];
  int n;
  int row;
  int filled_one;
  int is_legal;
  int found_legal;

  filled_one SET_TO 0;
  found_legal SET_TO 0;
  test[0] SET_TO '.';
  test[world->number_rows + 1] SET_TO '.';
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if (world->squares[row][col] ISNT ' ')
	continue;
      for (n SET_TO 0; n < world->number_rows; n++)
	{
	  test[n + 1] SET_TO
	    ((n IS row) ? '.' :
	     (square(n, col, world) IS ' ') ? 'Y' : square(n, col, world));
	}
      IFF(legal_col(test + 1, col, &is_legal, world));
      if (is_legal)
	found_legal SET_TO 1;
      else
	{
	  enx_square(row, col, world);
	  filled_one SET_TO 1;
	}
    }
  if (filled_one)
    world->cols_in[world->number_in_cols++] SET_TO col;
  if (NOT (found_legal))
    CHK(1, "Puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* try_full_but_one_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. legal_row returns ERROR.
  2. There is no legal way to fill the row: "Puzzle has no answer".

Called By:  try_full_but_one

Side Effects: This is called if all the blanks but one in a row must be
filled with ship parts. The remaining one must be filled with water.

For each blank in the row, this makes a copy of the row with water in
the position of that blank and fills the other blanks of the copy
with ship parts.  Then it tests whether the result is legal.
If not the blank is set to X.

test is made with extra squares at both ends filled with water so that
checking one square beyond the ends will work.

*/

int try_full_but_one_row(  /* ARGUMENTS    */
 int row,                  /* row to try   */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "try_full_but_one_row";
  char test[MAX_TEST];
  int n;
  int col;
  int filled_one;
  int is_legal;
  int found_legal;

  test[0] SET_TO '.';
  test[world->number_cols + 1] SET_TO '.';
  filled_one SET_TO 0;
  found_legal SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      if (world->squares[row][col] ISNT ' ')
	continue;
      for (n SET_TO 0; n < world->number_cols; n++)
	{
	  test[n + 1] SET_TO
	    ((n IS col) ? '.' :
	     (square(row, n, world) IS ' ') ? 'Y' : square(row, n, world));
	}
      IFF(legal_row(test + 1, row, &is_legal, world));
      if (is_legal)
	found_legal SET_TO 1;
      else
	{
	  enx_square(row, col, world);
	  filled_one SET_TO 1;
	}
    }
  if (filled_one)
    world->rows_in[world->number_in_rows++] SET_TO row;
  if (NOT (found_legal))
    CHK(1, "Puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* try_guessing

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. No answer is found: "Cannot solve puzzle".
  2. make_guesses_battleships returns ERROR.
  3. make_guesses_cruisers returns ERROR.
  4. make_guesses_destroyers returns ERROR.
  5. copy_bap returns ERROR.
  6. insert_guess returns ERROR.

Called By:  solve_problem

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
  const char * name SET_TO "try_guessing";
  int number_guesses;
  int n;
  struct guess guesses[MAX_GUESSES];
  int result;
  int return_value;
  struct bap_world world_copy;

  return_value SET_TO ERROR;
  number_guesses SET_TO (MAX_GUESSES + 1);
  if (world->battleships_left OR world->battleships_ass)
    IFF(make_guesses_battleships(&number_guesses, guesses, world));
  else if (world->cruisers_left OR world->cruisers_ass)
    IFF(make_guesses_cruisers(&number_guesses, guesses, world));
  else if (world->destroyers_left OR world->destroyers_ass)
    IFF(make_guesses_destroyers(&number_guesses, guesses, world));
  else if ((world->submarines_left_row) OR
	   (world->submarines_ass_row) OR
	   (world->submarines_left_col) OR
	   (world->submarines_ass_col))
    IFF(make_guesses_submarines(&number_guesses, guesses, world));
  else
    CHB(1, "Bug in try_guessing");
  for (n SET_TO 0; n < number_guesses; n++)
    {
      IFF(copy_bap(&world_copy, world));
      if (guesses[n].in_row)
	IFF(insert_guess_row(&(guesses[n]), &world_copy));
      else
	IFF(insert_guess_col(&(guesses[n]), &world_copy));
      result SET_TO solve_problem(&world_copy);
      IFF(copy_guess(&(world->guesses_tried[world->number_guesses_tried++]),
		     &(guesses[n])));
      if (result IS OK)
	{
	  return_value SET_TO OK;
	  if (world->find_all IS 0)
	    break;
	}
    }
  IFF(return_value);
  return OK;
}

/*************************************************************************/

/* try_square_blank

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. make_water returns ERROR.
  2. record_progress returns ERROR.
  3. ship_covers_col returns ERROR.
  4. ship_covers_row returns ERROR.
  5. submarine_covers returns ERROR.

Called By:  solve_problem

Side Effects: If the square at the given row and column cannot be ship,
it is marked as water.

*/

int try_square_blank(      /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "try_square_blank";
  int row;
  int col;
  int cover;

  for (row SET_TO 0; row < world->number_rows; row++)
    {
      if (world->rows[row].done)
	continue;
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  if (square(row, col, world) ISNT ' ')
	    continue;
	  cover SET_TO 0;
	  if ((world->rows[row].submarines OR world->submarines_left_row))
	    IFF(submarine_covers(row, col, &cover, world));
	  if (cover)
	    continue;
	  if (world->rows[row].destroyers OR world->destroyers_left)
	    IFF(ship_covers_row(row, col, 2, &cover, world));
	  if (cover)
	    continue;
	  if (world->cols[col].destroyers OR world->destroyers_left)
	    IFF(ship_covers_col(row, col, 2, &cover, world));
	  if (cover)
	    continue;
	  if (world->rows[row].cruisers OR world->cruisers_left)
	    IFF(ship_covers_row(row, col, 3, &cover, world));
	  if (cover)
	    continue;
	  if (world->cols[col].cruisers OR world->cruisers_left)
	    IFF(ship_covers_col(row, col, 3, &cover, world));
	  if (cover)
	    continue;
	  if (world->rows[row].battleships OR world->battleships_left)
	    IFF(ship_covers_row(row, col, 4, &cover, world));
	  if (cover)
	    continue;
	  if (world->cols[col].battleships OR world->battleships_left)
	    IFF(ship_covers_col(row, col, 4, &cover, world));
	  if (cover IS 0)
	    break;
	}
      if (cover IS 0)
	break;
    }
  if (cover IS 0)
    {
      IFF(make_water(row, col, world));
      world->number_in_rows SET_TO 1;
      world->number_in_cols SET_TO 1;
      world->rows_in[0] SET_TO row;
      world->cols_in[0] SET_TO col;
      IFF(record_progress
	  ("must be water in square at intersection of ", world));
    }
  return OK;
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
be a ship part, one of <>^vOX+

Notes:

The squares diagonally adjacent to the ship part being checked must
all be water. This function does not need to check that because pad_bap
checks that initially and any other square is filled by enx_square, which
checks that.

The sides of the puzzle are dealt with by using the function "square"
which returns water ('.') along the sides of the puzzle.

The first four cases handle ship ends, one of <>^v
1. A ship end must have its continuation side adjacent to one of:
   a. a mating ship end,
   b. a ship middle ('+'),
   c. a ship part of unknown type ('X'),
   d. a blank.
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

The seventh case is an unknown ship part, for which:
1. Any vertically or horizontally adjacent ship end must have its
   continuation side facing the unknown ship part.
2. A submarine may not be vertically or horizontally adjacent.

It is not necessary to check for "L" configurations because pad_bap
will detect any given in the statement of the puzzle, and the solving
procedure never makes them. Examples of "L" configurations follow.

   ^    XX
   +>   X

*/

int verify_square(         /* ARGUMENTS              */
 char item,                /* character in square    */
 int row,                  /* row index of square    */
 int col,                  /* column index of square */
 struct bap_world * world) /* puzzle model           */
{
  const char * name SET_TO "verify_square";
  char up;
  char dn;
  char rt;
  char lf;
  char switcher;           /* the item               */

  up SET_TO square(row - 1, col, world);
  dn SET_TO square(row + 1, col, world);
  rt SET_TO square(row, col + 1, world);
  lf SET_TO square(row, col - 1, world);
  switcher SET_TO item;
  if (switcher IS '<')
    {
      CHK(((rt ISNT '>') AND (rt ISNT '+') AND (rt ISNT 'X') AND (rt ISNT ' ')),
	  "Puzzle has no answer");
      CHK(((lf ISNT '.') AND (lf ISNT ' ')), "Puzzle has no answer");
      CHK(((up ISNT '.') AND (up ISNT ' ')), "Puzzle has no answer");
      CHK(((dn ISNT '.') AND (dn ISNT ' ')), "Puzzle has no answer");
    }
  else if (switcher IS '>')
    {
      CHK(((rt ISNT '.') AND (rt ISNT ' ')), "Puzzle has no answer");
      CHK(((lf ISNT '<') AND (lf ISNT '+') AND (lf ISNT 'X') AND (lf ISNT ' ')),
	  "Puzzle has no answer");
      CHK(((up ISNT '.') AND (up ISNT ' ')), "Puzzle has no answer");
      CHK(((dn ISNT '.') AND (dn ISNT ' ')), "Puzzle has no answer");
    }
  else if (switcher IS 'v')
    {
      CHK(((rt ISNT '.') AND (rt ISNT ' ')), "Puzzle has no answer");
      CHK(((lf ISNT '.') AND (lf ISNT ' ')), "Puzzle has no answer");
      CHK(((up ISNT '^') AND (up ISNT '+') AND (up ISNT 'X') AND (up ISNT ' ')),
	  "Puzzle has no answer");
      CHK(((dn ISNT '.') AND (dn ISNT ' ')), "Puzzle has no answer");
    }
  else if (switcher IS '^')
    {
      CHK(((rt ISNT '.') AND (rt ISNT ' ')), "Puzzle has no answer");
      CHK(((lf ISNT '.') AND (lf ISNT ' ')), "Puzzle has no answer");
      CHK(((up ISNT '.') AND (up ISNT ' ')), "Puzzle has no answer");
      CHK(((dn ISNT 'v') AND (dn ISNT '+') AND (dn ISNT 'X') AND (dn ISNT ' ')),
	  "Puzzle has no answer");
    }
  else if (switcher IS 'O')
    {
      CHK(((rt ISNT '.') AND (rt ISNT ' ')), "Puzzle has no answer");
      CHK(((lf ISNT '.') AND (lf ISNT ' ')), "Puzzle has no answer");
      CHK(((up ISNT '.') AND (up ISNT ' ')), "Puzzle has no answer");
      CHK(((dn ISNT '.') AND (dn ISNT ' ')), "Puzzle has no answer");
    }
  else if (switcher IS '+')
    {
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
      CHK((((rt IS '>') OR (rt IS '+') OR (rt IS 'X')) AND (lf IS '.')),
	  "Puzzle has no answer");
      CHK((((lf IS '<') OR (lf IS '+') OR (lf IS 'X')) AND (rt IS '.')),
	  "Puzzle has no answer");
      CHK((((up IS '^') OR (up IS '+') OR (up IS 'X')) AND (dn IS '.')),
	  "Puzzle has no answer");
      CHK((((dn IS 'v') OR (dn IS '+') OR (dn IS 'X')) AND (up IS '.')),
	  "Puzzle has no answer");
    }
  else if (switcher IS 'X')
    {
      CHK(((rt IS '<') OR (rt IS 'v') OR (rt IS '^') OR (rt IS 'O')),
	  "Puzzle has no answer");
      CHK(((lf IS '>') OR (lf IS 'v') OR (lf IS '^') OR (lf IS 'O')),
	  "Puzzle has no answer");
      CHK(((up IS 'v') OR (up IS '<') OR (up IS '>') OR (up IS 'O')),
	  "Puzzle has no answer");
      CHK(((dn IS '^') OR (dn IS '<') OR (dn IS '>') OR (dn IS 'O')),
	  "Puzzle has no answer");
    }
  else
    CHB(1, "Bug bad character undetected");
  return OK;
}

/*************************************************************************/

/* verify_squares

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. verify_square returns ERROR.
  2. A strip has too many ship parts: "Puzzle has no answer"
  3. A strip has too much water: "Puzzle has no answer"

Called By:  solve_problem

Side Effects: This checks that the world->squares layout is not
illegal. It looks at every square, and if it is a ship part, makes
sure it is possible to fit that part into a ship.

This also checks that:
a. the number of filled squares in each row and column is not larger
   than the total given for that row or column.
b. the number of non-water squares in each row and column is not smaller
   than the total given for that row or column.

Notes:

The squares diagonally adjacent to each ship part, which must all be
blank or water, are checked by pad_bap and enx_square (and every ship part
is examined by one or the other of those two functions), so they are not
checked here.

This does not check that the ships themselves are legal (not too many
of a given type, etc.).

*/

int verify_squares(        /* ARGUMENTS    */
 struct bap_world * world) /* puzzle model */
{
  const char * name SET_TO "verify_squares";
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
      CHK((rown.dry < rown.aim), "Puzzle has no answer");
    }
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      coln SET_TO world->cols[col];
      CHK((coln.got > coln.aim), "Puzzle has no answer");
      CHK((coln.dry < coln.aim), "Puzzle has no answer");
    }
  return OK;
}

/*************************************************************************/

