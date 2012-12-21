/*************************************************************************/

/* comments

The order of arguments for functions that make copies is to put
the destination (to) first and the source (from) second. The
reason for this is so that all copiers use the same order.
The strcpy function (included in the gen_gen_C language) already
uses that order, so that makes the choice.

This version has the try_contradict function removed.

*/

/*************************************************************************/

/* includes

*/

#include <stdio.h>
#include <math.h>
#include <string.h>

/*************************************************************************/

/* hash_defs

For the largest problems yet found, MAX_NUMBER_ROWS and MAX_NUMBER_COLS
need to be set to 200.

*/

#define AND       &&
#define IS        ==
#define ISNT      !=
#define NOT       !
#define OR        ||
#define SET_TO    =
#define OK        0
#define ERROR     -1
#define MAX_NUMBER_COLS 80
#define MAX_NUMBER_ROWS 80
#define TEXT_SIZE 256
#define STALLED 1
#define MAX_LOGIC 1024
#define MAX_STRIP_BLOCKS 20

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

A patch is the part of a strip in which a target block must lie. The
block must start at or after the patch start and must end at or before
the patch end.

Each patch starts out at its largest possible size and shrinks down to a
block the size of the target for the patch. Since each patch is identified
with a single target, the patches provide a way to work on targets. Patches
may start out overlapping, but one never contains another.

A patch is done when ((end + 1) - start) is target.

A strip is a row or column of the puzzle. The squares of the strip are
not in the strip class, but are in the squares of the paint model.

The "space" attribute of the paint_world structure is a double to allow
it to be large. It would make more sense to have it be a long int, but
gen_gen_c does not allow the "long" modifier.

*/

struct patch
{
  int done;   /* non-zero means patch is done               */
  int end;    /* strip index of last square of patch        */
  int start;  /* strip index of first square of patch       */
  int target; /* number of squares in target block of patch */
};

struct strip
{
  int number_patches;                     /* number of patches in the strip  */
  struct patch patches[MAX_STRIP_BLOCKS]; /* the patches of the strip        */
  int undone;                             /* number of patches not done      */
  int total;                              /* total squares to fill in strip  */
};

struct paint_world
{
  struct strip cols[MAX_NUMBER_COLS];  /* data on columns                    */
  int in_cols[MAX_NUMBER_COLS];   /* columns on which progress made          */
  int col_lines;                  /* number column lines, is max col targets */
  char col_numbers[MAX_STRIP_BLOCKS][TEXT_SIZE]; /* numbers above columns    */
  int depth;                      /* search depth                            */
  int find_all;                   /* set to 1 if all answers to be found     */
  char logic[MAX_LOGIC][TEXT_SIZE];  /* explanation of changes in puzzle     */
  int logic_line;                 /* number of next line of logic to write   */
  int number_cols;                /* number of columns in puzzle             */
  int number_in_cols;             /* number of colums on which progress made */
  int number_rows;                /* number of rows in puzzle                */
  int number_in_rows;             /* number of rows on which progress made   */
  int number_spaces;              /* number of blanks or digits before |     */
  int print_all;                  /* set to 1 for printing partial solutions */
  int progress_puzzle;            /* set to 1 if any square marked in puzzle */
  int progress_strip;             /* set to 1 if any square marked in strip  */
  struct strip rows[MAX_NUMBER_ROWS];  /* data on rows                       */
  int in_rows[MAX_NUMBER_ROWS];   /* rows on which progress made             */
  char row_numbers[MAX_NUMBER_ROWS][TEXT_SIZE]; /* numbers beside rows       */
  double space;                   /* estimated size space yet to be searched */
  char squares[MAX_NUMBER_ROWS][MAX_NUMBER_COLS]; /* array of squares        */
  int undone;                     /* number of strips not done               */
  int use_rows;                   /* 1 to use rows first 0 for columns first */
  int verbose;                    /* verbose (non-zero) or not (zero)        */
};

/*************************************************************************/

/* declare_functions

*/

int add_patch(struct strip * a_strip, int length);
int balanced(struct paint_world * world);
int balanced_down(struct paint_world * world);
int balanced_left(struct paint_world * world);
int balanced_right(struct paint_world * world);
int balanced_up(struct paint_world * world);
int copy_patch(struct patch * to, struct patch * from);
int copy_strip(struct strip * to, struct strip * from,
  struct paint_world * world);
int copy_world(struct paint_world * to, struct paint_world * world);
int enx_square(int index1, int index2, struct paint_world * world);
int explain_paint(struct paint_world * world);
int extend_block(int index1, int index2, int min_target, int length,
  struct paint_world * world);
int find_best_patch(int * strip_index, int * is_row, int * patch_index,
  int * many, struct paint_world * world);
int find_first_undone_target(struct strip * a_strip, int * target,
  struct paint_world * world);
int find_last_undone_target(struct strip * a_strip, int * target,
  struct paint_world * world);
int find_max_ends(int index1, int stop, struct strip * a_strip,
  struct paint_world * world);
int find_min_starts(int index1, int stop, struct strip * a_strip, int * found,
  struct paint_world * world);
int find_min_target(int index, struct strip * a_strip, int * size,
  struct paint_world * world);
int get_paint1(char * file_name, struct paint_world * world);
int get_paint2(char * file_name, struct paint_world * world);
char get_square(int index1, int index2, struct paint_world * world);
int init_patches(struct paint_world * world);
int init_patches_strip(int index1, int stop, struct strip * a_strip,
  struct paint_world * world);
int init_solution(struct paint_world * world);
int init_world(struct paint_world * world);
int logic_ok(struct paint_world * world);
int main(int argc, char ** argv);
int mark_block_strip(int index1, int stop, struct strip * a_strip,
  struct paint_world * world);
int mark_empty_square(int index1, int index2, struct paint_world * world);
int mark_empty_strip(int index1, int stop, struct strip * a_strip,
  struct paint_world * world);
int mark_empty_strip2(int index1, int stop, struct strip * a_strip,
  struct paint_world * world);
int mark_patches_strip(int index1, int stop, struct strip * a_strip,
  struct paint_world * world);
int mark_strip(int index1a, int index1b, int index2, char * save_strip,
  int size1, int * marked, struct strip * strips, int sign,
  struct paint_world * world);
int max(int int1, int int2);
int min(int int1, int int2);
int print_paint(struct paint_world * world);
int put_logic(int index, struct paint_world * world);
int put_square(int index1, int index2, char item, struct paint_world * world);
int read_arguments(int argc, char ** argv, struct paint_world * world);
int read_col_numbers(FILE * in_port, int * col_total,
  struct paint_world * world);
int read_problem(char * file_name, struct paint_world * world);
int read_row_numbers(FILE * in_port, int * row_total,
  struct paint_world * world);
int record_progress(char * tried, struct paint_world * world);
int shift_patches_left(int pat, int index2, int * shifted,
  struct strip * a_strip);
int shift_patches_right(int pat, int index2, int stop, int * shifted,
  struct strip * a_strip);
int solve_problem(struct paint_world * world);
int try_block(struct paint_world * world);
int try_corner_col(int row, int col, int i, int j,
  struct paint_world * world);
int try_corner_row(int row, int col, int i, int j,
  struct paint_world * world);
int try_corners(struct paint_world * world);
int try_edge_col(int col, int beside, struct paint_world * world);
int try_edge_row(int row, int beside, struct paint_world * world);
int try_edge_square_col(int col, int beside, int row, int * low, int high,
  struct paint_world * world);
int try_edge_square_row(int row, int beside, int col, int * low, int high,
  struct paint_world * world);
int try_edge_test_col(int col, int beside, int row, int size, int * is_empty,
  struct paint_world * world);
int try_edge_test_row(int row, int beside, int col, int size, int * is_empty,
  struct paint_world * world);
int try_edges(struct paint_world * world);
int try_empty_no_room(struct paint_world * world);
int try_empty_outside(struct paint_world * world);
int try_full_bounded(struct paint_world * world);
int try_guessing(struct paint_world * world);
int try_one(struct paint_world * world);
int try_one_strip(int index1, int stop, struct strip * a_strip,
  struct paint_world * world);

/*************************************************************************/

/* globals

The worlds array is declared because:
1. A copy of the paint_world world is needed in try_guessing for search.
2. Try_guessing is called recursively.
3. If a paint_world is declared in try_guessing as an automatic variable,
   it uses stack space. Since paint_world is a big structure, it uses
   a lot of space on the stack, and eventually the stack overflows
   if recursions goes deep. Global variables do not use stack space.

The deepest recursion has been observed to go is 52 levels deep, so
100 worlds is probably enough.

*/

struct paint_world worlds[100]; /* array of worlds for search */

/*************************************************************************/

/* define_functions

*/

/*************************************************************************/

/* add_patch

Returned Value: int (OK)

Called By:
  read_col_numbers
  read_row_numbers

Side Effects: This adds a patch of the given length to a_strip.

Notes:

The space for the patch is already allocated. The start and end of
the patch are set in init_patches.

A patch of length 0 is treated like every other patch.

*/

int add_patch(           /* ARGUMENTS                                 */
 struct strip * a_strip, /* the strip to which a patch is being added */
 int length)             /* the length of the patch being added       */
{
  const char * name SET_TO "add_patch";

  a_strip->patches[a_strip->number_patches].target SET_TO length;
  a_strip->patches[a_strip->number_patches].done SET_TO 0;
  a_strip->number_patches++;
  a_strip->total SET_TO (a_strip->total + length);
  a_strip->undone++;
  return OK;
}

/*************************************************************************/

/* balanced

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. See below

Called By:  try_guessing

Side Effects: none

Notes:

*/

int balanced(                /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "balanced";

  world->use_rows SET_TO 1;
  IFF(balanced_down(world));
  IFF(balanced_left(world));
  IFF(balanced_right(world));
  IFF(balanced_up(world));
  return OK;
}

/*************************************************************************/

/* balanced_down

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. See below

Called By:  balanced

Side Effects: none

Notes:

The following sort of configuration arose in solving alex_01.pro by search

                        1 1 1 1
                        3 5 5 5
         --------------------------
       1| . . .   . . .         . .|
       1| . . .   . . .         . .|
        | . X X . . . . . . . . . .|
        | . . . . . . . . X X X . .|
        | . . . . . . . . X X X . .|
        | . . . . . . . X X X X . .|
        | . . . . . . . X X X X . .|
        | . . . . . . . X X X X . .|

The 3 5 5 5 targets have been located, so the four 1's in the top row
of numbers must go somewhere on the first two lines of the grid. But
the row numbers allow for a total of only two squares to be filled on
first two lines. Thus, an impossible situation has been reached. There
is no way to complete the puzzle, but no row or column condition has
been violated. It is possible to complete any one of the columns. Only
after two of the 1's have been placed in columns will it become
impossible to complete the other two.

For rows, what this function does is, at each row (from the first to
the next-to-last), it calculates row_need, which is the number of
squares on or above the row that remain to be filled according to the
row numbers.  Then it calculates col_need, which is the number of
squares that remain to be filled in column patches that end on or
above the row. If B1 is greater than A1, it returns ERROR.

This does not need to go to the last row because that would cover the
whole puzzle, and we know the puzzle as a whole is balanced.

For the configuration shown above, the error will be detected when
row 2 is checked. The four column patches all start in row 1 and end
in row 2. In each of them, one square remains to be filled.

After doing the rows top-down, it does them bottom up similarly.
For columns, it does the analogous thing, first left to right, then
right to left.

A similar check could usefully be made for some subrectangles of the
puzzle, but it is difficult to identify the subrectangles for which
the check should be made, so this has not been attempted.

The point of this check, of course, is to cut short any search paths
that cannot succeed but whose impossibility is not otherwise detected.

In general, some column patches will cross a given row (or start on
the row). For these patches, it is not known whether the target is
across or below the row, so such patches are not used in determining
the minimum number of squares on or above the row that must be filled.

The function calculates the increments at each row rather than starting
from scratch at each row.

Note that the semantics of the row_XXX and col_XXX variables below
differ even though the names are similar.

*/

int balanced_down(           /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "balanced_down";
  int row;
  int col;
  int pat;
  struct patch * a_patch;
  int row_total; /* total in targets for rows up to row                     */
  int row_has;   /* num squares filled in rows up to row                    */
  int row_need;  /* num squares yet to be filled in rows up to row          */
  int col_total; /* total in targets for patches in cols ending up to row   */
  int col_has;   /* num squares filled for patches in cols ending up to row */
  int col_need;  /* num squares yet to be filled for patches in cols ...    */
  int patch_index[MAX_NUMBER_COLS];  /* max index of patch ending up to row */
  int n;

  row_total SET_TO 0;
  row_has SET_TO 0;
  col_total SET_TO 0;
  col_has SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    patch_index[col] SET_TO 0;
  for (row SET_TO 0; row < (world->number_rows - 1); row++)
    {
      row_total SET_TO (row_total + world->rows[row].total);
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  if (get_square(row, col, world) IS 'X')
	    row_has++;
	  if (patch_index[col] IS world->cols[col].number_patches)
	    continue;
	  pat SET_TO patch_index[col];
	  a_patch SET_TO &(world->cols[col].patches[pat]);
	  if (a_patch->end IS row)
	    {
	      col_total SET_TO (col_total + a_patch->target);
	      n SET_TO
		((pat IS 0) ? 0 : (1 + world->cols[col].patches[pat - 1].end));
	      for (; n <= row; n++)
		{
		  if (get_square(n, col, world) IS 'X')
		    col_has++;
		}
	      patch_index[col]++;
	    }
	}
      row_need SET_TO (row_total - row_has);
      col_need SET_TO (col_total - col_has);
      CHK((col_need > row_need), "puzzle not balanced down");
    }
  return OK;
}

/*************************************************************************/

/* balanced_left

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. See below

Called By:  balanced

Side Effects: none

Notes:

This does columns right to left.

See documentation of balanced_down.

*/

