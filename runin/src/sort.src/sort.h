#ifndef _SORT_T_
#define _SORT_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "error.h"

#define VERSION		"1.2"
#define DATA_TYPE	size_t	

// quick sort
inline void quickSort(DATA_TYPE numbers[], int array_size);
inline void q_sort(DATA_TYPE numbers[], int left, int right);
// heap sort
inline void heapSort(DATA_TYPE numbers[], int array_size);
inline void siftDown(DATA_TYPE numbers[], int root, int bottom);
// merge sort
inline void mergeSort(DATA_TYPE numbers[], DATA_TYPE temp[], int array_size);
inline void m_sort(DATA_TYPE numbers[], DATA_TYPE temp[], int left, int right);
inline void merge(DATA_TYPE numbers[], DATA_TYPE temp[], int left, int mid, int right);
// shell sort
inline void shellSort(DATA_TYPE numbers[], int array_size);

#endif	// _SORT_H_
 
