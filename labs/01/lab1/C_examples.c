/*
 * an example illustrating some differences between C and C++
 *
 * Look at my on-line C references off my help pages for more
 * information about pointers, strings, linked structs in C
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// C has structs and not classes
typedef struct student_struct {
  char name[20];
  int  age;
} student_t;

int main(int argc, char *argv[]) {

  int i;
  int *num_array, *next;
  char *message;
  student_t *students, *next_student;

  // (1) C style dynamic memory allocation
  
  // allocate an an array of 10 ints
  num_array = (int *)malloc(sizeof(int)*10);  

  // allocate char array (a C string) into which we will copy
  // the string hello (need to allocate space for trailing '\0' char)
  message = (char *)malloc(sizeof(char)*(strlen("hello")+1)); 
  
  // allocate an array of 3 student structs
  students = (student_t *)malloc(sizeof(student_t)*3);  

   // check return values of all sys calls
   // NULL and 0 are the same value, and 0 evaluates to false, all non-zero
   // to true, so often times you see expressions like (!num_array)
   // or (num_array != 0) in place of  (num_array != NULL)
  if(!num_array || !message || !students) { 
        perror("malloc");   // perror is a useful function for error handling 
        exit(1);
  }

  // (2) an example use of C string library functions
  //     strcpy null terminates the destination string for us
  //     caller needs to allocate space for the destination (message)
  printf("STRINGS ***************\n");
  if(strcpy(message, "hello") == 0) {
      printf("strcpy failed\n");
      exit(1);
  }
  printf(" %s\n", message);

  // (2a) C array syntax 
  
  printf("STRINGS AS ARRAYS  ************\n");
  // change C-style string and int array using array indexing
  for (i=0; i < 3; i++) {
    message[i] = 'a' + i;
  }
  //oops, we forgot to null terminate the string
  printf(" bad abc's: %s\n", message); 

  message[3] = '\0';  // null terminate it and try again
  printf(" fixed abc's: %s\n", message);

  printf("ARRAYS  ************\n");
  for (i=0; i < 10; i++) {
     num_array[i] = i;
  }
  for (i=0; i < 5; i++) {
     printf(" %d,",  num_array[i]);
  }
  printf("\n");

  printf("STRUCTS ***************\n");

  // (3) structs, arrays of structs
  for (i=0; i < 3; i++) {
    students[i].age = 18 + i; 
    strcpy(students[i].name, "mookie");
    if(i > 0) {
       students[i].name[0] = 'p'+i;
    }
  }

  // access field values using .
  for (i=0; i < 3; i++) {
    printf(" student %d: %s, %d\n", i, students[i].name, students[i].age);
  }

  // accessing values through pointers
  printf("POINTERS  ************\n");

  // lets make next point to the first value in the numbers array
  next = num_array;   // equivalent to next = &(num_array[0])

  printf(" next = %d\n", *next);  // dereference next using *

  // can use pointer arithmetic to make next point to the next value
  // in num_array (there is really no reason to using pointer arithmetic
  // instead of array syntax here, but here it is anyway)
 
  next++;  // make next point to the very next int
  printf(" the next value = %d\n", *next);

  // pointers to structs
  next_student = students;

  // dereference fields in struct pointed to by next_student using ->
  printf(" first student name %s age %d\n", 
      next_student->name, next_student->age);

  // use pointer arithmetic to print out all student values
  for (i=0; i < 3; i++) { 
    printf(" name: %s age: %d\n", next_student->name, next_student->age);
    next_student++;  
  }

  // at this point next_student points to garbage values beyond the 
  // bounds of the array of student structs we allocated
  // to see some garbage, uncomment:
  // printf("GARBAGE: name %s age %d\n", next_student->name, next_student->age);


  // (4) free malloced space when done...else your program may have
  //     have memory leaks (allocated heap space that is not accessable
  //     through any program variable)
  
  free(num_array);
  free(message);
  free(students);

  // setting pointers to NULL after free'ing the memory they point to
  // will ensure that you get a segfault if you try to dereference them
  // later in your program...this is desired behavior because you will 
  // be able to see illegal heap memory references this way (rather than 
  // seeing some strange memory errors)
  num_array  = NULL;
  message = NULL;
  students = NULL;
  next = NULL;
  next_student = NULL;

  exit(0);
}