int balanced_left(          /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "balanced_left";
  int row;
  int col;
  int pat;
  struct patch * a_patch;
  int col_total; /* total in targets for cols, col or after                   */
  int col_has;   /* num squares filled in cols, col or after                  */
  int col_need;  /* num squares yet to be filled in cols, col or after        */
  int row_total; /* total in trgets for patches in rows starting col or after */
  int row_has;   /* num sqrs filled for patches in rows starting col or after */
  int row_need;  /* num sqrs yet to be filled for patches in rows starting... */
  int patch_index[MAX_NUMBER_ROWS]; /* min ndx of patch starting col or after */
  int n;

  col_total SET_TO 0;
  col_has SET_TO 0;
  row_total SET_TO 0;
  row_has SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    patch_index[row] SET_TO (world->rows[row].number_patches - 1);
  for (col SET_TO (world->number_cols - 1); col > 0; col--)
    {
      col_total SET_TO (col_total + world->cols[col].total);
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  if (get_square(row, col, world) IS 'X')
	    col_has++;
	  if (patch_index[row] IS -1)
	    continue;
	  pat SET_TO patch_index[row];
	  a_patch SET_TO &(world->rows[row].patches[pat]);
	  if (a_patch->start IS col)
	    {
	      row_total SET_TO (row_total + a_patch->target);
	      n SET_TO
		((pat IS
		  (world->rows[row].number_patches - 1)) ? world->number_cols :
		 (world->rows[row].patches[pat + 1].start - 1));
	      for (; n >= col; n--)
		{
		  if (get_square(row, n, world) IS 'X')
		    row_has++;
		}
	      patch_index[row]--;
	    }
	}
      col_need SET_TO (col_total - col_has);
      row_need SET_TO (row_total - row_has);
      CHK((row_need > col_need), "puzzle not balanced left");
    }
  return OK;
}

/*************************************************************************/

/* balanced_right

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. See below

Called By:  balanced

Side Effects: none

Notes:

This does columns right to left.

See documentation of balanced_down.

*/

int balanced_right(          /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "balanced_right";
  int row;
  int col;
  int pat;
  struct patch * a_patch;
  int col_total; /* total in targets for cols up to col                     */
  int col_has;   /* num squares filled in cols up to col                    */
  int col_need;  /* num squares yet to be filled in cols up to col          */
  int row_total; /* total in targets for patches in rows ending up to col   */
  int row_has;   /* num squares filled for patches in rows ending up to col */
  int row_need;  /* num squares yet to be filled for patches in rows ...    */
  int patch_index[MAX_NUMBER_ROWS];  /* max index of patch ending up to col */
  int n;

  col_total SET_TO 0;
  col_has SET_TO 0;
  row_total SET_TO 0;
  row_has SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    patch_index[row] SET_TO 0;
  for (col SET_TO 0; col < (world->number_cols - 1); col++)
    {
      col_total SET_TO (col_total + world->cols[col].total);
      for (row SET_TO 0; row < world->number_rows; row++)
	{
	  if (get_square(row, col, world) IS 'X')
	    col_has++;
	  if (patch_index[row] IS world->rows[row].number_patches)
	    continue;
	  pat SET_TO patch_index[row];
	  a_patch SET_TO &(world->rows[row].patches[pat]);
	  if (a_patch->end IS col)
	    {
	      row_total SET_TO (row_total + a_patch->target);
	      n SET_TO
		((pat IS 0) ? 0 : (1 + world->rows[row].patches[pat - 1].end));
	      for (; n <= col; n++)
		{
		  if (get_square(row, n, world) IS 'X')
		    row_has++;
		}
	      patch_index[row]++;
	    }
	}
      col_need SET_TO (col_total - col_has);
      row_need SET_TO (row_total - row_has);
      CHK((row_need > col_need), "puzzle not balanced right");
    }
  return OK;
}

/*************************************************************************/

/* balanced_up

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. See below

Called By:  balanced

Side Effects: none

Notes:

See documentation of balanced_down. This is the same, except working bottom
up instead of top down.

*/

int balanced_up(             /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "balanced_up";
  int row;
  int col;
  int pat;
  struct patch * a_patch;
  int row_total; /* total in targets for rows, row or after                   */
  int row_has;   /* num squares filled in rows, row or after                  */
  int row_need;  /* num squares yet to be filled in rows, row or after        */
  int col_total; /* total in trgets for patches in cols starting row or after */
  int col_has;   /* num sqrs filled for patches in cols starting row or after */
  int col_need;  /* num sqrs yet to be filled for patches in cols starting... */
  int patch_index[MAX_NUMBER_COLS]; /* min ndx of patch starting row or after */
  int n;

  row_total SET_TO 0;
  row_has SET_TO 0;
  col_total SET_TO 0;
  col_has SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    patch_index[col] SET_TO (world->cols[col].number_patches - 1);
  for (row SET_TO (world->number_rows - 1); row > 0; row--)
    {
      row_total SET_TO (row_total + world->rows[row].total);
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  if (get_square(row, col, world) IS 'X')
	    row_has++;
	  if (patch_index[col] IS -1)
	    continue;
	  pat SET_TO patch_index[col];
	  a_patch SET_TO &(world->cols[col].patches[pat]);
	  if (a_patch->start IS row)
	    {
	      col_total SET_TO (col_total + a_patch->target);
	      n SET_TO
		((pat IS
		  (world->cols[col].number_patches - 1)) ? world->number_rows :
		 (world->cols[col].patches[pat + 1].start - 1));
	      for (; n >= row; n--)
		{
		  if (get_square(n, col, world) IS 'X')
		    col_has++;
		}
	      patch_index[col]--;
	    }
	}
      row_need SET_TO (row_total - row_has);
      col_need SET_TO (col_total - col_has);
      CHK((col_need > row_need), "puzzle not balanced up");
    }
  return OK;
}

/*************************************************************************/

/* copy_patch

Returned Value: int (OK)

Called By:  copy_strip

Side Effects: This copies the "from" patch onto the "to" patch.

*/

int copy_patch(        /* ARGUMENTS          */
 struct patch * to,    /* patch to copy into */
 struct patch * from)  /* patch to copy from */
{
  const char * name SET_TO "copy_patch";

  to->done SET_TO from->done;
  to->end SET_TO from->end;
  to->start SET_TO from->start;
  to->target SET_TO from->target;
  return OK;
}

/*************************************************************************/

/* copy_strip

Returned Value: int 
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. copy_patch returns ERROR.

Called By:
  copy_world
  try_corner_col
  try_corner_row
  try_edge_test_col
  try_edge_test_row
  try_one_strip

Side Effects: This copies the "from" strip onto the "to" strip.

Notes:

The world argument is needed because IFF uses it.

*/

int copy_strip(              /* ARGUMENTS          */
 struct strip * to,          /* strip to copy into */
 struct strip * from,        /* strip to copy from */
 struct paint_world * world) /* puzzle model       */
{
  const char * name SET_TO "copy_strip";
  int n;

  to->number_patches SET_TO from->number_patches;
  for (n SET_TO 0; n < to->number_patches; n++)
    {
      IFF(copy_patch(&(to->patches[n]), &(from->patches[n])));
    }
  to->total SET_TO from->total;
  to->undone SET_TO from->undone;
  return OK;
}

/*************************************************************************/

/* copy_world

Returned Value: int 
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. copy_strip does not return OK.

Called By:  try_guessing

Side Effects: This copies the "world" world onto the "to" world.

Notes:

The world is called "world" rather than "from" because IFF needs there
to be a "world" argument.

The last character of each logic line is copied explicitly so that
it will be possible to detect long lines. See documentation of logic_ok.

*/

int copy_world(              /* ARGUMENTS          */
 struct paint_world * to,    /* world to copy into */
 struct paint_world * world) /* world to copy from */
{
  const char * name SET_TO "copy_world";
  int row;
  int col;
  int n;
  int last;

  last SET_TO TEXT_SIZE;
  last SET_TO (last - 1);
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      IFF(copy_strip(&(to->cols[col]), &(world->cols[col]), world));
    }
  for (n SET_TO 0; n < world->number_in_cols; n++)
    {
      to->in_cols[n] SET_TO world->in_cols[n];
    }
  to->col_lines SET_TO world->col_lines;
  for (n SET_TO 0; n < world->col_lines; n++)
    {
      strncpy(to->col_numbers[n], world->col_numbers[n], TEXT_SIZE);
    }
  to->find_all SET_TO world->find_all;
  for (n SET_TO 0; n < world->logic_line; n++)
    {
      strncpy(to->logic[n], world->logic[n], TEXT_SIZE);
      to->logic[n][last] SET_TO world->logic[n][last];
    }
  for (n SET_TO world->logic_line; n < MAX_LOGIC; n++)
    to->logic[n][last] SET_TO 0;
  to->logic_line SET_TO world->logic_line;
  to->number_cols SET_TO world->number_cols;
  to->number_in_cols SET_TO world->number_in_cols;
  to->number_rows SET_TO world->number_rows;
  to->number_in_rows SET_TO world->number_in_rows;
  to->number_spaces SET_TO world->number_spaces;
  to->print_all SET_TO world->print_all;
  to->progress_puzzle SET_TO world->progress_puzzle;
  to->progress_strip SET_TO world->progress_strip;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      IFF(copy_strip(&(to->rows[row]), &(world->rows[row]), world));
    }
  for (n SET_TO 0; n < world->number_in_rows; n++)
    {
      to->in_rows[n] SET_TO world->in_rows[n];
    }
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      strncpy(to->row_numbers[row], world->row_numbers[row], TEXT_SIZE);
    }
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  to->squares[row][col] SET_TO world->squares[row][col];
	}
    }
  to->undone SET_TO world->undone;
  to->use_rows SET_TO world->use_rows;
  to->verbose SET_TO world->verbose;
  return OK;
}

/*************************************************************************/

/* enx_square

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The square is neither blank nor already marked with an X: "bad square".

Called By:
  extend_block
  init_patches_strip
  mark_patches_strip
  try_one_strip

Side Effects: If the square at [index1, index2] (for world->use_rows
or at [index2, index1] for not world->use_rows) is blank, this puts an
X in it and sets world->progress_puzzle and world->progress_strip to 1.

Might add an in-bounds check on index1 and index2.

*/

int enx_square(              /* ARGUMENTS              */
 int index1,                 /* first index of square  */
 int index2,                 /* second index of square */
 struct paint_world * world) /* puzzle model           */
{
  const char * name SET_TO "enx_square";

  if (world->use_rows)
    {
      if (world->squares[index1][index2] IS ' ')
	{
	  world->squares[index1][index2] SET_TO 'X';
	  world->progress_puzzle SET_TO 1;
	  world->progress_strip SET_TO 1;
	}
      else
	{
	  if (world->squares[index1][index2] ISNT 'X')
	    CHK((world->squares[index1][index2] ISNT 'X'), "bad square");
	}
    }
  else
    {
      if (world->squares[index2][index1] IS ' ')
	{
	  world->squares[index2][index1] SET_TO 'X';
	  world->progress_puzzle SET_TO 1;
	  world->progress_strip SET_TO 1;
	}
      else
	{
	  if (world->squares[index2][index1] ISNT 'X')
	    CHK((world->squares[index2][index1] ISNT 'X'), "bad square");
	}
    }
  return OK;
}

/*************************************************************************/

/* explain_paint

Returned Value: int (OK)

Called By:  solve_problem

Side Effects: This prints the explanation of the steps in solving the
puzzle, which has been accumulated in the world->logic array of strings.

*/

int explain_paint(           /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "explain_paint";
  int line;           /* index of logic line */
  int last;

  last SET_TO TEXT_SIZE;
  last SET_TO (last - 1);
  printf("\nEXPLANATION\n-----------\n");
  for (line SET_TO 0; line < world->logic_line; line++)
    {
      printf("%s\n", world->logic[line]);
      for (; (world->logic[line][last] ISNT 0); line++);
    }
  return OK;
}

/*************************************************************************/

/* extend_block

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. enx_square returns ERROR.

Called By:  mark_block_strip

Side Effects: If there is an empty square near one end of a block, this
extends the other end of the block. Both ends of the block are checked.

*/

int extend_block(            /* ARGUMENTS                             */
 int index1,                 /* index of strip                        */
 int index2,                 /* index at right end of contained block */
 int min_target,             /* minimum length of containing block    */
 int length,                 /* length of contained block             */
 struct paint_world * world) /* puzzle model                          */
{
  const char * name SET_TO "extend_block";
  int stop;
  int n;
  int k;

  stop SET_TO (min_target - length);
  for (n SET_TO 1; n <= stop; n++)
    {
      if (get_square(index1, (index2 + n), world) IS '.')
	{
	  for (k SET_TO 0; k <= (stop - n); k++)
	    IFF(enx_square(index1, (index2 - (length + k)), world));
	  break;
	}
    }
  for (n SET_TO 1; n <= stop; n++)
    {
      if (get_square(index1, ((index2 + 1) - (length + n)), world) IS '.')
	{
	  for (k SET_TO 0; k <= (stop - n); k++)
	    IFF(enx_square(index1, (index2 + k + 1), world));
	  break;
	}
    }
  return OK;
}

/*************************************************************************/

/* find_best_patch

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. There are no undone patches: "bug no target found".

Called By:  try_guessing

Side Effects: The values of strip_index, is_row, and patch_index are
set to those of the strip and patch with the best target.

Notes:

The best patch is the one that has the fewest choices for where to put
its block. If two patches have the same number of choices, the better
one is the one with the largest target.

The criterion for the best patch can easily be changed to be best is
largest target, and if equal length, best has fewest choices. In the
two places where the first two lines below appear, insert the second
two, instead.

         if ((spots < *many) OR
             ((spots IS *many) AND (a_patch->target > target)))

         if ((a_patch->target > target) OR
             ((a_patch->target IS target) AND (spots < *many)))

Timing tests done on the two does not show one to be clearly superior.
Timing tests on 7/19/2, searching for all solutions of the fuj_20 and
fuj_07 problems, gave the following results in seconds:

          fuj_20       fuj_07
current     86           33
alternate   28           73

Both versions took only a second or so on each of the other fuj problems
requiring search.

If a patch is not done, there should be at least two places the block in
the patch can go (if there were only one, it would have been found).

*/

int find_best_patch(         /* ARGUMENTS                                */
 int * strip_index,          /* index of strip with best patch, set here */
 int * is_row,               /* 1=strip is row, 0=strip is col, set here */
 int * patch_index,          /* index in strip of best patch, set here   */
 int * many,                 /* number of choices for best patch         */
 struct paint_world * world) /* puzzle model                             */
{
  const char * name SET_TO "find_best_patch";
  int row;
  int col;
  int pat;                /* index in strip of current patch    */
  int stop;               /* number of patches in strip         */
  int halt;               /* index of last possible start       */
  int spots;              /* number of choices of current patch */
  int start;
  int n;
  int target;             /* target size for current best patch */
  struct strip * a_strip;
  struct patch * a_patch;

