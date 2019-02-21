#include <stdio.h>
#include<stdlib.h>
int main(void)
{
int **ptr=NULL;
int i=0;
int j=0;

ptr = (int **)malloc(5 * sizeof(int *));

if(ptr==NULL)
{
	exit(0);
}

for(i=0;i<5;i++)
{
	ptr[i]=(int *)malloc(3 * sizeof(int));
	if(ptr[i]==NULL)
	{
		exit(0);
	}
	
}

for(i=0;i<5;i++)
{
	for(j=0;j<3;j++)
	{
		ptr[i][j]=i*3+j+1;
	}
}
for(i=0;i<5;i++)
{
	printf("Address of Array ptr + %d is %p\n",i,ptr+i);
}
printf("\n\n");

for(i=0;i<5;i++)
{
	for(j=0;j<3;j++)
	{
		printf("Number at Array[%d][%d] is %d and address is %p\n", i,j,ptr[i][j],&ptr[i][j]);
	}
	printf("\n");
}

for(i=0;i<5;i++)
{
	for(j=0;j<3;j++)
	{
		printf("Number at ptr[%d] + %d is %d and address is %p\n", i,j,ptr[i][j],&ptr[i][j]);
	}
	printf("\n");
}


for(i=4;i>=0;i--)
{
free(ptr[i]);
}
return 0;

}
