// shell.c
// shell sort routines
#include "sort.h"

void inline shellSort(DATA_TYPE numbers[], int array_size)
{
  int i, j, increment; 
  DATA_TYPE temp;

  increment = array_size / 2;
  while (increment > 0)
  {
    for (i=0; i < array_size; i++)
    {
      j = i;
      temp = numbers[i];
      while ((j >= increment) && (numbers[j-increment] > temp))
      {
        numbers[j] = numbers[j - increment];
        j = j - increment;
      }
      numbers[j] = temp;
    }  
 
    if (increment == 2)
       increment = 1;
    else 
       increment = (int) (increment / 2.2);
  }
}


