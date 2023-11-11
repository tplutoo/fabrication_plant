#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#define MAX 25

int buffer_blue[MAX];
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;

int main() {
  FILE *file;
  int buffer;

  file = fopen("railwayCars.txt", "r");

/*
  while (fscanf(file, "%d", &number) == 1) {
    count++;
  }

  printf("The file contains %d numbers.\n", count);
*/

  while (count < MAX && fscanf(file, "%d", &buffer_blue[count]) == 1) {
      count++;
  }

  for (int i = 0; i < 25; i++) {
    printf("%d ", buffer_blue[i]);
  }

  fclose(file);
  return 0;

  /* thread initialization, creation, destroy
  int i_empty = pthread_cond_init(&empty, NULL);
  assert(i_empty == 0);
  int i_fill = pthread_cond_init(&fill, NULL);
  assert(i_fill == 0);
  int rc = pthread_mutex_init(&mutex, NULL);
  assert(rc == 0);

  pthread_create(&p1, NULL, producer, NULL);
  pthread_create(&c1, NULL, consumer, NULL);
  //pthread_create(&c2, NULL, consumer, NULL);

  pthread_join(p1, NULL);


  pthread_cond_destroy(&empty);
  pthread_cond_destroy(&fill);
  pthread_mutex_destroy(&mutex);
  */
}
