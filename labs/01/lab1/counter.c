/*
 * Add a top level comment:
 * (1) high-level summary of what this module does
 * (2) your names and date
 *
 */
#include <stdio.h>
#include "counter.h"

// global variable declaration and initialization
// You do not need to initialize global variables
// when you declare them, and you can only initialize then
// to a scalar value in their declaration.
int my_counter=0;  

// "static" means that the scope of the global variable is limited to 
// this module (.c file).  This means that other .c files could have 
// a static  variable or function also named private_val, and  there 
// will be no name conflict with this one.  It is good practice to 
// declare all functions and globals variables that are not shared between 
// modules to be static.
static int private_val;

// a function prototype for a function defined later in this module. This
// is a static function, so its prototype should not appear in a .h file
static void update_old_val();

/*
 * This is an example global function definition that modifies a global 
 * variable shared by two modules (main.c and counter.c)
 * notice how the global is declared in main.c only.
 * notice how the function only used within main.c is declared static
 */
int inc_and_return_count() {
  update_old_val();
  my_counter++;
  return my_counter;
}

/*
 * an example of a static function
 * (a function whose scope is limited to this .c file)
 */
static void update_old_val() {
  private_val = my_counter;
}
