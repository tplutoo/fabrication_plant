
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#define MAX 50
#define MAX_L 25
#define MAX_R 15
#define BLUE_MAX 15
#define RED_MAX 10

FILE *file = NULL;
FILE *file_blue = NULL;
FILE *file_red = NULL;

int buffer_blue[BLUE_MAX];
int buffer_red[RED_MAX];

int retrieved_number = 0;

int fill_ptr_b = 0;
int fill_ptr_r = 0;

int use_ptr_bl = 0;
int use_ptr_rl = 0;

int b_count = 0;
int r_count = 0;

int done = 0;

int count_threadl_iteration = 0;
int count_threadr_iteration = 0;


pthread_cond_t b_empty,r_empty, b_filled, r_filled;
pthread_mutex_t mutex, mutex_f;

void put_l() {
  if (retrieved_number > 0 && retrieved_number <= 12) {
      while (b_count == MAX ) {
        pthread_cond_wait(&b_empty, &mutex);
      }
      buffer_blue[fill_ptr_b] = retrieved_number;
      fill_ptr_b = (fill_ptr_b + 1) % BLUE_MAX;
      b_count++;
      printf("..ThreadL moves part (%d) to BLUE [conveyor belt]..\n", retrieved_number);
      pthread_cond_signal(&b_filled);
  } else if (retrieved_number > 12 && retrieved_number <= 25) {
      while (r_count == MAX) {
        pthread_cond_wait(&r_empty, &mutex);
      }
      printf("..ThreadL moves part (%d) to RED [conveyor belt]..\n", retrieved_number);
      buffer_red[fill_ptr_r] = retrieved_number;
      fill_ptr_r = (fill_ptr_r + 1) % RED_MAX;
      r_count++;
      pthread_cond_signal(&r_filled);
  }
}

void *producer_threadl(void *arg) {
  while (1) {
  for (int i = 0; i< MAX_L ;i ++) {
    pthread_mutex_lock(&mutex_f);
    if (fscanf(file, "%d", &retrieved_number)==EOF) {
      if (feof(file)) {
        printf("\nThreadL has reached end of parts\n");
      }
      if (ferror(file)) {
        perror("Error rseading from file, ThreadL");
      }
      done = 1;
      pthread_cond_signal(&b_filled);
      pthread_cond_signal(&r_filled);
      pthread_mutex_unlock(&mutex_f);
      pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutex_f);

    pthread_mutex_lock(&mutex);
    put_l();
    pthread_mutex_unlock(&mutex);
    usleep(250000);
  }
  printf(".ThreadL finished iteration %d after 25 parts.\n\n", count_threadl_iteration++);
  }
  pthread_exit(NULL);
}

void put_r() {
  if (retrieved_number > 0 && retrieved_number <= 12) {
      while (b_count == MAX) {
        pthread_cond_wait(&b_empty, &mutex);
      }
      buffer_blue[fill_ptr_b] = retrieved_number;
      fill_ptr_b = (fill_ptr_b + 1) % BLUE_MAX;
      b_count++;
      printf("..ThreadR moves part (%d) to BLUE [conveyor belt]..\n", retrieved_number);
      pthread_cond_signal(&b_filled);
  } else if (retrieved_number > 12 && retrieved_number <= 25) {
      while (r_count == MAX) {
        printf("r_count == MAX, producer_l waiting\n");
        pthread_cond_wait(&r_empty, &mutex);
      }
      buffer_red[fill_ptr_r] = retrieved_number;
      fill_ptr_r = (fill_ptr_r + 1) % RED_MAX;
      r_count++;
      printf("..ThreadR moves part (%d) to RED [conveyor belt]..\n", retrieved_number);
      pthread_cond_signal(&r_filled);
  }
}

