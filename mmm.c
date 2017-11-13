#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include<unistd.h>
#include<stdlib.h>
int line;
#define free(p) _free(p,__LINE__)
#define malloc(p) _malloc(p,__LINE__)
int allocation_count = 0,cccm=1;

typedef struct memory_tracker
{
	long address;
	int block;
	int line;
	struct memory_tracker *next;
}mem_trac;


typedef struct illegal_free
{
	int line;
	struct illegal_free *next;
}ill_free;

ill_free * header1 =NULL;
mem_trac * header = NULL;


mem_trac add_ill_free_memory(int line)
{
    ill_free* mt = sbrk(sizeof(ill_free));
    mt->line = line;
    mt->next = NULL;
    if(header1==NULL)
    {
    	header1=mt;
    } 
    else
    {
    	ill_free* temp=header1;
    	while(temp->next!=NULL)
    	{
    		temp=temp->next;
    	}
    	temp->next = mt;
    }
        //printf("Memory added = %ld(%p)--%dbytes : line(%d)\n",mt->address,p,mt->block,mt->line);
}



mem_trac add_memory(void * p, int size, int line)
{
    mem_trac* mt = sbrk(sizeof(mem_trac));
    mt->address = (long)p;
    mt->block = size; 
    mt->line = line;
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
        printf("Memory added = %ld(%p)--%dbytes : line(%d)\n",mt->address,p,mt->block,mt->line);
}


int remove_memory(void * p,int line)
{
    mem_trac * temp =header;
    if(temp==NULL)
    {
    	printf("Illegal free at %d\n",line);
    	add_ill_free_memory(line);
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
	    	printf("Illegal free at %d\n",line);
    	    	add_ill_free_memory(line);
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
    		printf("Illegal free at %d\n",line);
	    	add_ill_free_memory(line);
    	    	return 0;
    	}
    }
    return 1;
}


void *_malloc(size_t size,int line)
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
		add_memory(p,(int)size,line);
        }
     	return p;
    }
}

void _free(void *p,int line)
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
    int flag = remove_memory(p,line);
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

void leak_result()
{
	mem_trac * temp1 =header;
	printf("\n\t\tLeaked summary\n");
	if(temp1!=NULL)
	{	
		while(temp1!=NULL)
		{
			printf("\t\tAddress=%ld--Blocks=%d--Line=%d\n",temp1->address,temp1->block,temp1->line);
			temp1=temp1->next;
		}
	}
	else
		printf("\t\tNo meory leak\n");
	
	ill_free * temp =header1;
	printf("\nLeaked summary\n");
	if(temp!=NULL)
	{	
		while(temp!=NULL)
		{
			printf("Illegal free at Line=%d\n",temp->line);
			temp=temp->next;
		}
	}
}



int main()
{
	printf("\n");
	int * a = malloc(10*sizeof(*a));
	print_mem();

	int * b = malloc(44);
		print_mem();
	
        b=a;
        free(a);
        printf("after free a\n");
		print_mem();
		
        free(b);
        printf("after free b\n");
        	print_mem();
       	printf("count=%d\n",allocation_count);
	b=NULL;
 	free(b);
 	leak_result();
}