  *many SET_TO max(world->number_rows, world->number_cols);
  target SET_TO 0;
  *strip_index SET_TO -1;
  *is_row SET_TO 1;
  world->use_rows SET_TO 1;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      a_strip SET_TO &(world->rows[row]);
      if (a_strip->undone IS 0)
	continue;
      stop SET_TO a_strip->number_patches;
      for (pat SET_TO 0; pat < stop; pat++)
	{
	  a_patch SET_TO &(a_strip->patches[pat]);
	  if (a_patch->done)
	    continue;
	  halt SET_TO ((a_patch->end + 2) - a_patch->target);
	  spots SET_TO (halt - a_patch->start);
	  for (start SET_TO a_patch->start; start < halt; start++)
	    {
	      for (n SET_TO 0; n < a_patch->target; n++)
		if (get_square(row, (start + n), world) IS '.')
		  break;
	      if ((n ISNT a_patch->target) OR
		  (get_square(row, (start - 1), world) IS 'X') OR
		  (get_square(row, (start + n), world) IS 'X'))
		spots--;
	    }
	  if (spots < 2)
	    CHK((spots < 2), "fewer than two possible locations");
	  if ((spots < *many) OR
	      ((spots IS *many) AND (a_patch->target > target)))
	    {
	      *many SET_TO spots;
	      *strip_index SET_TO row;
	      *patch_index SET_TO pat;
	      target SET_TO a_patch->target;
	    }
	}
    }
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      a_strip SET_TO &(world->cols[col]);
      if (a_strip->undone IS 0)
	continue;
      stop SET_TO a_strip->number_patches;
      for (pat SET_TO 0; pat < stop; pat++)
	{
	  a_patch SET_TO &(a_strip->patches[pat]);
	  if (a_patch->done)
	    continue;
	  halt SET_TO ((a_patch->end + 2) - a_patch->target);
	  spots SET_TO (halt - a_patch->start);
	  for (start SET_TO a_patch->start; start < halt; start++)
	    {
	      for (n SET_TO 0; n < a_patch->target; n++)
		if (get_square((start + n), col, world) IS '.')
		  break;
	      if ((n ISNT a_patch->target) OR
		  (get_square((start - 1), col, world) IS 'X') OR
		  (get_square((start + n), col, world) IS 'X'))
		spots--;
	    }
	  if (spots < 2)
	    CHK((spots < 2), "fewer than two possible locations");
	  if ((spots < *many) OR
	      ((spots IS *many) AND (a_patch->target > target)))
	    {
	      *many SET_TO spots;
	      target SET_TO a_patch->target;
	      *strip_index SET_TO col;
	      *patch_index SET_TO pat;
	      *is_row SET_TO 0;
	    }
	}
    }
  CHK((*strip_index IS -1), "bug no target found");
  return OK;
}

/*************************************************************************/

/* find_first_undone_target

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. no undone patch is found: "no undone target"

Called By:
  mark_strip
  try_corner_col
  try_corner_row

Side Effects: This sets the value of target to the length of the first
undone patch in a_strip.

*/

int find_first_undone_target( /* ARGUMENTS                               */
 struct strip * a_strip,      /* the strip being checked                 */
 int * target,                /* length of first undone target, set here */
 struct paint_world * world)  /* puzzle model                            */
{
  const char * name SET_TO "find_first_undone_target";
  int pat;

  for (pat SET_TO 0; pat < a_strip->number_patches; pat++)
    {
      if (a_strip->patches[pat].done);
      else
	{
	  *target SET_TO a_strip->patches[pat].target;
	  break;
	}
    }
  CHK((pat IS a_strip->number_patches), "no undone target");
  return OK;
}

/*************************************************************************/

/* find_last_undone_target

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. no undone patch is found: "no_undone_target"

Called By:
  mark_strip
  try_corner_col
  try_corner_row

Side Effects: This sets the value of target to the length of the last
undone patch in a_strip.

*/

int find_last_undone_target( /* ARGUMENTS                              */
 struct strip * a_strip,     /* the strip being checked                */
 int * target,               /* length of last undone target, set here */
 struct paint_world * world) /* puzzle model                           */
{
  const char * name SET_TO "find_last_undone_target";
  int pat;

  for (pat SET_TO a_strip->number_patches; pat > 0; pat--)
    {
      if (a_strip->patches[pat - 1].done);
      else
	{
	  *target SET_TO a_strip->patches[pat - 1].target;
	  break;
	}
    }
  CHK((pat IS 0), "no undone target");
  return OK;
}

/*************************************************************************/

/* find_max_ends

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. An X is found in the strip after the last patch of the strip:
     "puzzle has no answer".
  2. shift_patches_left returns ERROR.

Called By:  mark_patches_strip

Side Effects: This finds the largest possible values for the ends of
the patches of the given strip, consistent with what is known about
which squares are full and which empty.

Notes:

This works like a mirror image of find_min_starts. It is simpler than
find_min_starts, however, because it is called only when it is expected
to succeed. Thus the "found" argument of find_min_starts is not used
here.

*/

int find_max_ends(            /* ARGUMENTS                              */
 int index1,                  /* the index of the strip being processed */
 int stop,                    /* length of a_strip                      */
 struct strip * a_strip,      /* the strip being processed              */
 struct paint_world * world)  /* puzzle model                           */
{
  const char * name SET_TO "find_max_ends";
  int index2;
  int pat;
  int shifted;
  struct patch * a_patch;

  pat SET_TO 0;
  for (index2 SET_TO 0; ((index2 < stop) AND (pat < a_strip->number_patches)); )
    {
      a_patch SET_TO &(a_strip->patches[pat]);
      if (a_patch->end < index2)
	{
	  pat++;
	}
      else if (get_square(index1, index2, world) IS 'X')
	{
	  if ((a_patch->end - a_patch->target + 1) > index2)
	    {
	      IFF(shift_patches_left
		  (pat, (index2 + a_patch->target - 1), &shifted, a_strip));
	      CHK((NOT shifted), "cannot shift patches left");
	      pat SET_TO 0;
	      index2 SET_TO 0;
	    }
	  else
	    index2++;
	}
      else if (get_square(index1, index2, world) IS '.')
	{
	  if ((a_patch->end - a_patch->target + 1) <= index2)
	    {
	      IFF(shift_patches_left(pat, (index2 - 1), &shifted, a_strip));
	      CHK((NOT shifted), "cannot shift patches left");
	      pat SET_TO 0;
	      index2 SET_TO 0;
	    }
	  else
	    index2++;
	}
      else
	index2++;
    }
  for (; index2 < stop; index2++)
    CHK((get_square(index1, index2, world) IS 'X'), "puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* find_min_starts

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. shift_patches_right returns ERROR.

Called By:
  mark_patches_strip
  try_corner_col
  try_corner_row
  try_edge_test_col
  try_edge_test_row
  try_one_strip

Side Effects: This finds the smallest possible values for the starts
of the patches of the given strip, consistent with what is known about
which squares are full and which empty. The "start" for each patch may
be reset.

If there is no possible covering, this sets the found argument to 0.
Otherwise, it sets found to 1.

Notes:

Each strip contains an ordered set of patches. Each patch has a target
which is the number of squares in the block that is contained by the
patch. In their leftmost position in their patches, the blocks of the
patches must cover any full squares and must not cover any empty
squares. One patch is "before" another if its index is the strip
is smaller.

To understand this algorithm, imagine that physical patches are lying
on a physical strip and each patch has its completed block inside and
mentally perform the shifting process.  Or make a physical model and
manipulate it.

This is a constructive algorithm, not a search algorithm. All it
does is move patches to the right as necessary.

The conceptual approach is to start with imagined completed patches
moved as far left as was previously allowed, and move them right
if necessary (to cover a full square or uncover an empty square)
starting with the rightmost patch. If a patch is moved to the right,
it may be necessary to move the patches to its right farther to the
right. Whenever a patch is moved, the checking starts all over again
with the rightmost square and patch in the strip.

This works its way square by square from the right end of the strip to
the left. It focuses on one square (using index2) and one patch (using
pat) at a time. The following four steps are repeated until either all
squares inside the strip have been checked or the current square is to
the left of the leftmost patch. In the latter case, a check is made
that none of the squares to the left of the current square is full;
if one is full, found is set to 0.

1. If the start of current patch is to the right of the current square
   make the first patch before the current patch be the current patch.

2. Otherwise, if the current square is full:

  A. If the current square is not covered by the block in the current patch
  in its leftmost position, shift the start of current patch right just far
  enough that the block must cover the square (and move any patches after
  the current patch to the right if necessary so that the blocks in the
  patches will still be separated when all blocks are pushed as far left
  as possible). Then set the current square and the current patch to be the
  last square and the last patch (i.e. restart).

  B. Else set the current square to be the next square to the left.

3. Otherwise, if the current square is empty:

  A. If the current square is covered by the block of the current patch
  in its leftmost position, shift the start of current patch right just
  far enough to uncover the square (and move any patches to the right of
  the current patch to the right if necessary, as described in 2A).
  Then set the current square and the current patch to be the last square
  and the last patch (i.e. restart).

  B. Else set the current square to be the next square to the left.

4. Otherwise (the current square is blank), make the next square to the
  left be the current square.

After patches are shifted right, it would be feasible to continue
checking squares at a square determined by the rightmost moved patch,
but it is simpler to start over at the end, and that is what is
implemented.

Notes:

The (pat > -1) test near the beginning makes the checking stop when
the first patch has been checked and pat has been decremented to -1.

shifted is initially set to 1 to handle the case of a strip in which
no shifting is needed.

*/

int find_min_starts(         /* ARGUMENTS                                 */
 int index1,                 /* the index of the strip being processed    */
 int stop,                   /* length of a_strip                         */
 struct strip * a_strip,     /* the strip being processed                 */
 int * found,                /* set here to 1 if min_starts found, else 0 */
 struct paint_world * world) /* puzzle model                              */
{
  const char * name SET_TO "find_min_starts";
  int index2;
  int pat;
  struct patch * a_patch;
  int shifted;

  pat SET_TO (a_strip->number_patches - 1);
  shifted SET_TO 1;
  for (index2 SET_TO (stop - 1); ((index2 > -1) AND (pat > -1)); )
    {
      a_patch SET_TO &(a_strip->patches[pat]);
      if (a_patch->start > index2)
	{
	  pat--;
	}
      else if (get_square(index1, index2, world) IS 'X')
	{
	  if ((a_patch->start + a_patch->target - 1) < index2)
	    {
	      IFF(shift_patches_right(pat, (index2 + 1 - a_patch->target),
				      stop, &shifted, a_strip));
	      if (NOT shifted)
		break;
	      pat SET_TO (a_strip->number_patches - 1);
	      index2 SET_TO (stop - 1);
	    }
	  else
	    index2--;
	}
      else if (get_square(index1, index2, world) IS '.')
	{
	  if ((a_patch->start + a_patch->target - 1) >= index2)
	    {
	      IFF(shift_patches_right
		  (pat, (index2 + 1), stop, &shifted, a_strip));
	      if (NOT shifted)
		break;
	      pat SET_TO (a_strip->number_patches - 1);
	      index2 SET_TO (stop - 1);
	    }
	  else
	    index2--;
	}
      else
	index2--;
    }
  if (shifted)
    {
      *found SET_TO 1;
      for (; index2 > -1; index2--)
	{
	  if (get_square(index1, index2, world) IS 'X')
	    {
	      *found SET_TO 0;
	      break;
	    }
	}
    }
  else
    *found SET_TO 0;
  return OK;
}

/*************************************************************************/

/* find_min_target

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The given square is not covered by any patch:
     "no min target square lies outside all patches"

Called By:
  try_edge_square_col
  try_edge_square_row

Side Effects: The size argument is set to the minimum of the targets
that might contain the square of a_strip with the given index.
If the square is outside all patches, ERROR is returned.

*/

int find_min_target(         /* ARGUMENTS                     */
 int index,                  /* index of square in a_strip    */
 struct strip * a_strip,     /* the strip in being processed  */
 int * size,                 /* minimum target size, set here */
 struct paint_world * world) /* puzzle model                  */
{
  const char * name SET_TO "find_min_target";
  int pat;
  struct patch * a_patch;

  *size SET_TO (world->number_rows + world->number_cols);
  for (pat SET_TO 0; pat < a_strip->number_patches; pat++)
    {
      a_patch SET_TO &(a_strip->patches[pat]);
      if ((a_patch->start <= index) AND (a_patch->end >= index))
	{
	  if (a_patch->target < *size)
	    *size SET_TO a_patch->target;
	}
    }
  CHK((*size IS (world->number_rows + world->number_cols)),
      "no min target square lies outside all patches");
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
 11. There are more than MAX_NUMBER_ROWS rows: "Too many rows".
 12. There are more than MAX_NUMBER_COLS columns: "Too many columns".


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

The part of the program that looks for the first line of dashes messes
up in objectcenter if there are two blank lines just below the top of
the file.

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
  CHB((world->number_cols > MAX_NUMBER_COLS), "Too many columns");
  for (world->number_rows SET_TO 0; ; world->number_rows++)
    {
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends before second line of dashes");
      length SET_TO strlen(buffer);
      if (length IS (number_spaces + number_dashes + 2))
	break;
      CHB((world->number_rows IS MAX_NUMBER_ROWS), "Too many rows");
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
  world->undone SET_TO (world->number_rows + world->number_cols);
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
4. 1 to 359 rows each consisting of (i) B characters in sets of three that
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
2. Look for a blank line.
3. Read the top rows of numbers.
4. Read the numbers to the left of the puzzle.
5. Print the puzzle.

*/

int get_paint2(              /* ARGUMENTS            */
 char * file_name,           /* name of file to read */
 struct paint_world * world) /* puzzle model         */
{
  const char * name SET_TO "get_paint2";
  FILE * in_port;
  char dashes[TEXT_SIZE];
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
      if (buffer[0] IS '\n')
	break;
    }
  IFF(read_col_numbers(in_port, &col_total, world));
  IFF(read_row_numbers(in_port, &row_total, world));
  fclose(in_port);
  CHB((row_total ISNT col_total), "row total differs from column total");
  strcpy(world->logic[world->logic_line++], "read problem");
  if (world->print_all)
    {
      IFF(print_paint(world));
      printf("%s\n\n", world->logic[world->logic_line - 1]);
    }
  return OK;
}

/*************************************************************************/

/* get_square

Returned Value: char

This returns the actual character of world->squares (in the [index1,
index2] position if world->use_rows is 1 or in the [index2, index1]
position if world->use_rows is 0) if the square is inside the defined
area, and returns empty ('.') otherwise.

Called By:
  balanced_down
  balanced_left
  balanced_right
  balanced_up
  extend_block
  find_best_patch
  find_max_ends
  find_min_starts
  init_patches_strip
  mark_block_strip
  mark_empty_square
  mark_empty_strip2
  mark_patches_strip
  mark_strip
  try_corner_col
  try_corner_row
  try_corners
  try_edge_col
  try_edge_row
  try_guessing
  try_one_strip

Side Effects: none

Notes:

This is an array referencer for squares that pads the in-bounds
squares with a virtually infinite number of empty squares. Doing this
allows the functions that use it to pretend the array consists of
all inside squares (none along the edges), which simplifies things
greatly.

*/

char get_square(             /* ARGUMENTS              */
 int index1,                 /* first index of square  */
 int index2,                 /* second index of square */
 struct paint_world * world) /* puzzle model           */
{
  const char * name SET_TO "get_square";
  char result;

  result SET_TO
    (((index1 < 0) OR (index2 < 0)) ? '.' :
     (world->use_rows AND
      (index1 < world->number_rows) AND
      (index2 < world->number_cols)) ? world->squares[index1][index2] :
     ((NOT (world->use_rows)) AND
      (index1 < world->number_cols) AND
      (index2 < world->number_rows)) ? world->squares[index2][index1] : '.');
  return result;
}

/*************************************************************************/

/* init_patches

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. init_patches_strip returns ERROR.
  2. record_progress returns ERROR.

Called By:  init_solution

Side Effects: The start and end of the patches in each strip (row or
column) of the puzzle are set, and squares are filled in if possible.
Progress is reported if there is any.

*/

int init_patches(            /* ARGUMENTS     */
 struct paint_world * world) /* puzzle model  */
{
  const char * name SET_TO "init_patches";
  int row;
  int col;

  world->progress_puzzle SET_TO 0;
  world->number_in_rows SET_TO 0;
  world->use_rows SET_TO 1;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      IFF(init_patches_strip
	  (row, world->number_cols, &(world->rows[row]), world));
    }
  world->number_in_cols SET_TO 0;
  world->use_rows SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      IFF(init_patches_strip
	  (col, world->number_rows, &(world->cols[col]), world));
    }
  IFF(record_progress("mark first surely full squares and empties in ", world));
  world->progress_puzzle SET_TO 0;
  return OK;
}

/*************************************************************************/

/* init_patches_strip

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. enx_square returns ERROR.
  2. mark_empty_square returns ERROR.
  3. put_logic returns ERROR.
  4. The length of a patch is less than one in a_strip that either has more
     than one patch or has one patch of non-zero length: "bad patch size".

Called By:  init_patches

Side Effects: This looks at each patch in a_strip and marks the
smallest possible start and largest possible end of the patch by
allowing only enough room for the preceding and following targets. If
the patch must have some filled squares, they are filled. It also
records progress.

Notes:

A strip with one patch of length zero is OK.
If there is more than one patch, none of the patches may be length zero.

*/

int init_patches_strip(      /* ARGUMENTS                           */
 int index1,                 /* index of a_strip in rows or columns */
 int stop,                   /* length of a_strip                   */
 struct strip * a_strip,     /* the strip being processed           */
 struct paint_world * world) /* puzzle model                        */
{
  const char * name SET_TO "init_patches_strip";
  int index2;
  int place;            /* place marker */
  int pat;
  struct patch * patchn;