void *producer_threadr(void *arg) {
  while (1) {
  pthread_mutex_lock(&mutex_f);
  for (int i = 0; i < MAX_R; i++){
    if (fscanf(file, "%d", &retrieved_number)==EOF) {
      if (feof(file)) {
        printf("\nThreadR has reached end of parts\n");
      }
      if (ferror(file)) {
        perror("Error reading from file, ThreadR");
      }
      done = 1;
      pthread_cond_signal(&b_filled);
      pthread_cond_signal(&r_filled);
      pthread_mutex_unlock(&mutex_f);
      pthread_exit(NULL);

    }
    pthread_mutex_unlock(&mutex_f);

    pthread_mutex_lock(&mutex);
    put_r();
    pthread_mutex_unlock(&mutex);
    usleep(500000);
  }
  printf(".ThreadR finished iteration %d after 15 parts.\n\n", count_threadr_iteration++);
  }
  pthread_exit(NULL);
}


int get_b() {
    int temp_b = buffer_blue[use_ptr_bl];
    printf("...moving part (%d) to BLUE [delivery truck]...\n", temp_b);
    use_ptr_bl = (use_ptr_bl + 1) % BLUE_MAX;
    b_count--;
    return temp_b;
}

int get_r() {
    int temp_r = buffer_red[use_ptr_rl];
    printf("...moving part (%d) to RED [delivery truck]...\n", temp_r);
    use_ptr_rl = (use_ptr_rl + 1) % RED_MAX;
    r_count--;
    return temp_r;
}

void *consumer_b(void *arg) {
    while (done == 0) {
      pthread_mutex_lock(&mutex);
      while (b_count == 0 && done == 0) {
          pthread_cond_wait(&b_filled, &mutex);
      }
      if (b_count != 0) {
        fprintf(file_blue, "%d ", get_b());
      }
      pthread_cond_signal(&b_empty);
      pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

void *consumer_r(void *arg) {
    while (done == 0) {
      pthread_mutex_lock(&mutex);
      while (r_count == 0 && done == 0) {
          pthread_cond_wait(&r_filled, &mutex);
      }
      if (r_count != 0) {
        fprintf(file_red, "%d ", get_r());
      }
      pthread_cond_signal(&r_empty);
      pthread_mutex_unlock(&mutex);
  }
  return NULL;
}


int main(int argc, char *argv[]){
    if (argc != 4) {
        exit(EXIT_FAILURE);
    }

  int railway_fd = atoi(argv[1]);
  int blue_truck_fd = atoi(argv[2]);
  int red_truck_fd = atoi(argv[3]);

  file = fdopen(railway_fd, "r");
  file_blue = fdopen(blue_truck_fd, "wb");
  file_red = fdopen(red_truck_fd, "wb");

  int i_b_empty = pthread_cond_init(&b_empty, NULL);
  assert(i_b_empty == 0);

  int i_r_empty = pthread_cond_init(&r_empty, NULL);
  assert(i_r_empty == 0);

  int i_fill_cb = pthread_cond_init(&b_filled, NULL);
  assert(i_fill_cb == 0);

  int i_fill_cr = pthread_cond_init(&r_filled, NULL);
  assert(i_fill_cr == 0);

  int rc = pthread_mutex_init(&mutex, NULL);
  assert(rc == 0);

  int rc_f = pthread_mutex_init(&mutex_f, NULL);
  assert(rc_f == 0);

  pthread_t p1, p2, c1, c2;

  pthread_create(&c1, NULL, consumer_b, NULL);
  pthread_create(&c2, NULL, consumer_r, NULL);
  pthread_create(&p1, NULL, producer_threadr, NULL);
  pthread_create(&p2, NULL, producer_threadl, NULL);

  pthread_join(c1, NULL);
  pthread_join(c2, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);

  pthread_cond_destroy(&b_empty);
  pthread_cond_destroy(&r_empty);

  pthread_cond_destroy(&r_filled);
  pthread_cond_destroy(&b_filled);
  pthread_mutex_destroy(&mutex);
  pthread_mutex_destroy(&mutex_f);

  fclose(file);
  fclose(file_blue);
  fclose(file_red);

  printf("\nFactory operation has been completed!\n");

  return 0;

}
