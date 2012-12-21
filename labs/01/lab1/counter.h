// .h files should contain function prototype, 
// typedefs, and global variable defs shared 
// by multiple modules (.c files) 

// around the contents of a .h file is bolierplate
// ifndef-define-endif preprocessor directive.
// this will ensure that the contents of this .h file
// are only expanded into a .c file one time during
// the preprocessing phase of compilation, thus avoiding
// "multiple definition" compilation errors
#ifndef __COUNTER_H__
#define __COUNTER_H__

// "extern" defines the type and existance of a global variable
// or function that is declared and defined in exactly one .c
// file.  An extern definition is different from a global
// variable declaration that would allocate space for a an
// int-sized variable named counter.  Using extern before a
// global variable or function prototype in a .h files
// tells the compiler type information about a global names.
// With this type information, modules other than the single 
// one with the global variable declaration or the function
// definition can use these global names.
extern int my_counter;

// a function prototype for a function defined in one
// .c file, but can be called from any .c file that
// includes this .h file
extern int inc_and_return_count();


#endif
