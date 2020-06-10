#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int i;
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) { 
                printf("seed is a positive number\n");
                return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) { 
                printf("array_size is a positive number\n");
                return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) { 
                printf("pnum is a positive number\n");
                return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);
         
    int pipes_num = pnum * 2;
    int fd[pipes_num][2];    
       
    int step = array_size/pnum;

    
    if (!with_files) {

        for (i = 0; i < pipes_num; i++) {
            if (pipe(fd[i])==-1)
            {
                printf("Pipe Failed");
                return 1;
            } 
        }
    } else {
        
        FILE* cfp;
        cfp = fopen("file.txt", "w+");
        fprintf(cfp, "");
        fclose(cfp);
    }

  for (i = 0; i < pnum; i++) {
    pid_t child_pid = fork();   
    
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
          int begin = i * step;
          int end = (i + 1) * step;                
          if (i == pnum -1) { end = array_size - 1;}
          struct MinMax min_max = GetMinMax(array, begin, end);

        if (with_files) {
          // use files here
            FILE * fp;
            fp = fopen ("file.txt", "a+");
            fprintf(fp, "%d\n%d\n", min_max.min, min_max.max);            
            fclose (fp);


        } else {
          // use pipe here
                    int current_min_pipe = i * 2;
                    int current_max_pipe = i * 2 + 1;                    
                    close(fd[current_min_pipe][0]);
                    close(fd[current_max_pipe][0]);                    
                    char str_min_num[255];
                    char str_max_num[255];
                    sprintf (str_min_num, "%d\n", min_max.min);
                    sprintf (str_max_num, "%d\n", min_max.max);
                    write(fd[current_min_pipe][1], str_min_num, strlen(str_min_num)+1);
                    write(fd[current_max_pipe][1], str_max_num, strlen(str_max_num)+1);                    
                    close(fd[current_min_pipe][1]);
                    close(fd[current_max_pipe][1]);
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    // your code here
    int status = -1;
    waitpid(-1, &status, 0);

    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
  FILE* fp;
  fp = fopen("file.txt", "r");

  for (i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;    

    if (with_files) {
      // read from files           
            char min_str[255];
            char max_str[255];
            fscanf(fp, "%s", min_str);
            fscanf(fp, "%s", max_str);
            min = atoi(min_str);
            max = atoi(max_str);         
            if (i == pnum - 1) { fclose(fp); }
    } else {
            int current_min_pipe = i * 2;
            int current_max_pipe = i * 2 + 1;            
            close(fd[current_min_pipe][1]);
            close(fd[current_max_pipe][1]);          
            char min_str[255];
            char max_str[255];
            read(fd[current_min_pipe][0], min_str, 255);
            read(fd[current_max_pipe][0], max_str, 255);
            min = atoi(min_str);
            max = atoi(max_str);
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
