#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

FILE *file = NULL;
FILE *file_blue=NULL;
FILE *file_red=NULL;

int main(int argc, char *argv[]) {
    printf("Welcome to Factory!\n");
    printf("Services will be starting shortly\n\n");
    sleep(1);

    FILE *file = fopen("railwayCars.txt", "r");
    FILE *file_blue = fopen("blue_delievery.txt", "wb");
    FILE *file_red = fopen("red_delievery.txt", "wb");

    if (file == NULL || file_blue == NULL || file_red == NULL) {
        perror("Error opening file");
        return 1;
    }

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
      char fd_railway_str[16];
      char fd_blue_deliv_str[16];
      char fd_red_deliv_str[16];

      snprintf(fd_railway_str, sizeof(fd_railway_str), "%d", fileno(file));
      snprintf(fd_blue_deliv_str, sizeof(fd_blue_deliv_str), "%d", fileno(file_blue));
      snprintf(fd_red_deliv_str, sizeof(fd_red_deliv_str), "%d", fileno(file_red));

      char *myargs[] = {"./assembly_manager", fd_railway_str, fd_blue_deliv_str, fd_red_deliv_str, NULL};
      execvp(myargs[0], myargs);
    } else {
        int rc_wait = wait(NULL);
        printf("\nFactory operation has been completed!\n");
      }
    return 0;
}
