#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include<unistd.h>
int allocation_count = 0,cccm=0;

typedef struct memory_tracker
{
	long address;
	int block;
	struct memory_tracker *next;
}mem_trac;

mem_trac * header = NULL;

mem_trac add_memory(void * p, int size)
{
    mem_trac* mt = sbrk(sizeof(mem_trac));
    mt->address = (long)p;
    mt->block = size; 
    mt->next = NULL;
    if(header==NULL)
    {
    	header=mt;
    } 
    else
    {
    	mem_trac* temp=header;
    	while(temp->next!=NULL)
    	{
    		temp=temp->next;
    	}
    	temp->next = mt;
    }
        printf("Memory added = %ld(%p)--%dbytes\n::",mt->address,p,mt->block);
}


int remove_memory(void * p)
{
    mem_trac * temp =header;
    if(temp==NULL)
    {
    	printf("Illegal free\n");
    	return 0;
    }
    else if(temp->next==NULL)
    {
    	if(temp->address == (long)p)
    	{
	    	allocation_count = allocation_count - temp->block;
	    	printf("Deleted memory = %ld(%p) --%dbytes\n",temp->address,p,temp->block);
    		header = NULL;
    	}
    	else
    	{
	    	printf("Illegal free\n");
    	    	return 0;
    	}
    }
    else if(temp->address == (long)p)
    {
    	allocation_count = allocation_count - temp->block;
    	printf("Deleted memory = %ld(%p) --%dbytes\n",temp->address,p,temp->block);
 	header= temp->next;    		
    }
    else
    {
    	while(temp->next!=NULL)
    	{
    		if(temp->next->address == (long)p)
    		{
    			allocation_count = allocation_count - temp->block;
    			printf("Deleted memory = %ld(%p) --%dbytes\n",temp->next->address,p,temp->next->block);
    			temp->next = temp->next->next;
    			brk(temp->next);
   			break;
    		}
    		temp=temp->next;
    	}
    	if(temp==NULL)
    	{
    		printf("ILlegal free\n");
    	    	return 0;
    	}
    }
    return 1;
}


void *malloc(size_t size)
{
   if(cccm==0) cccm=1;
   else
   { 
   	printf("\n");
   	static void* (*real_malloc)(size_t);
    	if(real_malloc==NULL) 
    	{
    		real_malloc = dlsym(RTLD_NEXT, "malloc");
    		if (NULL == real_malloc) 
    		{
        		fprintf(stderr, "Error in `dlsym`malloc: %s\n", dlerror());
    		}
    	}
	void *p = NULL;	
	fprintf(stderr,"malloc(%d)::\n", (int)size);
	p = real_malloc(size);
	if(p!=NULL)
	{
		allocation_count = allocation_count + (int)size;
		add_memory(p,(int)size);
        }
     	return p;
    }
}

void free(void *p)
{
    printf("\n");
    static void* (*real_free)(void*);
    if(real_free==NULL) 
    {
    	real_free = dlsym(RTLD_NEXT, "free");
    	if (NULL == real_free) 
    	{
        	fprintf(stderr, "Error in `dlsym` free: %s\n", dlerror());
    	}
    }

    fprintf(stderr, "Freeing(%p)\n", p);
    int flag = remove_memory(p);
	if(flag==1)
	{    
		real_free(p);
    		printf("freed\n");
    	}

}

void print_mem()
{
	mem_trac * temp =header;
	if(temp!=NULL)
	{	
		while(temp!=NULL)
		{
			printf("Address=%ld--Block=%d\n",temp->address,temp->block);
			temp=temp->next;
		}
	}
	else printf("NO data\n");
}



int main()
{
	printf("\n");
	int * a = malloc(10*sizeof(*a));
	print_mem();

	int * b = malloc(44);
		print_mem();
	//printf("Starting add of a:%p\n",a);
	//printf("count=%d\n\n",allocation_count);
 
        b=a;
        free(a);
        printf("after free a\n");
		print_mem();
		

	//printf("Starting add of second b:%p\n",b);
        free(b);
        printf("after free b\n");
        	print_mem();
       	printf("count=%d\n",allocation_count);
	b=NULL;
 	free(b);
 	printf("%d---%s\n",__LINE__,__FILE__);
}