  world->progress_strip SET_TO 0;
  if ((a_strip->number_patches IS 1) AND (a_strip->patches[0].target IS 0))
    {
      a_strip->undone SET_TO 0;
      a_strip->patches[0].done SET_TO 1;
      a_strip->patches[0].start SET_TO 0;
      a_strip->patches[0].end SET_TO 0;
      for (index2 SET_TO 0; index2 < stop; index2++)
	{
	  IFF(mark_empty_square(index1, index2, world));
	}
    }
  else
    {
      place SET_TO 0;
      for (pat SET_TO 0; pat < a_strip->number_patches; pat++)
	{
	  CHB((a_strip->patches[pat].target < 1), "bad patch size");
	  a_strip->patches[pat].start SET_TO place;
	  place SET_TO (place + a_strip->patches[pat].target + 1);
	}
      place SET_TO (stop - 1);
      for (pat SET_TO (a_strip->number_patches - 1); pat > -1; pat--)
	{
	  patchn SET_TO &(a_strip->patches[pat]);
	  patchn->end SET_TO place;
	  for (index2 SET_TO (patchn->end - patchn->target + 1);
	       index2 < (patchn->start + patchn->target);
	       index2++)
	    {
	      if (get_square(index1, index2, world) ISNT 'X')
		IFF(enx_square(index1, index2, world));
	    }
	  if (((patchn->end + 1) - patchn->start) IS patchn->target)
	    {
	      patchn->done SET_TO 1;
	      a_strip->undone--;
	    }
	  place SET_TO (place - patchn->target - 1);
	}
    }
  if (a_strip->undone IS 0)
    {
      world->undone--;
      for (index2 SET_TO 0; index2 < stop; index2++)
	{
	  if (get_square(index1, index2, world) IS ' ')
	    IFF(mark_empty_square(index1, index2, world));
	}
    }
  if (world->progress_strip)
    IFF(put_logic(index1, world));
  return OK;
}

/*************************************************************************/

/* init_solution

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. init_patches returns ERROR.

Called By:  main

Side Effects:  See documentation of init_patches.

Notes:

To initialize the solution, only init_patches needs to be called.

*/

int init_solution(           /* ARGUMENTS     */
 struct paint_world * world) /* puzzle model  */
{
  const char * name SET_TO "init_solution";

  IFF(init_patches(world));
  return OK;
}

/*************************************************************************/

/* init_world

Returned Value: int (OK)

Called By:  main

Side Effects: The puzzle model is initialized.

Notes:

*/

int init_world(              /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "init_world";
  int row;
  int col;
  int line;
  int n;
  int m;

  for (col SET_TO 0; col < MAX_NUMBER_COLS; col++)
    {
      world->cols[col].total SET_TO 0;
      world->cols[col].undone SET_TO 0;
      world->cols[col].number_patches SET_TO 0;
      for (n SET_TO 0; n < MAX_STRIP_BLOCKS; n++)
	{
	  world->cols[col].patches[n].done SET_TO 0;
	  world->cols[col].patches[n].end SET_TO 0;
	  world->cols[col].patches[n].start SET_TO 0;
	  world->cols[col].patches[n].target SET_TO 0;
	}
      world->in_cols[col] SET_TO 0;
    }
  world->col_lines SET_TO 0;
  for (n SET_TO 0; n < MAX_STRIP_BLOCKS; n++)
    for (m SET_TO 0; m < TEXT_SIZE; m++)
      world->col_numbers[n][m] SET_TO 0;
  world->depth SET_TO 0;
  world->find_all SET_TO 0;
  for (n SET_TO 0; n < MAX_LOGIC; n++)
    for (m SET_TO 0; m < TEXT_SIZE; m++)
      world->logic[n][m] SET_TO 0;
  world->logic_line SET_TO 0;
  world->number_cols SET_TO 0;
  world->number_in_cols SET_TO 0;
  world->number_rows SET_TO 0;
  world->number_in_rows SET_TO 0;
  world->number_spaces SET_TO 0;
  world->print_all SET_TO 0;
  world->progress_puzzle SET_TO 0;
  world->progress_strip SET_TO 0;
  for (row SET_TO 0; row < MAX_NUMBER_ROWS; row++)
    {
      world->rows[row].total SET_TO 0;
      world->rows[row].undone SET_TO 0;
      world->rows[row].number_patches SET_TO 0;
      for (n SET_TO 0; n < MAX_STRIP_BLOCKS; n++)
	{
	  world->rows[row].patches[n].done SET_TO 0;
	  world->rows[row].patches[n].end SET_TO 0;
	  world->rows[row].patches[n].start SET_TO 0;
	  world->rows[row].patches[n].target SET_TO 0;
	}
      world->in_rows[row] SET_TO 0;
      for (m SET_TO 0; m < TEXT_SIZE; m++)
	{
	  world->row_numbers[row][m] SET_TO 0;
	}
      for (col SET_TO 0; col < MAX_NUMBER_COLS; col++)
	{
	  world->squares[row][col] SET_TO ' ';
	}
    }
  world->space SET_TO 1;
  world->undone SET_TO 0;
  world->use_rows SET_TO 1;
  world->verbose SET_TO 0;
  return OK;
}

/*************************************************************************/

/* logic_ok

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. More than (MAX_LOGIC - 10) logic lines are used: "too many logic lines".

Called By:  record_progress

Side Effects: world->logic_line may be incremented

Notes:

The "stop" and "last" variables are needed because gen_gen_C does not
allow hash-defined constants in expressions.

The world->logic array is an array of MAX_LOGIC arrays of TEXT_SIZE
characters. There are no dividers between the lines, so several lines
may be used as one line. The record_progress function may have written
beyond the end of the line it started writing on. That is detected
here by seeing if there is a 0 in the last position of the previous
line. All the last positions are initialized to 0 (and the last
positions are copied in copy_world), so if the last position of the
line is not zero, writing must have gone into the next line. In this
case, the world->logic line is incremented more than once to get beyond
the end of the string.

To be sure there is always lots of room at the end of the array (to
allow for long lines), this signals an error if the the logic_line
gets within ten lines of the end of the array.

*/

int logic_ok(                /* ARGUMENTS                */
 struct paint_world * world) /* puzzle model             */
{
  const char * name SET_TO "logic_ok";
  int stop;
  int last;

  stop SET_TO MAX_LOGIC;
  stop SET_TO (stop - 10);
  last SET_TO TEXT_SIZE;
  last SET_TO (last - 1);
  for (; world->logic[world->logic_line][last] ISNT 0; world->logic_line++)
    CHB((world->logic_line > stop), "too many logic lines");
  world->logic_line++;
  CHB((world->logic_line > stop), "too many logic lines");
  return OK;
}

/*************************************************************************/

/* main

Side Effects: This
1. calls init_world to initialize the puzzle model.
2. calls read_arguments to read the arguments.
3. calls read_problem to read the puzzle, record data, and make
   a lot of format checks.
4. calls init_solution to initialize the solving process.
5. calls solve_problem to solve the puzzle.

Notes:

This main follows the model for all mains for solving squares problems.
The model always has the five steps listed immediately above.

It is expected that solve_XXX may be called recursively for search,
but that some initial steps in the solution will need to be done only
once. The init_solution function takes these steps.

The executable takes two command arguments. The first argument is the
name of the file containing the puzzle. The second argument must be
"yes" or "no", with "yes" meaning that the partially solved puzzle
should be printed each time a rule is applied and has succeeded in
marking one or more squares full or empty.

Always finding all solutions is currently implemented.

Finding a solution to a puzzle with one or more solutions is
guaranteed if the program is left to run long enough.

If a puzzle has no solution, the message "Puzzle has no answer" will
be printed.

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

  world SET_TO worlds;
  IFF(init_world(world));
  IFF(read_arguments(argc, argv, world));
  IFF(read_problem(argv[1], world));
  IFF(init_solution(world));
  CHB((solve_problem(world) ISNT OK), "puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* mark_block_strip

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. mark_empty_square returns ERROR.
  2. extend_block returns ERROR.
  3. put_logic returns ERROR.
  4. A square is not blank or marked with X or dot: "bad square"

Called By:  try_block

Side Effects: For each block of a_strip, if the block is as long as
the largest target of the strip for a patch that contains the block,
dots are placed at the ends of the block. Also, if the block is
shorter than the minimum target of the strip for a patch that contains
the block, the ends of the block are extended, if possible.  Progress
is recorded.

*/

int mark_block_strip(        /* ARGUMENTS                       */
 int index1,                 /* index of the strip being tested */
 int stop,                   /* length of a_strip               */
 struct strip * a_strip,     /* the strip being tested          */
 struct paint_world * world) /* puzzle model                    */
{
  const char * name SET_TO "mark_block_strip";
  int pat;
  int max_target;
  int min_target;
  int index2;
  char item;
  int length;
  int done;

  world->progress_strip SET_TO 0;
  length SET_TO 0;
  for (index2 SET_TO 0; index2 <= stop; index2++)
    {
      item SET_TO get_square(index1, index2, world);
      if ((item IS '.') OR (item IS ' '))
	{
	  if (length)
	    {
	      done SET_TO 0;
	      max_target SET_TO 0;
	      min_target SET_TO (world->number_rows + world->number_cols);
	      for (pat SET_TO 0; pat < a_strip->number_patches; pat++)
		{
		  if (a_strip->patches[pat].start > (index2 - length));
		  else if (a_strip->patches[pat].end < (index2 - 1));
		  else if (a_strip->patches[pat].done)
		    {
		      if (a_strip->patches[pat].end IS (index2 - 1))
			{
			  done SET_TO 1;
			  break;
			}
		    }
		  else
		    {
		      if (a_strip->patches[pat].target > max_target)
			max_target SET_TO a_strip->patches[pat].target;
		      if (a_strip->patches[pat].target < min_target)
			min_target SET_TO a_strip->patches[pat].target;
		    }
		}
	      if (done);
	      else if (length IS max_target)
		{
		  IFF(mark_empty_square
		      (index1, (index2 - (length + 1)), world));
		  IFF(mark_empty_square(index1, index2, world));
		}
	      else if (min_target > length)
		IFF(extend_block
		    (index1, (index2 - 1), min_target, length, world));
	      length SET_TO 0;
	    }
	}
      else if (item IS 'X')
	{
	  if (length)
	    length++;
	  else
	    length SET_TO 1;
	}
      else
	CHK(1, "bad square");
    }
  if (world->progress_strip)
    IFF(put_logic(index1, world));
  return OK;
}

/*************************************************************************/

/* mark_empty_square

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. A square that should be marked empty is neither blank nor already
     marked empty: "puzzle has no answer".

Called By:
  init_patches_strip
  mark_block_strip
  mark_empty_strip
  mark_empty_strip2
  mark_patches_strip
  try_corner_col
  try_corner_row
  try_edge_square_col
  try_edge_square_row
  try_one_strip

Side Effects: If the given square is not already marked empty, it is
marked empty and world->progress_puzzle and world->progress_strip are
both set to 1.

*/

int mark_empty_square(       /* ARGUMENTS                              */
 int index1,                 /* first index of square (row or column)  */
 int index2,                 /* second index of square (column or row) */
 struct paint_world * world) /* puzzle model                           */
{
  const char * name SET_TO "mark_empty_square";

  if (get_square(index1, index2, world) IS ' ')
    {
      put_square(index1, index2, '.', world);
      world->progress_puzzle SET_TO 1;
      world->progress_strip SET_TO 1;
    }
  else
    CHK((get_square(index1, index2, world) ISNT '.'), "puzzle has no answer");
  return OK;
}

/*************************************************************************/

/* mark_empty_strip

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. mark_empty_square returns ERROR.
  2. put_logic returns ERROR.

Called By:  try_empty_outside

Side Effects: Squares in the strip that are before the start of the
first patch, after the end of the last patch, or between the end of
one patch and the start of the next are marked empty.

*/

int mark_empty_strip(        /* ARGUMENTS                              */
 int index1,                 /* index of row or column that is a_strip */
 int stop,                   /* length of a_strip                      */
 struct strip * a_strip,     /* the strip being processed              */
 struct paint_world * world) /* puzzle model                           */
{
  const char * name SET_TO "mark_empty_strip";
  int pat;
  int index2;

