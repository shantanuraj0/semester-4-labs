

/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/


#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



typedef struct
{
	int page;
	int bits;
} frame_T;




void random_policy(struct page_table *p , int pg );
void fifo_policy(struct page_table *p , int pg );
void remove_page(struct page_table *p , int fno );
void custom_policy(struct page_table *p , int pg );
int Check_Free_Frame();


frame_T* frame_table = NULL;



int front =0;
int rear =0;
int *fifo_array;



int page_faults = 0;
int disk_reads = 0;
int disk_writes = 0;


char *Policy ;
int n_pages;
int n_frames;


char *virt_mem = NULL;
char *phys_mem = NULL;
struct disk *disk = NULL;



void page_fault_handler( struct page_table *p , int pg  )
{
	page_faults++;

	if( !strcmp (Policy,"rand") ) 
    {
		random_policy(p , pg );
	}
	else if( !strcmp (Policy,"fifo")) 
    {
		fifo_policy(p , pg );
	}
	else if( !strcmp (Policy, "custom")) 
    {
		custom_policy(p, pg );
	}
	else
     {
		fprintf(stderr,"unknown policy: %s\n", Policy);
		exit(1);
	}
}



int main( int argc, char *argv[] )
{
	if(argc!=5) 
    {
		printf("use: virtmem <npages> <nframes> <rand|fifo|custom> <sort|scan|focus>\n");
		return 1;
	}

	n_pages = atoi(argv[1]);
	n_frames = atoi(argv[2]);
	Policy = argv[3];
	const char *program = argv[4];

	disk = disk_open("myvirtualdisk",n_pages);

	
	if(!disk) 
    {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}


	struct page_table *p  = page_table_create( n_pages, n_frames, page_fault_handler );
	
	if(!p )
     {
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}
	
	
	fifo_array = malloc (n_frames * sizeof(int));
	frame_table = malloc(n_frames * sizeof(frame_T));

	
	if (frame_table == NULL) 
    {
	    printf("error! in  allocating space for frame table \n");
	    exit(1);
	}

	
	virt_mem = page_table_get_virtmem(p );

	phys_mem = page_table_get_physmem(p );



	
	if( !strcmp( program,"sort")) 
    {
		sort_program(virt_mem,n_pages*PAGE_SIZE);
	} 
    else if( !strcmp( program,"scan"))
     {
		scan_program( virt_mem,n_pages*PAGE_SIZE);

	} 
    else if( !strcmp( program,"focus")) 
    {
		focus_program(virt_mem,n_pages*PAGE_SIZE);

	}
    else
    {
		fprintf(stderr,"unknown program: %s\n",argv[4]);
	}
	
	
	free(frame_table);
	free(fifo_array);
	page_table_delete(p );
	disk_close(disk);
	
	
	printf(" page faults : %d\n", page_faults);
	printf(" disk reads : %d\n", disk_reads);
	printf(" disk writes : %d\n", disk_writes);

	return 0;
}





void random_policy(struct page_table *p , int pg ) 
{
	int frame, bits;

	page_table_get_entry(p , pg, &frame, &bits );

	int f_index;

	
	if(!bits) 
    {
		bits = PROT_READ;
		
		f_index = Check_Free_Frame();

		if(f_index < 0) 
        {
			f_index = (int) lrand48() % n_frames;
			remove_page(p , f_index );
		}
		
		disk_read(disk, pg , &phys_mem[f_index*PAGE_SIZE]);
		disk_reads++;

	}
	else if(bits & PROT_READ)
    {
		bits = PROT_READ | PROT_WRITE;
		f_index = frame;
	} 
	else
    {
		printf(" error! on random page fault.\n");
		exit(1);
	}

	page_table_set_entry(p , pg , f_index, bits);

	frame_table[f_index].page = pg;
	frame_table[f_index].bits = bits;
}




void fifo_policy(struct page_table *p , int pg )
 {
	int frame, bits;

	page_table_get_entry(p , pg , &frame, &bits );

	
	int f_index;
	
	if(!bits) 
    { 
		
		bits = PROT_READ;
		f_index = Check_Free_Frame();

		
		if(f_index < 0) 
        {
			if ( front == rear )
             {
				
				f_index = fifo_array[front];
				remove_page(p , f_index );
			}
			else 
            {
				printf(" error! : on FIFO page swap\n");
				exit(1);
			}
		}

		
		disk_read(disk, pg , &phys_mem[f_index*PAGE_SIZE]);
		disk_reads++;
		fifo_array[rear]=f_index;
		rear =(rear +1) % n_frames;

	}
	else if(bits & PROT_READ) 
    {
		bits = PROT_READ | PROT_WRITE;
		f_index = frame;
	} 
	else 
    {
		printf("Error on random page fault.\n");
		exit(1);
	}
	
	page_table_set_entry(p , pg, f_index, bits );

	frame_table[f_index].page = pg ;
	frame_table[f_index].bits = bits;
}





void custom_policy (struct page_table *p , int pg ) 
{
	int frame, bits;

	page_table_get_entry(p , pg , &frame, &bits );

	int f_index;
	
	if(!bits) 
    { 	
		bits = PROT_READ;
		f_index = Check_Free_Frame();

		if(f_index < 0) 
        {
			if ( front == rear ) 
            {
				f_index = fifo_array[front];


				int s =0;
				
                while (s <2) 
                {
					if( frame_table[f_index].bits & PROT_WRITE )
                     {
						front = (front + 1)% n_frames;
						rear = (rear + 1)% n_frames;
						f_index = fifo_array[front];
					}
					s++;
				}
				remove_page(p , f_index );
			}
			else 
            {
				printf("error!");
				exit(1);
			}
		}

		disk_read(disk,  pg , &phys_mem[f_index*PAGE_SIZE]);
		disk_reads++;
		fifo_array[rear]= f_index;
		rear =(rear +1) % n_frames;

	}
	else if(bits & PROT_READ)
     {
		bits = PROT_READ | PROT_WRITE;
		f_index = frame;
	} 
	else 
    {
		printf("Error on random page fault.\n");
		exit(1);
	}

	page_table_set_entry(p , pg , f_index, bits );
	frame_table[f_index].page = pg ;
	frame_table[f_index].bits = bits ;
}




int Check_Free_Frame() 
{

	int p;

	for( p=0 ; p< n_frames ; p++ )
     {
		
        if( frame_table[p].bits  == 0 )
        {
			return p ;
        }
	}

	return -1;
}





void remove_page (struct page_table *p , int fno ) 
{
	if( frame_table[fno].bits & PROT_WRITE)
     {
		disk_write(disk, frame_table[fno].page, &phys_mem[fno*PAGE_SIZE]);
		disk_writes++;
	}

	page_table_set_entry(p , frame_table[fno].page , fno , 0);
	frame_table[fno].bits = 0;
	front=(front+1) % n_frames;
}