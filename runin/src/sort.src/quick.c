// quick.c
// quick sort routines
#include "sort.h"

void inline quickSort(DATA_TYPE numbers[], int array_size)
{
	q_sort(numbers, 0, array_size - 1);
}

void inline q_sort(DATA_TYPE numbers[], int left, int right)
{
	int l_hold, r_hold;
	int temp;
	DATA_TYPE pivot;
	l_hold = left;
	r_hold = right;
	pivot = numbers[left];
	while (left < right)
	{
		while ((numbers[right] >= pivot) && (left < right))
			right--;
		if (left != right)
		{
			numbers[left] = numbers[right];
			left++;
		}
		while ((numbers[left] <= pivot) && (left < right))
			left++;
		if (left != right)
		{
			numbers[right] = numbers[left];
			right--;
		}
	}
	numbers[left] = pivot;
	temp = left;
	left = l_hold;
	right = r_hold;
	if (left < temp)
		q_sort(numbers, left, temp-1);
	if (right > temp)
		q_sort(numbers, temp+1, right);
}