  world->progress_strip SET_TO 0;
  for (index2 SET_TO 0; index2 < a_strip->patches[0].start; index2++)
    {
      IFF(mark_empty_square(index1, index2, world));
    }
  for (pat SET_TO 0; pat < (a_strip->number_patches - 1); pat++)
    {
      for (index2 SET_TO (a_strip->patches[pat].end + 1);
	   index2 < a_strip->patches[pat + 1].start;
	   index2++)
	{
	  IFF(mark_empty_square(index1, index2, world));
	}
    }
  for (index2 SET_TO (stop - 1);
       index2 > a_strip->patches[a_strip->number_patches - 1].end;
       index2--)
    {
      IFF(mark_empty_square(index1, index2, world));
    }
  if (world->progress_strip)
    IFF(put_logic(index1, world));
  return OK;
}

/*************************************************************************/

/* mark_empty_strip2

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. mark_empty_square returns ERROR.
  2. put_logic returns ERROR.
  3. A square is found not marked with a blank, a dot, or an X: "bad square".

Called By:  try_empty_no_room

Side Effects: Squares in the strip that are in blocks of blank squares
between empty squares that are too small to hold any target of those
patches that are not outside the blank block and are not done are
marked empty.

*/

int mark_empty_strip2(       /* ARGUMENTS                                    */
 int index1,                 /* index of row or column that contains a_strip */
 int stop,                   /* length of a_strip                            */
 struct strip * a_strip,     /* the strip being processed                    */
 struct paint_world * world) /* puzzle model                                 */
{
  const char * name SET_TO "mark_empty_strip2";
  int pat;
  int on_blanks;
  int min_target;
  int index2;
  char item;
  int length;

  world->progress_strip SET_TO 0;
  on_blanks SET_TO 0;
  for (index2 SET_TO 0; index2 < stop; index2++)
    {
      item SET_TO get_square(index1, index2, world);
      if (item IS '.')
	{
	  if (on_blanks)
	    {
	      min_target SET_TO max(world->number_rows, world->number_cols);
	      for (pat SET_TO 0; pat < a_strip->number_patches; pat++)
		{
		  if (a_strip->patches[pat].end < (index2 - length));
		  else if (a_strip->patches[pat].start >= index2);
		  else if (a_strip->patches[pat].done);
		  else if (a_strip->patches[pat].target < min_target)
		    min_target SET_TO a_strip->patches[pat].target;
		}
	      if (length < min_target)
		{
		  for (; length > 0; length--)
		    {
		      IFF(mark_empty_square(index1, (index2 - length), world));
		    }
		}
	      on_blanks SET_TO 0;
	    }
	  if (get_square(index1, (index2 + 1), world) IS ' ')
	    {
	      length SET_TO 0;
	      on_blanks SET_TO 1;
	    }
	}
      else if (item IS 'X')
	{
	  on_blanks SET_TO 0;
	}
      else if (item IS ' ')
	{
	  if (on_blanks)
	    length++;
	}
      else
	CHK(1, "bad square");
    }
  if (world->progress_strip)
    IFF(put_logic(index1, world));
  return OK;
}

/*************************************************************************/

/* mark_patches_strip

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_min_starts returns ERROR.
  2. find_min_starts does not find min_starts: "puzzle has no answer"
  3. find_max_ends returns ERROR.
  4. enx_square returns ERROR.
  5. mark_empty_square returns ERROR.
  6. put_logic returns ERROR.

Called By:  try_full_bounded

Side Effects: Squares in each not done patch of the strip that must be
full are marked full. If the patch becomes done (indicated by the target
being the same size as [end plus one] minus start), the undone of the
strip is decremented. If the undone of the strip becomes zero, the
undone of the puzzle is decremented and any blank squares in the strip
are marked empty.

*/

int mark_patches_strip(      /* ARGUMENTS                         */
 int index1,                 /* index of row or column of a_strip */
 int stop,                   /* length of a_strip                 */
 struct strip * a_strip,     /* strip to be processed             */
 struct paint_world * world) /* puzzle model                      */
{
  const char * name SET_TO "mark_patches_strip";
  int pat;
  struct patch * patchn;
  int index2;              /* index of column or row */
  int found;

  world->progress_strip SET_TO 0;
  IFF(find_min_starts(index1, stop, a_strip, &found, world));
  CHK((NOT found), "puzzle has no answer");
  IFF(find_max_ends(index1, stop, a_strip, world));
  for (pat SET_TO 0; pat < a_strip->number_patches; pat++)
    {
      patchn SET_TO &(a_strip->patches[pat]);
      if (NOT (patchn->done))
	{
	  for (index2 SET_TO (patchn->end - patchn->target + 1);
	       index2 < (patchn->start + patchn->target);
	       index2++)
	    {
	      if (get_square(index1, index2, world) ISNT 'X')
		{
		  IFF(enx_square(index1, index2, world));
		}
	    }
	  if (((patchn->end + 1) - patchn->start) IS patchn->target)
	    {
	      patchn->done SET_TO 1;
	      a_strip->undone--;
	    }
	}
    }
  if (a_strip->undone IS 0)
    {
      world->undone--;
      for (index2 SET_TO 0; index2 < stop; index2++)
	{
	  if (get_square(index1, index2, world) IS ' ')
	    IFF(mark_empty_square(index1, index2, world));
	}
    }
  if (world->progress_strip)
    IFF(put_logic(index1, world));
  return OK;
}

/*************************************************************************/

/* mark_strip

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_last_undone_target returns ERROR.
  2. find_first_undone_target returns ERROR.

Called By:
  try_corner_col
  try_corner_row
  try_edge_test_col
  try_edge_test_row

Side Effects: This marks squares of the strip whose index is index1 (a
row if use_rows, a column if use_cols). Before marking the squares, it
copies their original values into save_strip, so they can be restored
later.

If it is impossible to fill in the strip because squares have already
been filled differently from what is required, *marked is set to 0.
Otherwise, *marked is set to 1.

Notes:

index1a is the index of the strip being marked.
index1b is the index of the strip next to the one being marked in
which the square actually being tested lies.

*/

int mark_strip(         /* ARGUMENTS                                       */
 int index1a,           /* row index if use_rows, column index if use_cols */
 int index1b,           /* row index if use_rows, column index if use_cols */
 int index2,            /* column index if use_rows, row index if use_cols */
 char * save_strip,     /* array of chars in which to save squares         */
 int size1,             /* number of places to mark (and save)             */
 int * marked,          /* set here to 1 if strip can be marked, else to 0 */
 struct strip * strips, /* columns if use_rows, rows if use_cols           */
 int sign,              /* +1 for right or down, -1 for left or up         */
 struct paint_world * world) /* puzzle model                               */
{
  const char * name SET_TO "mark_strip";
  int n;
  int size2;

  *marked SET_TO 1;
  for (n SET_TO 0; n < size1; n++)
    {
      save_strip[n] SET_TO get_square(index1a, (index2 + (sign * n)), world);
    }
  for (n SET_TO 0; n < size1; n++)
    {
      if (get_square(index1b, (index2 + (sign * n)), world) IS ' ')
	{
	  if (index1b < index1a)
	    IFF(find_first_undone_target
		(&(strips[index2 + (sign * n)]), &size2, world));
	  else
	    IFF(find_last_undone_target
		(&(strips[index2 + (sign * n)]), &size2, world));
	  if (size2 IS 1)
	    {
	      if (save_strip[n] IS 'X')
		{
		  *marked SET_TO 0;
		  break;
		}
	      else
		put_square(index1a, (index2 + (sign * n)), '.', world);
	    }
	  else
	    {
	      if (save_strip[n] IS '.')
		{
		  *marked SET_TO 0;
		  break;
		}
	      else
		put_square(index1a, (index2 + (sign * n)), 'X', world);
	    }
	}
    }
  return OK;
}

/*************************************************************************/

/* max

Returned Value: int (the larger of int1 and int2)

Called By:
  find_best_patch
  mark_empty_strip2
  try_edge_square_col
  try_edge_square_row

Side Effects: none

*/

int max(   /* ARGUMENTS       */
 int int1, /* an integer      */
 int int2) /* another integer */
{
  const char * name SET_TO "max";
  int answer;

  answer SET_TO ((int1 > int2) ? int1 : int2);
  return answer;
}

/*************************************************************************/

/* min

Returned Value: int (the smaller of int1 and int2)

Called By:
  try_corner_col
  try_corner_row
  try_edge_square_col
  try_edge_square_row

Side Effects: none

*/

int min(   /* ARGUMENTS       */
 int int1, /* an integer      */
 int int2) /* another integer */
{
  const char * name SET_TO "min";
  int answer;

  answer SET_TO ((int1 < int2) ? int1 : int2);
  return answer;
}

/*************************************************************************/

/* print_paint

Returned Value: int (OK)

Called By:
  get_paint2
  record_progress
  solve_problem

Side Effects: the puzzle is printed at its current stage of solution.

Notes:

This is printing a narrow version of the puzzle. The column number
reader narrows the column number lines.

Adding  'system("clear");' as the first line keeps the display in one
place while the solver is working, but then only part of the puzzle appears
when scrolling back, so the in-progress work is lost.

*/

int print_paint(             /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "print_paint";
  int line;
  int row;
  int col;

  printf("\n");
  for (line SET_TO 0; line < world->col_lines; line++)
    {
      printf("%s", world->col_numbers[line]);
    }
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      printf("%s", world->row_numbers[row]);
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  printf("%c", world->squares[row][col]);
	}
      printf("|\n");
    }
  printf("%s\n", world->col_numbers[line - 1]);
  return OK;
}

/*************************************************************************/

/* put_logic

Returned Value: none

Called By:
  init_patches_strip
  mark_block_strip
  mark_empty_strip
  mark_empty_strip2
  mark_patches_strip
  try_corners
  try_edge_col
  try_edge_row
  try_guessing
  try_one_strip

Side Effects: the given row or column is added to the list in the puzzle
model of rows or columns on which progress has been made, and the number
in the model of such rows or columns is increased by one.

*/

int put_logic(               /* ARGUMENTS              */
 int index,                  /* the index of the strip */
 struct paint_world * world) /* puzzle model           */
{
  const char * name SET_TO "put_logic";

  if (world->use_rows)
    world->in_rows[world->number_in_rows++] SET_TO index;
  else
    world->in_cols[world->number_in_cols++] SET_TO index;
  return OK;
}

/*************************************************************************/

/* put_square

Returned Value: int (OK)

Called By:
  mark_empty_square
  mark_strip
  try_corner_col
  try_corner_row
  try_edge_test_col
  try_edge_test_row
  try_one_strip

Side Effects: the given character (item) is put into the puzzle at the
[index1, index2] position if world->use_rows or at the [index2, index1]
position, if not.

Notes:

This is not checking that the square is in bounds.

*/

int put_square(              /* ARGUMENTS            */
 int index1,                 /* the first index      */
 int index2,                 /* the second index     */
 char item,                  /* the character to put */
 struct paint_world * world) /* puzzle model         */
{
  const char * name SET_TO "put_square";

  if (world->use_rows)
    world->squares[index1][index2] SET_TO item;
  else
    world->squares[index2][index1] SET_TO item;
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

Side Effects: The value of world->print_all is set. If there is an
argument error, a usage message is printed.

*/

int read_arguments(          /* ARGUMENTS                             */
 int argc,                   /* one more than the number of arguments */
 char ** argv,               /* function name and arguments           */
 struct paint_world * world) /* puzzle model                          */
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

/* read_col_numbers

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. The file ends while reading column numbers:
     "file ends while reading column numbers".
  2. add_patch returns ERROR.
  3. Two consecutive characters in the column numbers area do not make
     a number and are not blank: "bad column number".
  4. There are more than (MAX_STRIP_BLOCKS - 1) rows of column numbers:
     "too many rows of column numbers".

Called By:  get_paint2

Side Effects: This reads and records the column numbers that appear
above the puzzle. The lines of numbers are recoded (so each number
takes one space rather than two) and saved in the world->col_numbers
array. The strip for each column has patches added to it.
world->col_lines is set. The top line of dashes is read, shortened,
and saved.

Notes:

This expects that the first unread line is the top row of numbers.
It stops reading after reading the first line of dashes.

The coding for column numbers for printing puts each number into a
single character as follows:

 0 -  9 -> 0 - 9
10 - 35 -> a - z
36 - 61 -> A - Z
   > 61 -> ~

Thus, all column numbers greater than 61 are printed as ~ in the output.
This is inconvenient, but targets greater than 61 are very rare. It
would be more inconvenient to double the width of the output.

The column numbers for solving, however, are as read for 0 - 99.
Since there are only two spaces allocated for the numbers, numbers
greater than 99 are coded in the input so that A = 100, B = 110, etc.
Thus, targets as large as (99 + 26*10 =) 359 (= Z9) can be encoded in
the input file.

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
  char * buffer;

  digits[2] SET_TO 0;
  for (line SET_TO 0; line < MAX_STRIP_BLOCKS; line++)
    {
      buffer SET_TO world->col_numbers[line];
      CHB((fgets(buffer, TEXT_SIZE, in_port) IS NULL),
	  "file ends while reading column numbers");
      if (buffer[world->number_spaces + 1] IS '-')
	{
	  buffer[(world->number_spaces + 1) + world->number_cols] SET_TO '\n';
	  buffer[(world->number_spaces + 2) + world->number_cols] SET_TO 0;
	  break;
	}
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
	      IFF(add_patch(&(world->cols[col]), length));
	      buffer[(world->number_spaces + 1) + col] SET_TO '~';
	    }
	  else if (sscanf(digits, "%d", &length) IS 1)
	    {
	      *col_total SET_TO (*col_total + length);
	      IFF(add_patch(&(world->cols[col]), length));
	      buffer[(world->number_spaces + 1) + col] SET_TO
		((length < 10) ?
		 (48 + length) :
		 (length < 36) ?
		 (87 + length) :
		 (length < 62) ?
		 (29 + length) : '~');
	    }
	  else if ((digits[0] IS ' ') AND (digits[1] IS ' '))
	    buffer[(world->number_spaces + 1) + col] SET_TO ' ';
	  else
	    CHB(1, "bad column number");
	  col++;
	}
      buffer[(world->number_spaces + 1) + col] SET_TO '\n';
      buffer[(world->number_spaces + 2) + col] SET_TO 0;
    }
  CHB((buffer[world->number_spaces + 1] ISNT '-'),
      "too many rows of column numbers");
  world->col_lines SET_TO (line + 1);
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
  2. add_patch returns ERROR.

Called By:  get_paint2

Side Effects: This reads and records the row numbers that appear
to the left of the puzzle.

Notes:

This repeatedly reads a line of row numbers into a new line of the
world->row_numbers array and then transcribes characters from the
beginning of the row into the digits array three at a time. Then it
tries to read a number from the digits array. If there is one, it is
recorded by add_patch. The line in the world->row_numbers array is
null terminated at the end of the numbers it holds.

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
	      IFF(add_patch(&(world->rows[row]), length));
	    }
	}
      buffer[n + 1] SET_TO 0;
      strcpy(world->row_numbers[row], buffer);
    }
  return OK;
}

/*************************************************************************/

/* record_progress

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. There are more than MAX_LOGIC logic lines: "too many logic lines"

Called By:
  init_patches
  try_block
  try_corners
  try_edges
  try_empty_no_room
  try_empty_outside
  try_full_bounded
  try_guessing
  try_one

Side Effects: If progress has been made, a line of explanation is
recorded, and if world->print_all is non-zero, the puzzle is printed.

This is not checking the length of the strings it writes and may write
beyond the end of the current logic line. This is OK because that will
be detected in logic_ok and the logic line will be incremented. In
this case two (or more) lines of the array are effectively
concatenated. The logic_ok function also does not allow the last
ten lines of the array to be used except for overflow, so overwriting
the bounds of the array, while theoretically possible, is very unlikely.

See documentation of logic_ok for more details.

*/

int record_progress(         /* ARGUMENTS       */
 char * tried,               /* text of message */
 struct paint_world * world) /* puzzle model    */
{
  const char * name SET_TO "record_progress";
  int numb;     /* number of characters printed       */
  int n;        /* counter for in_rows and columns_in */
  char * where; /* where to put next characters       */

