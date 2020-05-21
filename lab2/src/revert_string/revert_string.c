#include "revert_string.h"
#include <string.h>

void RevertString(char *str)
{
    int k= (strlen(str)-1);    
    char s [k];         
	for (int i=0;i<k+1;i++)
    {
        s[i]=str[(k-i)];        

    }   
   strcpy(str,s); 
}