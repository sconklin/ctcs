// heap.c
// heap sort routines
#include "sort.h"

void inline heapSort(DATA_TYPE numbers[], const int array_size)
{
	int i;
	DATA_TYPE temp;

	for (i = (array_size / 2)-1; i >= 0; i--)
		siftDown(numbers, i, array_size);
	
	for (i = array_size-1; i >= 1; i--)
	{
		temp = numbers[0];
		numbers[0] = numbers[i];
		numbers[i] = temp;
		siftDown(numbers, 0, i-1);
	}
}


void inline siftDown(DATA_TYPE numbers[], int root, int bottom)
{
  size_t done, maxChild;
  DATA_TYPE temp;

  done = 0;
  while ((root*2 <= bottom) && (!done))
  {
    if (root*2 == bottom)
      maxChild = root * 2;
    else if (numbers[root * 2] > numbers[root * 2 + 1])
      maxChild = root * 2;
    else
      maxChild = root * 2 + 1;

    if (numbers[root] < numbers[maxChild])
    {
      temp = numbers[root];
      numbers[root] = numbers[maxChild];
      numbers[maxChild] = temp;
      root = maxChild;
    }
    else
      done = 1;
  }
}