  if (world->progress_puzzle ISNT 0)
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
	      numb SET_TO sprintf(where, "%d ", (1 + world->in_rows[n]));
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
	      numb SET_TO sprintf(where, "%d ", (1 + world->in_cols[n]));
	      where SET_TO (where + numb);
	    }
	}
      if (world->print_all)
	{
	  print_paint(world);
	  printf("%s\n\n", world->logic[world->logic_line]);
	}
      IFF(logic_ok(world));
    }
  return OK;
}

/*************************************************************************/

/* shift_patches_left

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. A patch cannot fit where it is required to fit: "puzzle has no answer"

Called By:
  find_max_ends
  try_guessing

Side Effects: If possible, this shifts [the patch whose index in
a_strip is pat] to the left (or up) so that the end of the patch
is on the square whose index is index2 and then sets shifted to 1.  If
not possible (because the block in the patch would be pushed off the
left end of the strip), this sets shifted to 0 and returns.

If the first shift works, and it is necessary, the ends of patches
to the left of the shifted patch are also shifted left so that for
each pair of adjacent patches, the end of the patch on the left is
two squares to the left of the left end of the block in the patch on
the right when the block is in its rightmost possible position. If any
shift is impossible (because a block would be pushed off the left end
of the strip), this sets shifted to 0.

Notes:

The index of the row or column being processed is not needed.

This is a mirror image of shift_patches_right, except that
shift_patches_left does not need a stop argument, since the
index at the left end of the strip is known to be zero.

*/

int shift_patches_left(  /* ARGUMENTS                        */
 int pat,                /* index of patch in a_strip        */
 int index2,             /* index at which to end patch      */
 int * shifted,          /* non-zero means success, set here */
 struct strip * a_strip) /* the strip being processed        */
{
  const char * name SET_TO "shift_patches_left";
  struct patch * a_patch;

  a_patch SET_TO &(a_strip->patches[pat]);
  a_patch->end SET_TO index2;
  if ((index2 - a_patch->target) < -1)
    *shifted SET_TO 0;
  else
    {
      pat--;
      for (index2 SET_TO (index2 - a_patch->target - 1);
	   ((pat > -1) AND (a_strip->patches[pat].end > index2));
	   index2 SET_TO (index2 - a_patch->target - 1))
	{
	  a_patch SET_TO &(a_strip->patches[pat]);
	  if ((index2 - a_patch->target) < -1)
	    {
	      *shifted SET_TO 0;
	      break;
	    }
	  a_patch->end SET_TO index2;
	  pat--;
	}
    }
  return OK;
}

/*************************************************************************/

/* shift_patches_right

Returned Value: int (OK)

Called By:
  find_min_starts
  try_guessing

Side Effects: If possible, this shifts [the patch whose index in
a_strip is pat] to the right (or down) so that the start of the patch
is on the square whose index is index2 and then sets shifted to 1.  If
not possible (because the block in the patch would be pushed off the
right end of the strip), this sets shifted to 0 and returns.

If the first shift works, and it is necessary, the starts of patches
to the right of the shifted patch are also shifted right so that for
each pair of adjacent patches, the start of the patch on the right is
two squares to the right of the right end of the block in the patch on
the left when the block is in its leftmost possible position. If any
shift is impossible (because a block would be pushed off the right end
of the strip), this sets shifted to 0.

Notes:

The index of the row or column being processed is not needed.

This is a mirror image of shift_patches_left, except that
shift_patches_left does not need a stop argument, since the
index at the left end of the strip is known to be zero.

*/

int shift_patches_right( /* ARGUMENTS                        */
 int pat,                /* index of patch in a_strip        */
 int index2,             /* index at which to start patch    */
 int stop,               /* length of a_strip                */
 int * shifted,          /* non-zero means success, set here */
 struct strip * a_strip) /* the strip being processed        */
{
  const char * name SET_TO "shift_patches_right";
  struct patch * a_patch;

  a_patch SET_TO &(a_strip->patches[pat]);
  a_patch->start SET_TO index2;
  if ((index2 + a_patch->target) > stop)
    *shifted SET_TO 0;
  else
    {
      *shifted SET_TO 1;
      pat++;
      for (index2 SET_TO (index2 + a_patch->target + 1);
	   ((pat < a_strip->number_patches) AND
	    (a_strip->patches[pat].start < index2));
	   index2 SET_TO (index2 + a_patch->target + 1))
	{
	  a_patch SET_TO &(a_strip->patches[pat]);
	  if ((index2 + a_patch->target) > stop)
	    {
	      *shifted SET_TO 0;
	      break;
	    }
	  a_patch->start SET_TO index2;
	  pat++;
	}
    }
  return OK;
}

/*************************************************************************/

/* solve_problem

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. explain_paint returns ERROR.
  2. print_paint returns ERROR.
  3. try_block returns ERROR.
  4. try_corners returns ERROR.
  5. try_edges returns ERROR.
  6. try_empty_no_room returns ERROR.
  7. try_empty_outside returns ERROR.
  8. try_full_bounded returns ERROR.
  9. try_guessing returns ERROR.
 10. try_one returns ERROR.

Called By:
  main
  try_guessing

Side Effects: This goes through a loop repeatedly.

It tries various methods of marking blank squares either empty or
full. If any method marks one or more squares and world->print_all is
non-zero, it prints an updated puzzle and lists the rows and/or
columns in which it marked squares.

The loop tries methods in batches. Once it starts a batch, it tries
all the methods in that batch. Then, if any method in the batch has
succeeded in marking a square, it goes back to the beginning of the
loop. The batches are:

a. try_full_bounded, try_empty_no_room, try_empty_outside
b. try_block
c. try_edges, try_corners
d. try_one

If none of the methods marks a square, the loop is exited.  This
happens if either (a) the puzzle is done or (b) the puzzle is not
done, but none of the methods is able to make progress.  If the puzzle
is not done, try_guessing is called. If the puzzle is done, the
answer is printed.

Notes:

It might be a little faster to order the strips by number of changes
since the last time the strip was processed, but probably not much
faster, and ordering requires extra work. Changes are not being
tracked.

Doneness is tracked as follows.

At the world level, there is world->undone, which is the number of
strips not yet done. This is initialized to the number of rows plus
the number of columns. It is decremented in init_patches_strip and
mark_patches_strip. The puzzle is done when world->undone is zero.

At the strip level, there is an "undone" data member that counts the
number of undone patches. The undone for a strip is:
1. initialized to zero by init_world.
2. incremented by add_patch (an initialization procedure).
3. decremented by init_patches_strip (which determines if any are done).
4. decremented in mark_patches_strip (which determines if any are done).

At the patch level, there is a "done" data member that indicates whether
the final patch boundaries have been determined (the squares of the patch
will all be filled when a patch is marked done). The done for a patch is:
1. initialized to zero in add_patch.
2. set to one in init_patches_strip.
3. set to one in mark_patches_strip.

It is possible that mark_patches_strip (called by try_full_bounded)
changes world->undone to zero without marking any squares. In this
case, the loop runs again, but of course no progress is made (since
everything is done). This does not waste much time, because each
function works only on undone strips.

The use of try_guessing implements depth-first search.

*/

int solve_problem(           /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "solve_problem";
  int progress;

  for (world->progress_puzzle SET_TO 0;
       world->undone ISNT 0;
       world->progress_puzzle SET_TO 0)
    {
      progress SET_TO 0;
      IFF(try_full_bounded(world));
      if (world->progress_puzzle)
	{
	  progress SET_TO 1;
	  world->progress_puzzle SET_TO 0;
	}
      IFF(try_empty_outside(world));
      if (world->progress_puzzle)
	{
	  progress SET_TO 1;
	  world->progress_puzzle SET_TO 0;
	}
      IFF(try_empty_no_room(world));
      if (world->progress_puzzle)
	{
	  progress SET_TO 1;
	  world->progress_puzzle SET_TO 0;
	}
      if (progress)
	continue;
      IFF(try_block(world));
      if (world->progress_puzzle)
	{
	  progress SET_TO 1;
	  world->progress_puzzle SET_TO 0;
	}
      if (progress)
	continue;
      IFF(try_edges(world));
      if (world->progress_puzzle)
	{
	  progress SET_TO 1;
	  world->progress_puzzle SET_TO 0;
	}
      IFF(try_corners(world));
      if (world->progress_puzzle)
	{
	  progress SET_TO 1;
	  world->progress_puzzle SET_TO 0;
	}
      if (progress)
	continue;
      IFF(try_one(world));
      if (world->progress_puzzle)
	{
	  progress SET_TO 1;
	  world->progress_puzzle SET_TO 0;
	}
      if (progress OR (world->undone IS 0))
	continue;
      if (progress IS 0)
	break;
    }
  if (world->undone IS 0)
    {
      IFF(print_paint(world));
      IFF(explain_paint(world));
    }
  else
    {
      IFF(try_guessing(world));
    }
  return OK;
}

/*************************************************************************/

/* try_block

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. mark_block_strip returns ERROR.
  2. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: For each strip of the puzzle, if a block of squares is
as long as the largest target of the strip that might cover it, dots
are placed at the ends of the block. Also, if the block is shorter
than the minimum target of the strip that might cover it, the ends of
the block are extended, if possible.

Notes:

If a strip holds several patches with the same size target it may
occur that several blocks are finished but it is not possible to
identify the blocks with specific patches. In the example below, it is
not possible to tell whether the two blocks are the first and second,
first and third, or second and third.

  2  2  2|         .XX.          .XX.            |

This function finds and marks blocks that can be determined to be
complete but cannot be identified with specific patches.

*/

int try_block(               /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "try_block";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  world->use_rows SET_TO 1;
  for (row SET_TO 0; row < world->number_rows; row++)
    if (world->rows[row].undone)
      IFF(mark_block_strip
	  (row, world->number_cols, &(world->rows[row]), world));
  world->number_in_cols SET_TO 0;
  world->use_rows SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    if (world->cols[col].undone)
      IFF(mark_block_strip
	  (col, world->number_rows, &(world->cols[col]), world));
  IFF(record_progress("mark empty or full squares for block in ", world));
  return OK;
}

/*************************************************************************/

/* try_corner_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_first_undone_target returns ERROR.
  2. find_last_undone_target returns ERROR.
  3. mark_strip returns ERROR.
  4. copy_strip returns ERROR.
  5. find_min_starts returns ERROR.
  6. mark_empty_square returns ERROR.

Called By:  try_corners

Side Effects: See documentation of try_corners.

This handles the four cases shown on the left side of the figure for
try_corners.

*/

int try_corner_col(          /* ARGUMENTS                           */
 int row,                    /* row of square being tested          */
 int col,                    /* column of square being tested       */
 int i,                      /* row increment, 1 or -1              */
 int j,                      /* column increment, 1 or -1           */
 struct paint_world * world) /* puzzle model                        */
{
  const char * name SET_TO "try_corner_col";
  int stop_row;              /* index of first out-of-bounds row    */
  int stop_col;              /* index of first out-of-bounds column */
  int length;                /* length of corner                    */
  int n;                     /* utility index, gets re-used         */
  char item;
  char save_col[MAX_NUMBER_ROWS];
  int size;                  /* length of patch                     */
  int marked;
  int found;
  struct strip strip_copy;

  stop_row SET_TO ((i IS 1) ? -1 : world->number_rows);
  stop_col SET_TO ((j IS 1) ? -1 : world->number_cols);
  for (length SET_TO 0;
       ((get_square((row + length), (col - j), world) IS '.') AND
	(get_square((row + length), col, world) IS ' '));
       length SET_TO (length + i))
    {
      for (n SET_TO (col - j); n ISNT stop_col; n SET_TO (n - j))
	{
	  item SET_TO get_square((row + length), n, world);
	  if ((item ISNT '.') AND (item ISNT 'X'))
	    break;
	}
      if (n ISNT stop_col)
	break;
    }
  for (n SET_TO (row - i); n ISNT stop_row; n SET_TO (n - i))
    {
      item SET_TO get_square(n, col, world);
      if ((item ISNT 'X') AND (item ISNT '.'))
	break;
    }
  if (length < 0)
    length SET_TO -length;
  if ((n IS stop_row) AND (length > 1))
    {
      if (stop_row IS -1)
	IFF(find_first_undone_target(&(world->cols[col]), &size, world));
      else
	IFF(find_last_undone_target(&(world->cols[col]), &size, world));
      world->use_rows SET_TO 0;
      IFF(mark_strip((col + j), col, row, save_col, min(size, length), &marked,
		     world->rows, i, world));
      world->use_rows SET_TO 1;
      if (marked)
	{
	  IFF(copy_strip(&strip_copy, &(world->cols[col + j]), world));
	  world->use_rows SET_TO 0;
	  IFF(find_min_starts
	      ((col + j), world->number_rows, &strip_copy, &found, world));
	  world->use_rows SET_TO 1;
	}
      for (n SET_TO 0; n < min(size, length); n++)
	put_square((row + (n * i)), (col + j), save_col[n], world);
      if ((NOT marked) OR (NOT found))
	IFF(mark_empty_square(row, col, world));
    }
  return OK;
}

/*************************************************************************/

