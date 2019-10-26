#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUMS_MAX_LEN 100
int nums[NUMS_MAX_LEN];
int nums_sorted[NUMS_MAX_LEN];

typedef struct
{
	int low;
	int high;
} parameters;

void *sort(void *param);
void *merge(void *param);

int main(int argc, char *argv[])
{
	if(argc - 1 > NUMS_MAX_LEN)
	{
		perror("Too many numbers\n");
		return 1;
	}
	pthread_t tid_s1, tid_s2, tid_m;
	pthread_attr_t attr_s1, attr_s2, attr_m;
	int n = argc-1;
	int mid = n/2;

	/* read args to global array */
	for(int i=0; i< n; ++i)
		nums[i] = atoi(argv[i+1]);
	
	/* array index low <= i <= high */
	parameters *data_s1 = (parameters *)malloc(sizeof(parameters));
	parameters *data_s2 = (parameters *)malloc(sizeof(parameters));

	pthread_attr_init(&attr_s1);
	pthread_attr_init(&attr_s2);
	pthread_attr_init(&attr_m);
	data_s1 -> low = 0;
	data_s1 -> high = mid-1;
	pthread_create(&tid_s1, &attr_s1, sort, data_s1);
	data_s2 -> low = mid;
	data_s2 -> high = n-1;
	pthread_create(&tid_s2, &attr_s2, sort, data_s2);
	pthread_join(tid_s1, NULL);
	pthread_join(tid_s2, NULL);
	free(data_s1);
	free(data_s2);
	pthread_create(&tid_m, &attr_m, merge, &n);
	pthread_join(tid_m, NULL);

	printf("The sorted list:\n");
	for(int i =0; i< n; ++i)
		printf("%d  ", nums_sorted[i]);
	printf("\n");
	return 0;
}


void *sort(void *param)
{
	parameters *para = (parameters *)param;
	for(int i = para->low; i <= para->high; ++i)
	{
		for(int j = i-1; j >= para->low && nums[j] > nums[j+1]; --j)
		{
			int tmp = nums[j];
			nums[j] = nums[j+1];
			nums[j+1] = tmp;
		}
	}
	pthread_exit(0);
}

void *merge(void *param)
{
	int n = *(int*)param;
	int mid = (int)n / 2;
	int i, j, k;
	for(i=0, j = mid, k =0; i<= mid-1 && j<= n-1; )
	{
		if(nums[i]<nums[j])
			nums_sorted[k++] = nums[i++];
		else
			nums_sorted[k++] = nums[j++];
	}
	for(;i<=mid-1;)
		nums_sorted[k++] = nums[i++];
	for(;j<=n-1;)
		nums_sorted[k++] = nums[j++];
	pthread_exit(0);
}

