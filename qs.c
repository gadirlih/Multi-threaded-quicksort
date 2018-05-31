/*
 Due to large ARRAY_SIZE print function was commented out, in order to see difference more clearly. 
 It can be enabled.	
 
 r = regular p = parallel

 Array Size: 1,000,000     r: real 0m0.265s p: real 0m0.209s
 Array Size: 10,000,000    r: real 0m3.019s p: real 0m1.451s
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct{
	int *num;
	int start;
	int end;
	int pivot;
}array_data;

#ifndef MAX_THREAD
#define MAX_THREAD 8
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE 1000000
#endif

void swap();
int partition();
void quick_sort();
void * threaded_quick_sort();
void print();
void thread_join();

static pthread_t tids[MAX_THREAD];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int i = 0;
static int num[ARRAY_SIZE];

int main()
{
    for(int i = 0; i < ARRAY_SIZE; i++){
        num[i] = rand() % ARRAY_SIZE + 1;
    }

    array_data * a_data = malloc(sizeof(array_data));
    a_data->start = 0;
    a_data->end = ARRAY_SIZE - 1;
    a_data->num = num;

    threaded_quick_sort(a_data);
    thread_join();

    //printf("Result: \n");
    //print(num, ARRAY_SIZE);

    return 0;
}

void * threaded_quick_sort(void *a_data)
{
	array_data *temp_data = a_data;
	int start = temp_data->start;
	int end = temp_data->end;
	int *num = temp_data->num;
	int pivot = temp_data->pivot;

	if (start < end && i < MAX_THREAD){


        	int pivot_index = partition(num, start, end);

		array_data *l_data = malloc(sizeof(array_data));
		array_data *r_data = malloc(sizeof(array_data));

		pthread_mutex_lock(&mutex);
		if(start < pivot_index){
            		l_data->end = pivot_index - 1;
			l_data->start = start;
			l_data->num = num;
			l_data->pivot = pivot_index;
			pthread_create(tids + i, NULL, &threaded_quick_sort, l_data);
		}
		i++;

		if(pivot_index < end){
			r_data->start = pivot_index + 1;
			r_data->end = end;
			r_data->num = num;
			r_data->pivot = pivot_index;
            		pthread_create(tids + i, NULL, &threaded_quick_sort, r_data);
		}
		i++;
		pthread_mutex_unlock(&mutex);

	}else if(start < end){
		quick_sort(num, start, pivot - 1);
		quick_sort(num, pivot + 1, end);
	}
}

int partition (int *num, int start, int end)
{
    int pivot = num[end];
    int pivot_index = start;

    for (int i = start; i <= end- 1; i++)
    {
        if (num[i] <= pivot)
        {

            swap(&num[pivot_index], &num[i]);
            pivot_index++;
        }
    }
    swap(&num[pivot_index], &num[end]);
    return pivot_index;
}

void quick_sort(int arr[], int low, int high)
{
    if (low < high)
    {
        int pivot = partition(arr, low, high);

        quick_sort(arr, low, pivot - 1);
        quick_sort(arr, pivot + 1, high);
    }
}

void print(int *num, int size)
{
    for (int i = 0; i < size; i++){
        printf("%d ", num[i]);
    }
    printf("\n");
}

void thread_join(){

	for(int j = 0; j < MAX_THREAD; j++){
		pthread_join(tids[j], NULL);
	}

}

void swap(int * x, int * y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}