/* try_corner_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_first_undone_target returns ERROR.
  2. find_last_undone_target returns ERROR.
  3. mark_strip returns ERROR.
  4. copy_strip returns ERROR.
  5. find_min_starts returns ERROR.
  6. mark_empty_square returns ERROR.

Called By:  try_corners

Side Effects: See documentation of try_corners.

This handles the four cases shown on the right side of the figure for
try_corners.

*/

int try_corner_row(          /* ARGUMENTS                           */
 int row,                    /* row of square being tested          */
 int col,                    /* column of square being tested       */
 int i,                      /* row increment, 1 or -1              */
 int j,                      /* column increment, 1 or -1           */
 struct paint_world * world) /* puzzle model                        */
{
  const char * name SET_TO "try_corner_row";
  int stop_row;              /* index of first out-of-bounds row    */
  int stop_col;              /* index of first out-of-bounds column */
  int length;                /* length of corner                    */
  int n;                     /* utility index, gets re-used         */
  char item;
  char save_row[MAX_NUMBER_COLS];
  int size;                  /* length of patch                     */
  int marked;
  int found;
  struct strip strip_copy;

  stop_row SET_TO ((i IS 1) ? -1 : world->number_rows);
  stop_col SET_TO ((j IS 1) ? -1 : world->number_cols);
  for (length SET_TO 0;
       ((get_square((row - i), (col + length), world) IS '.') AND
	(get_square(row, (col + length), world) IS ' '));
       length SET_TO (length + j))
    {
      for (n SET_TO (row - i); n ISNT stop_row; n SET_TO (n - i))
	{
	  item SET_TO get_square(n, (col + length), world);
	  if ((item ISNT '.') AND (item ISNT 'X'))
	    break;
	}
      if (n ISNT stop_row)
	break;
    }
  for (n SET_TO (col - j); n ISNT stop_col; n SET_TO (n - j))
    {
      item SET_TO get_square(row, n, world);
      if ((item ISNT 'X') AND (item ISNT '.'))
	break;
    }
  if (length < 0)
    length SET_TO -length;
  if ((n IS stop_col) AND (length > 1))
    {
      if (stop_col IS -1)
	IFF(find_first_undone_target(&(world->rows[row]), &size, world));
      else
	IFF(find_last_undone_target(&(world->rows[row]), &size, world));
      IFF(mark_strip((row + i), row, col, save_row, min(size, length), &marked,
		     world->cols, j, world));
      if (marked)
	{
	  IFF(copy_strip(&strip_copy, &(world->rows[row + i]), world));
	  IFF(find_min_starts
	      ((row + i), world->number_cols, &strip_copy, &found, world));
	}
      for (n SET_TO 0; n < min(size, length); n++)
	put_square((row + i), (col + (n * j)), save_row[n], world);
      if ((NOT marked) OR (NOT found))
	IFF(mark_empty_square(row, col, world));
    }
  return OK;
}

/*************************************************************************/

/* try_corners

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. try_corner_row returns ERROR.
  2. try_corner_col returns ERROR.
  3. put_logic returns ERROR.
  4. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: Corners inside the puzzle are identified, and squares
at the corner and along the edges of the corner are marked empty if
possible. Any progress is reported.

Notes:

A corner is defined as a configuration of empty squares that makes
a corner, with
1. one empty square in one direction,
2. at least two empty squares in the other direction,
3. blank squares adjacent to the empty squares inside the corner,
4. marked squares (empty or full) extending to the edge of the puzzle,
   starting at every empty square
The squares considered in a corner are all the blank ones adjacent to
the direction with two or more empty squares. The eight corner types
are shown in the figure below. The arrows show where the rows and
columns must be marked all the way to the edge of the puzzle. The
dots show the squares that must be empty. The Bs show the squares
that must be blank.

The method at a corner is to attempt to prove that the first square
must be empty by temporarily pretending it is marked full, and
seeing what the consequences are for the next row (or column) over.
The hope is that the consequences will be contrary to the requirements
for that row (or column).

The requirement that the squares shown by arrows all be marked ensures
that the empty square (if full) is covered by the first undone patch
in the row (or column).


   ^    ^             ^^  ^^
   |    |             ||  ||
   .    .             ..  ..
<-.B    B.->       <-.BB  BB.->
<-.B    B.->

<-.B    B.->
<-.B    B.->       <-.BB  BB.->
   .    .             ..  ..
   |    |             ||  ||
   v    v             vv  vv

Here is a simple example, using an upper left corner (that is an
actual corner of a puzzle). Recall that edges of the puzzle are
treated like empty squares. If the square marked X is full, then the
three squares to the right of it marked R must also be full (because
of the 4). Then the two squares below the X marked D must also be full
(because of the 3), and the square below the first R marked D must be
full (because of the 2).  But this makes an illegal condition (two
full squares in a row) next to the 1. Hence, the square marked with an
X must be empty.

   32
   ----
 4|XRRR
 1|DD
  |D

*/

int try_corners(             /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "try_corners";
  int row;
  int col;

  world->use_rows SET_TO 1;
  world->number_in_rows SET_TO 0;
  world->number_in_cols SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      world->progress_strip SET_TO 0;
      for (col SET_TO 0; col < world->number_cols; col++)
	{
	  if (get_square(row, col, world) IS ' ')
	    {
	      if ((get_square((row - 1), col, world) IS '.') AND
		  (get_square(row, (col - 1), world) IS '.'))
		{
		  IFF(try_corner_row(row, col, 1, 1, world));
		  IFF(try_corner_col(row, col, 1, 1, world));
		}
	      else if ((get_square((row - 1), col, world) IS '.') AND
		       (get_square(row, (col + 1), world) IS '.'))
		{
		  IFF(try_corner_row(row, col, 1, -1, world));
		  IFF(try_corner_col(row, col, 1, -1, world));
		}
	      else if ((get_square((row + 1), col, world) IS '.') AND
		       (get_square(row, (col + 1), world) IS '.'))
		{
		  IFF(try_corner_row(row, col, -1, -1, world));
		  IFF(try_corner_col(row, col, -1, -1, world));
		}
	      else if ((get_square((row + 1), col, world) IS '.') AND
		       (get_square(row, (col - 1), world) IS '.'))
		{
		  IFF(try_corner_row(row, col, -1, 1, world));
		  IFF(try_corner_col(row, col, -1, 1, world));
		}
	    }
	}
      if (world->progress_strip)
	IFF(put_logic(row, world));
    }
  IFF(record_progress("mark empty corner squares in ", world));
  return OK;
}

/*************************************************************************/

/* try_edge_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. try_edge_square_col returns ERROR.
  2. put_logic returns ERROR.
  3. A square is not marked empty, blank, or full: "bad square".

Called By:  try_edges

Side Effects: This tries one vertical edge (see try_edges)

*/

int try_edge_col(     /* ARGUMENTS                                     */
 int col,             /* index of column being processed               */
 int beside,          /* index of column beside column being processed */
 struct paint_world * world) /* puzzle model                           */
{
  const char * name SET_TO "try_edge_col";
  int row;  /* row index */
  int k;    /* another row index */
  int size;
  char item;
  int low;

  world->progress_strip SET_TO 0;
  world->use_rows SET_TO 1;
  size SET_TO 0;
  for (row SET_TO 0; row < world->number_rows; row++)
    {
      item SET_TO get_square(row, col, world);
      if ((item IS ' ') OR (item IS 'X'))
	size++;
      else if (item IS '.')
	{
	  if (size < 2)
	    {
	      size SET_TO 0;
	      continue;
	    }
	  low SET_TO (row - (size + 1));
	  for (k SET_TO (row - size); k < row; k++)
	    {
	      if (get_square(k, col, world) IS ' ')
		{
		  IFF(try_edge_square_col(col, beside, k, &low, row, world));
		}
	    }
	  size SET_TO 0;
	}
      else
	CHK(1, "bad square");
    }
  if (world->progress_strip)
    {
      world->use_rows SET_TO 0;
      IFF(put_logic(col, world));
      world->use_rows SET_TO 1;
    }
  return OK;
}

/*************************************************************************/

/* try_edge_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. try_edge_square_row returns ERROR.
  2. put_logic returns ERROR.
  3. A square is not marked empty, blank, or full: "bad square".

Called By:  try_edges

Side Effects: This tries one horizontal edge (see try_edges)

*/

int try_edge_row(            /* ARGUMENTS                               */
 int row,                    /* index of row being processed            */
 int beside,                 /* index of row beside row being processed */
 struct paint_world * world) /* puzzle model                            */
{
  const char * name SET_TO "try_edge_row";
  int col;  /* column index */
  int k;    /* another column index */
  int size;
  char item;
  int low;

  world->progress_strip SET_TO 0;
  world->use_rows SET_TO 1;
  size SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    {
      item SET_TO get_square(row, col, world);
      if ((item IS ' ') OR (item IS 'X'))
	size++;
      else if (item IS '.')
	{
	  if (size < 2)
	    {
	      size SET_TO 0;
	      continue;
	    }
	  low SET_TO (col - (size + 1));
	  for (k SET_TO (col - size); k < col; k++)
	    {
	      if (get_square(row, k, world) IS ' ')
		{
		  IFF(try_edge_square_row(row, beside, k, &low, col, world));
		}
	    }
	  size SET_TO 0;
	}
      else
	CHK(1, "bad square");
    }
  if (world->progress_strip)
    IFF(put_logic(row, world));
  return OK;
}

/*************************************************************************/

/* try_edge_square_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_min_target returns ERROR.
  2. try_edge_test_col returns ERROR.
  3. mark_empty_square returns ERROR.

Called By:  try_edge_col

Side Effects: The square at row,col is known to be blank. This
determines if it must be empty and, if so, (i) puts a dot in it,
and (ii) resets *low.

Notes:

All columns after or before the given column are done, so they must be
completely filled.

In the first "if", the size does not fit, so must be empty.

*/

int try_edge_square_col( /* ARGUMENTS                                     */
 int col,                /* index of column being processed               */
 int beside,             /* index of column beside column being processed */
 int row,                /* index of row being processed                  */
 int * low,              /* index of empty row above                      */
 int high,               /* index of empty row below                      */
 struct paint_world * world) /* puzzle model                              */
{
  const char * name SET_TO "try_edge_square_col";
  int is_empty;
  int size;
  int k;

  IFF(find_min_target(row, &(world->cols[col]), &size, world));
  is_empty SET_TO 1;
  if (size > (high - (*low + 1)));
  else
    {
      for (k SET_TO max((*low + 1), ((row + 1) - size));
	   k <= min((high - size), row);
	   k++)
	{
	  IFF(try_edge_test_col(col, beside, k, size, &is_empty, world));
	  if (NOT (is_empty))
	    break;
	}
    }
  if (is_empty)
    {
      IFF(mark_empty_square(row, col, world));
      *low SET_TO row;
    }
  return OK;
}

/*************************************************************************/

/* try_edge_square_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_min_target returns ERROR.
  2. try_edge_test_row returns ERROR.
  3. mark_empty_square returns ERROR.

Called By:  try_edge_row

Side Effects: The square at row,col is known to be blank. This
determines if it must be empty and, if so, (i) puts a dot in it,
and (ii) resets *low.

Notes:

All rows before or after the given row are done, so they must be
completely filled.

*/

int try_edge_square_row(     /* ARGUMENTS                               */
 int row,                    /* index of row being processed            */
 int beside,                 /* index of row beside row being processed */
 int col,                    /* index of column being processed         */
 int * low,                  /* index of empty column on left           */
 int high,                   /* index of empty column on right          */
 struct paint_world * world) /* puzzle model                            */
{
  const char * name SET_TO "try_edge_square_row";
  int is_empty;
  int size;
  int k;

  IFF(find_min_target(col, &(world->rows[row]), &size, world));
  is_empty SET_TO 1;
  if (size > (high - (*low + 1)));
  else
    {
      for (k SET_TO max((*low + 1), ((col + 1) - size));
	   k <= min((high - size), col);
	   k++)
	{
	  IFF(try_edge_test_row(row, beside, k, size, &is_empty, world));
	  if (NOT (is_empty))
	    break;
	}
    }
  if (is_empty)
    {
      IFF(mark_empty_square(row, col, world));
      *low SET_TO col;
    }
  return OK;
}

/*************************************************************************/

/* try_edge_test_col

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_min_starts returns ERROR.
  2. copy_strip returns ERROR.
  3. mark_strip returns ERROR.

Called By:  try_edge_square_col

Side Effects: *is_empty is set to 0 if it is possible to mark the column
beside the one being tested and find a way to fit in all the patches.

Notes:

*is_empty is expected to be set to 1 before this starts.

*/

int try_edge_test_col( /* ARGUMENTS                                     */
 int col,              /* index of column with the square being tested  */
 int beside,           /* index of column beside col                    */
 int row,              /* index of row with the square being tested     */
 int size,             /* size of the block to fit in                   */
 int * is_empty,       /* set to 0 if block fits in row starting        */
 struct paint_world * world) /* puzzle model                            */
{
  const char * name SET_TO "try_edge_test_col";
  char save_col[MAX_NUMBER_ROWS];
  int marked;
  int found;
  struct strip strip_copy;
  int n;

  world->use_rows SET_TO 0;
  IFF(mark_strip
      (beside, col, row, save_col, size, &marked, world->rows, 1, world));
  if (marked)
    {
      IFF(copy_strip(&strip_copy, &(world->cols[beside]), world));
      IFF(find_min_starts
	  (beside, world->number_rows, &strip_copy, &found, world));
    }
  world->use_rows SET_TO 1;
  for (n SET_TO 0; n < size; n++)
    put_square((row + n), beside, save_col[n], world);
  if (marked AND found)
    *is_empty SET_TO 0;
  return OK;
}

/*************************************************************************/

/* try_edge_test_row

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_min_starts returns ERROR.
  2. copy_strip returns ERROR.
  3. mark_strip returns ERROR.

Called By:  try_edge_square_row

Side Effects: *is_empty is set to 0 if it is possible to mark the row
beside the one being tested and find a way to fit in all the patches.

Notes:

*is_empty is expected to be set to 1 before this starts.

*/

int try_edge_test_row( /* ARGUMENTS                                     */
 int row,              /* index of row with the square being tested     */
 int beside,           /* index of row beside row                       */
 int col,              /* index of column with the square being tested  */
 int size,             /* size of the block to fit in                   */
 int * is_empty,       /* set to 0 if block fits in row starting        */
 struct paint_world * world) /* puzzle model                            */
{
  const char * name SET_TO "try_edge_test_row";
  char save_row[MAX_NUMBER_COLS];
  int marked;
  int found;
  struct strip strip_copy;
  int n;

