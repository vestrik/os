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

int main(void)
{
   pid_t child_pid = fork();
   if (child_pid==0)
   {
       execl("seq" , "seq" , "10","50", NULL );
       return 0;
      
   }
   else
   {
   int status = -1;
    waitpid(-1, &status, 0);
   }
}