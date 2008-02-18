#include "sort.h"
#include <sys/time.h>

#define CHUNK_SIZE 10000000

typedef void (*funcSortPtr_t)(DATA_TYPE[], int);
// helper functions
inline double	random0_1(void)
{
	return rand()/((double)RAND_MAX + 1);
}
// random number generator
inline size_t	random_num(size_t N)
{
	return (size_t)(rand() / (((double)RAND_MAX + 1)/ N));
}
// permutes an array
inline void permute_chunk(DATA_TYPE numbers[], size_t size)
{
	size_t i;
	size_t rnd;
	register DATA_TYPE tmp;
	for (i = 0; i < size; i++)
	{
		rnd = random_num(size);
		// Swap
		tmp = numbers[i];
		numbers[i] = numbers[rnd];
		numbers[rnd] = tmp;
	}
}
// permutes an array(each chunk is permuted within the chunksize range)
inline void permute(DATA_TYPE numbers[], size_t size, size_t chunkSize)
{
	size_t chunk;
	size_t numChunks = size/chunkSize;
	for (chunk=0; chunk<numChunks; chunk++)
	{
		permute_chunk(&numbers[chunk*chunkSize], chunkSize);
	}
}
// subtracts 2 time values(timeval)
void tv_sub(struct timeval *out,struct timeval *in)
{
	if ( (out->tv_usec -= in->tv_usec) < 0) {	/* out -= in */
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
void sort(void (*funcSortPtr)(DATA_TYPE[], int), DATA_TYPE numbers[],size_t size, size_t chunkSize)
{
	size_t chunk;
	size_t numChunks = size/chunkSize;
	for (chunk=0; chunk<numChunks; chunk++)
	{
		(*funcSortPtr)(&numbers[chunk*chunkSize], chunkSize);
	}
}

void inline verify(DATA_TYPE numbers[], const int array_size, const char* testID)
{
    size_t i;
    for (i = 0; i < array_size; i++)
	if (numbers[i] != (DATA_TYPE) i)
	{
		err_quit("\nTest has failed: location %d returned %d instead of %d\n\n", i,numbers[i],i);
	}
}

void syntax_error(char* arg0)
{
	printf("Syntax error - Usage:\n");
	printf("%s -q -h -m -s -c  [mem size] \n",arg0 );
	printf("\t-q Perofrms a Quick Sort\n");
	printf("\t-h Performs a Heap Sort\n");
	printf("\t-m Perofrms a Merge Sort(Note: Merge sort, needs 1.5x more memory than other sorts\n");
	printf("\t-s Performs a Shell Sort(Warning: Extremely slow!)\n");
	printf("\t-c Partions memory into chunks and sorts each chunk locally\n");
	printf("\t[mem size]	Memory size to be tested(in MB)\n",arg0 );
	exit(1);
}

int main(int argc, char **argv)
{
	DATA_TYPE *numbers;
	size_t i;
	size_t array_size;
	int	c,
		oQSort = 0,
		oHSort = 0,
		oMSort = 0,
		oSSort = 0,
		oChunk = 0;
	static size_t chunkSize;
	struct timeval	tv_start, tv_finish;
	double timer;
	opterr = 0;		/* don't want getopt() writing to stderr */
	while ( (c = getopt(argc, argv, "qhmsc")) != -1) 
	{
		switch (c) {
		case 'q':	
			if(argc-1<optind)
				syntax_error(argv[0]);
			oQSort = 1;
			break;
		case 'h':	
			if(argc-1<optind)
				syntax_error(argv[0]);
			oHSort = 1;
			break;
		case 'm':	
			if(argc-1<optind)
				syntax_error(argv[0]);
			oMSort = 1;
			break;
		case 's':	
			if(argc-1<optind)
				syntax_error(argv[0]);
			oSSort = 1;
			break;
		case 'c':
			if(argc-1<optind)
				syntax_error(argv[0]);
			oChunk = 1;
			break;
		
		default:
			syntax_error(argv[0]);
		}		
	}

	if (optind != argc-1)
		syntax_error(argv[0]);	
	// Get Array size
	array_size = (atoi(argv[optind])*(1024*1024)) / sizeof(DATA_TYPE);
	if(oChunk)	// Do in chunks?
	{
		chunkSize = CHUNK_SIZE;
		array_size = ((size_t)array_size/chunkSize)*chunkSize;	// round down each chunk
	}
	else
		chunkSize = array_size;

	// Allocate memory	
	printf("Chunk span: %d elemets\n", chunkSize);
	printf("Allocating memory for array size: %d elements, %d MB...", array_size, (array_size*sizeof(DATA_TYPE))/(1024*1024));
	fflush(stdout);
	numbers = calloc(array_size, sizeof(DATA_TYPE));	// Create the array
	if (!numbers)
		err_sys("error: calloc : REDUCE ARRAY SIZE! :");
	printf("done\n");
	// Fill array with random integers
	for (i = 0; i < array_size; i++)
		numbers[i] = (DATA_TYPE) i;

	// Quick Sort
	if(oQSort)

	{
		printf("\n-QuickSort:");		
		printf("\nPermuting Array...");
		fflush(stdout);
		permute(numbers,array_size,chunkSize);

		printf("done\n");	
		fflush(stdout);
		
		//perform quick sort on array
		printf("Performing Quick Sort...");
		fflush(stdout);
		gettimeofday(&tv_start,NULL);			// Start timer
		//quickSort(numbers, array_size);
		sort(quickSort, numbers, array_size, chunkSize);
		gettimeofday(&tv_finish,NULL);			// Stop timer
		tv_sub(&tv_finish,&tv_start);
		timer = tv_finish.tv_sec + tv_finish.tv_usec / 1000000.0;
		printf("done, %.2f seconds\n", timer);	
		// Verify the arrayarray
		printf("Verifying Results...");	
		verify(numbers, array_size, "HeapSort");
		printf("done\n");	
	}
	// Heap Sort
	if(oHSort)
	{
		printf("\n-HeapSort:");		
		printf("\nPermuting Array...");	
		fflush(stdout);
		permute(numbers,array_size,array_size);
		printf("done\n");	
		fflush(stdout);

		//perform quick sort on array
		printf("Performing a Heap Sort...");	
		fflush(stdout);
		gettimeofday(&tv_start,NULL);			// Start timer
		//heapSort(numbers, array_size);
		sort(heapSort, numbers, array_size, array_size);
		gettimeofday(&tv_finish,NULL);			// Stop timer
		tv_sub(&tv_finish,&tv_start);
		timer = tv_finish.tv_sec + tv_finish.tv_usec / 1000000.0;
		printf("done, %.2f seconds\n", timer);	
		// Verify the array
		printf("Verifying Results...");	
		verify(numbers, array_size, "HeapSort");
		printf("done\n");	
	}	
	// Merge Sort
	if(oMSort)
	{
		printf("\n-MergeSort:");		
		printf("\nAllocating extra space for merge sort: %d elements, %d KB...", (array_size/2), (((array_size/2))*sizeof(DATA_TYPE))/1024);
		fflush(stdout);
		DATA_TYPE* temp = calloc((array_size), sizeof(DATA_TYPE));	// Create the array
		if (!temp)
			err_sys("error: calloc : REDUCE ARRAY SIZE! :");
		printf("done\n");
		printf("Permuting Array...");	
		fflush(stdout);
		permute(numbers,array_size,chunkSize);
		printf("done\n");	
		//perform merge sort on array	array_size = (atoi(argv[optind])*(1024*1024)) / sizeof(DATA_TYPE);
		printf("Performing Merge Sort...");	
		fflush(stdout);
		gettimeofday(&tv_start,NULL);			// Start timer
		mergeSort(numbers,temp, array_size);
		gettimeofday(&tv_finish,NULL);			// Stop timer
		tv_sub(&tv_finish,&tv_start);
		timer = tv_finish.tv_sec + tv_finish.tv_usec / 1000000.0;
		printf("done, %.2f seconds\n", timer);	
		// freeing temp memory
		free(temp);
		temp = NULL;
		// Verify the array
		printf("Verifying Results...");	
		verify(numbers, array_size, "MergeSort");
		printf("done\n");	
	}		
	// Shell Sort
	if(oSSort)
	{
		printf("\n-ShellSort:");		
		printf("\nPermuting Array...");	
		fflush(stdout);
		permute(numbers,array_size,chunkSize);
		printf("done\n");		
		fflush(stdout);
		//perform shell sort on array
		printf("Performing Shell Sort...");	
		fflush(stdout);
		gettimeofday(&tv_start,NULL);			// Start timer
		//shellSort(numbers, array_size);
		sort(shellSort, numbers, array_size, chunkSize);
		gettimeofday(&tv_finish,NULL);			// Stop timer
		tv_sub(&tv_finish,&tv_start);
		timer = tv_finish.tv_sec + tv_finish.tv_usec / 1000000.0;
		printf("done, %.2f seconds\n", timer);	
		// Verify the array
		printf("Verifying Results...");	
		verify(numbers, array_size, "ShellSort");
		printf("done\n");	
	}
	// freeing numbers memory
	free(numbers);
	numbers = NULL;
	printf("\nTest has passed!\n\n");	
	exit(0);
}