  IFF(mark_strip
      (beside, row, col, save_row, size, &marked, world->cols, 1, world));
  if (marked)
    {
      IFF(copy_strip(&strip_copy, &(world->rows[beside]), world));
      IFF(find_min_starts
	  (beside, world->number_cols, &strip_copy, &found, world));
    }
  for (n SET_TO 0; n < size; n++)
    put_square(beside, (col + n), save_row[n], world);
  if (marked AND found)
    *is_empty SET_TO 0;
  return OK;
}

/*************************************************************************/

/* try_edges

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. try_edge_row returns ERROR.
  2. try_edge_col returns ERROR.
  3. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: Any squares along an edge that can be proved not fillable
are marked empty.

Notes:

An edge is a strip for which either all preceding strips are done or all
following strips are done.

This proceeds square by square along the edge, looking at blank squares.
For each blank square, it finds the size of the smallest block that might
cover the square and tries to cover the square with a block of that size
in all possible positions. For each position, it determines how the
squares in the next strip over (towards the inside of the puzzle) must
be marked and determines if that is a possible marking for that strip.
If all block positions give impossible markings for the next strip over,
the square in question must be empty, so it is marked empty.

Example

In the diagram below, the edge is the first row of the puzzle and the
square being tested is marked with a question mark.

     12121
  -----------
3|     ?
2|

The square must be covered by a block of size 3, which is possible three
ways, as shown below with Xs. For each position of the block, other
squares must be filled or empty, as shown by Ys and dots.

     12121              12121              12121
  -----------        -----------        -----------
3|     XXX         3|    XXX          3|   XXX
2|     .Y.         2|    Y.Y          2|   .Y.

In all three positions of the XXX block, an illegal condition exists
in the second row. Therefore, the position with the question mark must
be empty.

*/

int try_edges(               /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "try_edges";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  world->number_in_cols SET_TO 0;
  for (row SET_TO 0; row < (world->number_rows - 1); row++)
    {
      if ((world->rows[row].undone) AND
	  ((row IS 0) OR (NOT (world->rows[row - 1].undone))))
	{
	  IFF(try_edge_row(row, (row + 1), world));
	  break;
	}
    }
  for (col SET_TO (world->number_cols - 1); col > 0; col--)
    {
      if ((world->cols[col].undone) AND
	  ((col IS (world->number_cols - 1)) OR
	   (NOT (world->cols[col + 1].undone))))
	{
	  IFF(try_edge_col(col, (col - 1), world));
	  break;
	}
    }
  for (row SET_TO (world->number_rows - 1); row > 0; row--)
    {
      if ((world->rows[row].undone) AND
	  ((row IS (world->number_rows - 1)) OR
	   (NOT (world->rows[row + 1].undone))))
	{
	  IFF(try_edge_row(row, (row - 1), world));
	  break;
	}
    }
  for (col SET_TO 0; col < (world->number_cols - 1); col++)
    {
      if ((world->cols[col].undone) AND
	  ((col IS 0) OR (NOT (world->cols[col - 1].undone))))
	{
	  IFF(try_edge_col(col, (col + 1), world));
	  break;
	}
    }
  IFF(record_progress("mark empty edge squares in ", world));
  return OK;
}

/*************************************************************************/

/* try_empty_no_room

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. mark_empty_strip2 returns ERROR.
  2. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: squares of the puzzle certain to be empty because they
are part of a blank block between two empty squares that is too small
to fit any available patch are marked empty.

*/

int try_empty_no_room(        /* ARGUMENTS    */
 struct paint_world * world)  /* puzzle model */
{
  const char * name SET_TO "try_empty_no_room";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  world->use_rows SET_TO 1;
  for (row SET_TO 0; row < world->number_rows; row++)
    if (world->rows[row].undone)
      IFF(mark_empty_strip2
	  (row, world->number_cols, &(world->rows[row]), world));
  world->number_in_cols SET_TO 0;
  world->use_rows SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    if (world->cols[col].undone)
      IFF(mark_empty_strip2
	  (col, world->number_rows, &(world->cols[col]), world));
  IFF(record_progress("mark empty squares where no room in ", world));
  return OK;
}

/*************************************************************************/

/* try_empty_outside

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. mark_empty_strip returns ERROR.
  2. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: squares of the puzzle certain to be empty because they
lie outside the bounds of any patch in a strip are marked empty.

*/

int try_empty_outside(       /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "try_empty_outside";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  world->use_rows SET_TO 1;
  for (row SET_TO 0; row < world->number_rows; row++)
    if (world->rows[row].undone)
      IFF(mark_empty_strip
	  (row, world->number_cols, &(world->rows[row]), world));
  world->number_in_cols SET_TO 0;
  world->use_rows SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    if (world->cols[col].undone)
      IFF(mark_empty_strip
	  (col, world->number_rows, &(world->cols[col]), world));
  IFF(record_progress("mark surely empty squares in ", world));
  return OK;
}

/*************************************************************************/

/* try_full_bounded

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. mark_patches_strip returns ERROR.
  2. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: The start and end of each patch of each strip (row or
column) are marked. Any squares between the start and end of a patch
that must be full are marked full.

*/

int try_full_bounded(        /* ARGUMENTS      */
 struct paint_world * world) /* puzzle model   */
{
  const char * name SET_TO "try_full_bounded";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  world->use_rows SET_TO 1;
  for (row SET_TO 0; row < world->number_rows; row++)
    if (world->rows[row].undone)
      {
	IFF(mark_patches_strip
	    (row, world->number_cols, &(world->rows[row]), world));
      }
  world->number_in_cols SET_TO 0;
  world->use_rows SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    if (world->cols[col].undone)
      {
	IFF(mark_patches_strip
	    (col, world->number_rows, &(world->cols[col]), world));
      }
  IFF(record_progress("mark surely full squares and empties in ", world));
  return OK;
}

/*************************************************************************/

/* try_guessing

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. find_best_patch returns ERROR.
  2. copy_world returns ERROR.
  3. mark_patches_strip returns ERROR.
  4. record_progress returns ERROR.
  5. put_logic returns ERROR.

Called By:  solve_problem

Side Effects:

This finds the best patch to guess. Then it makes copies of the
world with that patch done in each possible position of the block
in the patch. It tries to solve each copy.

A position of the block in the patch will not be possible if any of
the following obtains:
a. Any of the squares where the block would go is marked empty.
b. The square just before or just after where the block would go is full.

When a guess is chosen, it does not suffice to just mark X's here and
let the other try_ functions figure out which patch was marked,
because it may be impossible to determine which patch it was.

The following case of a row of the puzzle occurred.  1 3 1 1|     .X.. |
The last patch was chosen as the best patch by find_best_patch.
There are two possibilities for the location of the last patch (i) on
the X, (ii) on the blank just before the end of the row. The last patch
was chosen as the patch to try, and X was re-Xed as the first of the
two guesses. This left the puzzle unchanged, so the last patch was
chosen again on the next try. And so on, so every subsequent guess was
the same.

A row could be  1 1 1|  .X.X.  | so that the next-to-last patch
must be on one of the X's, but it is not possible to tell which. In this
case, marking either place the patch might be changes nothing.

Because of the above problem, when a guess is used, try_guessing must
mark the patch done and reset the ends.

To insert a guess, this only moves the start and end of the patch for
which the guess is being made. When solve_problem runs again,
try_full_bounded will insert X's if needed. This means that the
appearance of the puzzle does not change when it is announced that a
guess has been made.

The "many" variable is decremented after being used so that the printed
number is an estimate of the size of the space remaining to be searched.

*/

int try_guessing(            /* ARGUMENTS      */
 struct paint_world * world) /* puzzle model   */
{
  const char * name SET_TO "try_guessing";
  int strip_index; /* index in rows or cols of strip selected for guessing  */
  int is_row;             /* set to 1 if selected strip is row, to 0 if col */
  int patch_index;        /* index in strip of patch selected for guessing  */
  struct patch * a_patch; /* the patch selected for guessing                */
  struct paint_world * world_copy;
  int stop;
  int result;
  int return_value;
  int start;
  int n;
  int shifted;
  int many;           /* number of remaining choices of where to put guess */
  char buffer[TEXT_SIZE];

  return_value SET_TO ERROR;
  world_copy SET_TO (worlds + world->depth + 1);
  IFF(find_best_patch(&strip_index, &is_row, &patch_index, &many, world));
  if (is_row)
    a_patch SET_TO &(world->rows[strip_index].patches[patch_index]);
  else
    a_patch SET_TO &(world->cols[strip_index].patches[patch_index]);
  stop SET_TO ((a_patch->end + 2) - a_patch->target);
  for (start SET_TO a_patch->start; start < stop; start++)
    {
      if (is_row)
	{
	  world->use_rows SET_TO 1;
	  for (n SET_TO 0; n < a_patch->target; n++)
	    if (get_square(strip_index, (start + n), world) IS '.')
	      break;
	  if ((n ISNT a_patch->target) OR
	      (get_square(strip_index, (start - 1), world) IS 'X') OR
	      (get_square(strip_index, (start + n), world) IS 'X'))
	    continue;
	  IFF(copy_world(world_copy, world));
	  world_copy->number_in_rows SET_TO 0;
	  world_copy->number_in_cols SET_TO 0;
	  IFF(shift_patches_right(patch_index, start, world->number_cols,
				  &shifted, &(world_copy->rows[strip_index])));
	  if (NOT shifted)
	    continue;
	  IFF(shift_patches_left(patch_index, (start + (a_patch->target - 1)),
				 &shifted, &(world_copy->rows[strip_index])));
	  if (NOT shifted)
	    continue;
	  IFF(put_logic(strip_index, world_copy));
	  world_copy->use_rows SET_TO 0;
	  IFF(put_logic(start, world_copy));
	  world_copy->use_rows SET_TO 1;
	  sprintf
	    (buffer, "guess block %d of row starts at ", (patch_index + 1));
	}
      else
	{
	  world->use_rows SET_TO 1;
	  for (n SET_TO 0; n < a_patch->target; n++)
	    if (get_square((start + n), strip_index, world) IS '.')
	      break;
	  if ((n ISNT a_patch->target) OR
	      (get_square((start - 1), strip_index, world) IS 'X') OR
	      (get_square((start + n), strip_index, world) IS 'X'))
	    continue;
	  IFF(copy_world(world_copy, world));
	  world_copy->number_in_rows SET_TO 0;
	  world_copy->number_in_cols SET_TO 0;
	  IFF(shift_patches_right(patch_index, start, world->number_rows,
				  &shifted, &(world_copy->cols[strip_index])));
	  if (NOT shifted)
	    continue;
	  IFF(shift_patches_left(patch_index, (start + (a_patch->target - 1)),
				 &shifted, &(world_copy->cols[strip_index])));
	  if (NOT shifted)
	    continue;
	  IFF(put_logic(start, world_copy));
	  world_copy->use_rows SET_TO 0;
	  IFF(put_logic(strip_index, world_copy));
	  world_copy->use_rows SET_TO 1;
	  sprintf
	    (buffer, "guess block %d of column starts at ", (patch_index + 1));
	}
      world_copy->progress_puzzle SET_TO 1;
      world_copy->depth SET_TO (world->depth + 1);
      world_copy->space SET_TO (world->space * many);
      many--;
      printf("%d %.0lf\n", world_copy->depth, world_copy->space);
      IFF(record_progress(buffer, world_copy));
      IFF(balanced(world_copy));
      result SET_TO solve_problem(world_copy);
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

/* try_one

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. try_one_strip returns ERROR.
  2. record_progress returns ERROR.

Called By:  solve_problem

Side Effects: For each strip of the puzzle, if a blank square in the
strip cannot be full, it is marked empty, and if it cannot be empty,
it is marked full.

Notes:

This is a time-consuming procedure, since every blank square of the
puzzle is likely to be tested four times (row and column, full or
empty) and the entire strip is copied and modified for each test.

*/

int try_one(                 /* ARGUMENTS    */
 struct paint_world * world) /* puzzle model */
{
  const char * name SET_TO "try_one";
  int row;
  int col;

  world->number_in_rows SET_TO 0;
  world->use_rows SET_TO 1;
  for (row SET_TO 0; row < world->number_rows; row++)
    if (world->rows[row].undone)
      IFF(try_one_strip(row, world->number_cols, &(world->rows[row]), world));
  world->number_in_cols SET_TO 0;
  world->use_rows SET_TO 0;
  for (col SET_TO 0; col < world->number_cols; col++)
    if (world->cols[col].undone)
      IFF(try_one_strip(col, world->number_rows, &(world->cols[col]), world));
  IFF(record_progress("mark single empty or full squares in ", world));
  return OK;
}

/*************************************************************************/

/* try_one_strip

Returned Value: int
  If any of the following errors occur, this returns ERROR.
  Otherwise, it returns OK.
  1. copy_strip returns ERROR.
  2. find_min_starts returns ERROR.
  3. mark_empty_square returns ERROR.
  4. enx_square returns ERROR.
  5. put_logic returns ERROR.

Called By:  try_one

Side Effects: In a_strip, each blank square that cannot be full
(because a_strip cannot be covered by its patches if it is full)
is marked empty. Similarly, in a_strip, each square that cannot be
empty (because a_strip cannot be covered by its patches if it is
empty) is marked full.

Notes:

This assumes without testing that if a square cannot be full it must
be empty, and conversely. Thus, ill-formed puzzles where neither full
nor empty works for a square will not be detected here.

mark_empty_square requires square to be blank previously in "if(NOT found)".
enx_square requires square to be blank previously in second "if(NOT found)".

*/

int try_one_strip(           /* ARGUMENTS                             */
 int index1,                 /* index of the strip containing a_strip */
 int stop,                   /* length of a_strip                     */
 struct strip * a_strip,     /* the strip being tested                */
 struct paint_world * world) /* puzzle model                          */
{
  const char * name SET_TO "try_one_strip";
  int index2;
  struct strip test_strip;
  char item;
  int found;

  world->progress_strip SET_TO 0;
  for (index2 SET_TO 0; index2 < stop; index2++)
    {
      item SET_TO get_square(index1, index2, world);
      if (item IS ' ')
	{
	  IFF(copy_strip(&test_strip, a_strip, world));
	  put_square(index1, index2, 'X', world);
	  IFF(find_min_starts(index1, stop, &test_strip, &found, world));
	  if (NOT found)
	    {
	      put_square(index1, index2, ' ', world);
	      IFF(mark_empty_square(index1, index2, world));
	    }
	  else
	    {
	      IFF(copy_strip(&test_strip, a_strip, world));
	      put_square(index1, index2, '.', world);
	      IFF(find_min_starts(index1, stop, &test_strip, &found, world));
	      if (NOT found)
		{
		  put_square(index1, index2, ' ', world);
		  IFF(enx_square(index1, index2, world));
		}
	      else
		put_square(index1, index2, ' ', world);
	    }
	}
    }
  if (world->progress_strip)
    IFF(put_logic(index1, world));
  return OK;
}

/*************************************************************************/

