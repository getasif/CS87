/* 
 * Simple pthreads program with a race condition
 * (newhall, 2010)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_hello(void *arg);

static int count;
static pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv) {

    pthread_t tids[10];
    int ret, i, *my_val;

    count = 0;
    for (i=0; i < 10; i++) {
        my_val = malloc(sizeof(int));
        *my_val = i;
        ret = pthread_create(&tids[i], 0, thread_hello, my_val);

        // try this call to pthread_create, run a few times and
        // see what happens to the value passed in as the arg 
        // (do you understand what is going on?):
        // ret = pthread_create(&tids[i], 0, thread_hello, &i);
    }
    for (i=0; i < 10; i++) {
        pthread_join(tids[i], 0);
    }
    printf("count = %d\n", count);
    exit(0);
}

// thread main loop
// arg: int value specifying a logical tid value
void *thread_hello(void *arg) {

  int x, i;
  x = *((int *)arg);
  printf("hello I'm thread %d with tid %lu\n", x, pthread_self());

  // RACE CONDITION:  try running this program a few times,
  //                  you should see different final results
  for(i = 0; i < 1000000; i++) {
    if(i%2 == 0) { 
      count += x;
    }else {
      count += 2*x;
    }
  }

  // FIX TO RACE CONDITION:
  // add mutex lock and unlock around count access in above
  // code to avoid race condition:
  //  pthread_mutex_lock(&my_mutex);
  //  count = x; 
  //  pthread_mutex_unlock(&my_mutex);
  //
  
  free ((int *)arg);
  return (void *)0;
}

