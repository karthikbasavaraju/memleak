#define _GNU_SOURCE // to compile : gcc mmm.c -lstdc++ -Wl,--no-as-needed -ldl
#include <stdio.h>
#include <dlfcn.h>
#include<unistd.h>
#include<stdlib.h>

#define free(p) _free(p,__LINE__)
#define malloc(p) _malloc(p,__LINE__)
#define realloc(p,s) _realloc(p,s,__LINE__)
int allocation_count = 0;

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

void my_printf(char a[])
{
	//printf("%s",a); //uncomment to get detailed log
}

void add_ill_free_memory(int line)
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
}



void add_memory(void * p, int size, int line)
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
    	char c[100];
        sprintf(c,"Memory added = %ld(%p)--%dbytes : line(%d)\n",mt->address,p,mt->block,mt->line);
        my_printf(c);
}


int remove_memory(void * p,int line)
{
    mem_trac * temp =header;
    if(temp==NULL)
    {
	char c[100];
        sprintf(c,"Illegal free at %d\n",line);
        my_printf(c);
    	add_ill_free_memory(line);
    	return 0;
    }
    else if(temp->next==NULL)
    {
    	if(temp->address == (long)p)
    	{
	    	allocation_count = allocation_count - temp->block;
		char c[100];
	        sprintf(c,"Deleted memory = %ld(%p) --%dbytes\n",temp->address,p,temp->block);
	        my_printf(c);
    		header = NULL;
    	}
    	else
    	{
		char c[100];
        	sprintf(c,"Illegal free at %d\n",line);
	        my_printf(c);
    	    	add_ill_free_memory(line);
    	    	return 0;
    	}
    }
    else if(temp->address == (long)p)
    {
    	allocation_count = allocation_count - temp->block;
	char c[100];
       	sprintf(c,"Deleted memory = %ld(%p) --%dbytes\n",temp->address,p,temp->block);
        my_printf(c);
 	header= temp->next;    		
    }
    else
    {
    	while(temp->next!=NULL)
    	{
    		if(temp->next->address == (long)p)
    		{
    			allocation_count = allocation_count - temp->next->block;
			char c[100];
			sprintf(c,"Deleted memory = %ld(%p) --%dbytes\n",temp->next->address,p,temp->next->block);
			my_printf(c);
    			temp->next = temp->next->next;
    			brk(temp->next);
   			break;
    		}
    		temp=temp->next;
    	}
    	if(temp==NULL)
    	{
		char c[100];
		sprintf(c,"Illegal free at %d\n",line);
		my_printf(c);
	    	add_ill_free_memory(line);
    	    	return 0;
    	}
    }
    return 1;
}

void update_memory(void *new_p,void *p, int size,int line)
{
	mem_trac * temp =header;
	while(temp!=NULL)
	{
		if(temp->address == (long)p)
		{
			allocation_count = allocation_count + size -temp->block;
			temp->address = (long)new_p;
			temp->block = size;
			temp->line = line;
			char c[100];
			sprintf(c,"New Address=%ld--Block=%d\n",temp->address,temp->block);
			my_printf(c);
			break;
		}
		temp=temp->next;
	}
	if(temp==NULL)
	{
		allocation_count = allocation_count + (int)size;
		add_memory(new_p,size,line);
	}
}



void *_malloc(size_t size,int line)
{
	my_printf("\n");
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
	char *c = sbrk(1);
	sprintf(c,"malloc(%d)::\n", (int)size);
	my_printf(c);
	p = real_malloc(size);
	if(p!=NULL)
	{
		allocation_count = allocation_count + (int)size;
		add_memory(p,(int)size,line);
        }
     	return p;
}


void *_realloc(void *p, size_t size,int line)
{
   	my_printf("\n");
   	static void* (*real_realloc)(void*,size_t);
    	if(real_realloc==NULL) 
    	{
    		real_realloc = dlsym(RTLD_NEXT, "realloc");
    		if (NULL == real_realloc) 
    		{
        		fprintf(stderr, "Error in `dlsym`malloc: %s\n", dlerror());
    		}
    	}
	void *new_p = NULL;	
	char *c = sbrk(1);
	sprintf(c,"realloc(%d)::\n", (int)size);
	my_printf(c);
	new_p = real_realloc(p,size);
	if(new_p!=NULL)
	{
		update_memory(new_p,p,(int)size,line);
        }
        else
        {
        	remove_memory(p,line);
        }
     	return new_p;
}

void _free(void *p,int line)
{
    my_printf("\n");
    static void* (*real_free)(void*);
    if(real_free==NULL) 
    {
    	real_free = dlsym(RTLD_NEXT, "free");
    	if (NULL == real_free) 
    	{
        	fprintf(stderr, "Error in `dlsym` free: %s\n", dlerror());
    	}
    }

//    fprintf(stderr,"Freeing(%p)\n", p);
    int flag = remove_memory(p,line);
    if(flag==1)
    {    
	real_free(p);
    	my_printf("freed\n");
    }

}

void print_mem()  //for reference
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
	printf("\n\tSummary of data which are not freed :: Total %d blocks\n\n",allocation_count);
	if(temp1!=NULL)
	{	
		while(temp1!=NULL)
		{
			printf("\t\tAddress=0x%x--Blocks=%d--Line=%d\n",(unsigned int)temp1->address,temp1->block,temp1->line);
			temp1=temp1->next;		
		}
		brk(header);
		header = temp1;
	}
	else
		printf("\t\tNo Memory leak\n");
	
	ill_free * temp =header1;
	printf("\n\tSummary of corrupted memory::\n\n");
	if(temp!=NULL)
	{	
		while(temp!=NULL)
		{
			printf("\t\tDouble free or corruption at Line=%d\n",temp->line);
			temp=temp->next;
		}
		brk(header1);
		header1 = temp;
	}
	else
		printf("\t\tNO double free\n\n");
}



int main()
{
	printf("\n");
	int * a = malloc(0);

        int * b = malloc(sizeof(int));

        char * d1 = malloc(100*sizeof(d1));

  //      free(d1);
	
//	free(a);

	int * d = NULL;
	int * c = realloc(d,0);		
	
//	free(b);
        b=NULL;
 
 	free(c);
 	leak_result();
}
