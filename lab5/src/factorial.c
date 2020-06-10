#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include <getopt.h>



pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

long long int total_fac=1;

long long int mod;


typedef struct FactorialArgs {
   int begin;
   int end;
   long long int res;

} thread_data;

void *Factorial(void *arg)
{  
    long long int result=1;

   thread_data *tdata=(thread_data *)arg;   
   
   //printf("begin %d end %d\n",tdata->begin,tdata->end);
   for (int i = tdata->begin; i < tdata->end; i++)
    {
        
        result*=(i+1);
       // printf("i=%d, %lli\n",i,result);
        
    }

   tdata->res=result;
   //printf("res thread %lli\n",tdata->result);
   
  
   pthread_exit(NULL);
}



int main(int argc, char **argv) {
  long long int k = -1;
  int threads_num = -1;

  while (1) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"threads_num", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            k = atoi(optarg);
            // your code here
            // error handling
            if (k <= 0)
            {
                printf("k must be a positive number");
                return 1;
            }
            break;
          case 1:
            mod = atoi(optarg);
            // your code here
            // error handling
            if(mod <= 0)
            {
                printf("Mod must be a positive number");
                return 1;
            }
            break;
          case 2:
            threads_num = atoi(optarg);
            // your code here
            // error handling
            if(threads_num <= 0)
            {
                printf("Number of threads must be a positive");
                return 1;
            }
            break;
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
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

  if (mod == -1 || k == -1 || threads_num == -1) {
    printf("Usage: %s --k \"num\" --threads_num \"num\" --mod \"num\" \n",
           argv[0]);
    return 1;
  }
  pthread_t threads[threads_num];
  struct FactorialArgs args[threads_num];
  int part = k/threads_num;  
  
  for (int i = 0; i < threads_num; i++)
  {

    args[i].begin = part * i;
    args[i].end = args[i].begin + part;  
  }
  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, (void*)Factorial, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }
  
  total_fac=1;  
  for (uint32_t i = 0; i < threads_num; i++) 
  { 
    pthread_join(threads[i], NULL);   
    printf(" = %lli\n",  args[i].res);
    total_fac*=args[i].res; 
   
  }
  printf("factorial %lli\n",total_fac); 
  
  return 0;
}