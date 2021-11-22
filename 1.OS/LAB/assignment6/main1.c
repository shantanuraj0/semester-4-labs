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
} frame_t;


void Random_Policy(struct page_table *ptab , int page );
void Fifo_Folicy(struct page_table *ptab , int page );
void Remove_Page(struct page_table *ptab , int f_no );
void Custom_Policy(struct page_table *ptab , int page );
int Check_Free_Frame();


frame_t* Frame_Table = NULL;

int front =0;
int rear =0;
int *Fifo_Array;

int Page_Faults = 0;
int Disk_Reads = 0;
int Disk_Writes = 0;

char *Policy ;
int N_Pages;
int N_Frames;

char *Virt_Mem = NULL;
char *Phys_Mem = NULL;
struct disk *disk = NULL;


void page_fault_handler( struct page_table *ptab , int page  )
{
	Page_Faults++;

	if( !strcmp (Policy,"rand") ) 
    {
		random_policy(ptab , page );
	}
	else if( !strcmp (Policy,"fifo")) 
    {
		fifo_policy(ptab , page );
	}
	else if( !strcmp (Policy, "custom")) 
    {
		custom_policy(ptab, page );
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

	N_Pages = atoi(argv[1]);
	N_Frames = atoi(argv[2]);
	Policy = argv[3];
	const char *program = argv[4];

	disk = disk_open("myvirtualdisk",N_Pages);

	if(!disk) 
    {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}


	struct page_table *ptab  = page_table_create( N_Pages, N_Frames, page_fault_handler );
	if(!ptab )
     {
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}
	
	Fifo_Array = malloc (N_Frames * sizeof(int));
	Frame_Table = malloc(N_Frames * sizeof(Frame_t));

	if (Frame_Table == NULL) 
    {
	    printf("error! in  allocating space for frame table \n");
	    exit(1);
	}

	Virt_Mem = page_table_get_virtmem(ptab );

	Phys_Mem = page_table_get_physmem(ptab );



	if( !strcmp( program,"sort")) 
    {
		sort_program(Virt_Mem,N_Pages*PAGE_SIZE);
	} 
    else if( !strcmp( program,"scan"))
     {
		scan_program( Virt_Mem,N_Pages*PAGE_SIZE);

	} 
    else if( !strcmp( program,"focus")) 
    {
		focus_program(Virt_Mem,N_Pages*PAGE_SIZE);

	}
    else
    {
		fprintf(stderr,"unknown program: %s\n",argv[4]);
	}
	free(Frame_Table);
	free(Fifo_Array);
	page_table_delete(ptab );
	disk_close(disk);
	
	printf(" page faults : %d\n", Page_Faults);
	printf(" disk reads : %d\n", Disk_Reads);
	printf(" disk writes : %d\n", Disk_Writes);

	return 0;
}



void Random_Policy(struct page_table *ptab , int page )
 {
	int frame, bits;

	page_table_get_entry(ptab , page, &frame, &bits );

	int F_Index;

	if(!bits) 
    {
		bits = PROT_READ;
		
		F_Index = Check_Free_Frame();

		if(F_Index < 0) 
        {
			F_Index = (int) lrand48() % N_Frames;
			Remove_Page(ptab , F_Index );
		}
		disk_read(disk, page , &Phys_Mem[F_Index*PAGE_SIZE]);
		Disk_Reads++;

	}
	else if(bits & PROT_READ)
    {
		bits = PROT_READ | PROT_WRITE;
		F_Index = frame;
	} 
	else
    {
		printf(" error! on random page fault.\n");
		exit(1);
	}

	page_table_set_entry(ptab , page , F_Index, bits);

	Frame_Table[F_Index].page = page;
	Frame_Table[F_Index].bits = bits;
}




void Fifo_Policy(struct page_table *ptab , int page )
 {
	int frame, bits;

	page_table_get_entry(ptab , page , &frame, &bits );

	int F_Index;
	
	if(!bits) 
    { 
		bits = PROT_READ;
		F_Index = Check_Free_Frame();

		if(F_Index < 0) 
        {
			if ( front == rear )
             {
				F_Index = Fifo_Array[front];
				Remove_Page(ptab , F_Index );
			}
			else 
            {
				printf(" error! : on FIFO page swap\n");
				exit(1);
			}
		}

		disk_read(disk, page , &Phys_Mem[F_Index*PAGE_SIZE]);
		Disk_Reads++;
		Fifo_Array[rear]=F_Index;
		rear =(rear +1) % N_Frames;

	}
	else if(bits & PROT_READ) 
    {
		bits = PROT_READ | PROT_WRITE;
		F_Index = frame;
	} 
	else 
    {
		printf("Error on random page fault.\n");
		exit(1);
	}
	
	page_table_set_entry(ptab , page, F_Index, bits );

	frame_table[F_Index].page = page ;
	frame_table[F_Index].bits = bits;
}





void Custom_Policy (struct page_table *ptab , int page ) 
{
	int frame, bits;

	page_table_get_entry(ptab , page , &frame, &bits );

	int F_Index;
	
	if(!bits) 
    { 	
		bits = PROT_READ;
		F_Index = Check_Free_Frame();

		if(F_Index < 0) 
        {
			if ( front == rear ) 
            {
				F_Index = Fifo_Array[front];


				int t =0;
				
                while (t <2) 
                {
					if( Frame_Table[F_Index].bits & PROT_WRITE )
                     {
						front = (front + 1)% N_Frames;
						rear = (rear + 1)% N_Frames;
						F_Index = Fifo_Array[front];
					}
					t++;
				}
				Remove_Page(ptab , F_Index );
			}
			else 
            {
				printf("error!");
				exit(1);
			}
		}

		disk_read(disk,  page , &Phys_Mem[F_Index*PAGE_SIZE]);
		Disk_Reads++;
		Fifo_Array[rear]= F_Index;
		rear =(rear +1) % N_Frames;

	}
	else if(bits & PROT_READ)
     {
		bits = PROT_READ | PROT_WRITE;
		F_Index = frame;
	} 
	else 
    {
		printf("Error on random page fault.\n");
		exit(1);
	}

	page_table_set_entry(ptab , page , F_Index, bits );
	Frame_Table[F_Index].page = page ;
	Frame_Table[F_Index].bits = bits ;
}




int Check_Free_Frame() 
{

	int t;

	for( t=0 ; t< N_Frames ; t++ )
     {
		
        if( Frame_Table[t].bits  == 0 )
        {
			return t ;
        }
	}

	return -1;
}





void Remove_Page (struct page_table *ptab , int f_no ) 
{
	if( Frame_Table[f_no].bits & PROT_WRITE)
     {
		disk_write(disk, Frame_Table[f_no].page, &Phys_Mem[f_no*PAGE_SIZE]);
		Disk_Writes++;
	}

	page_table_set_entry(ptab , Frame_Table[f_no].page , f_no , 0);
	Frame_Table[f_no].bits = 0;
	front=(front+1) % N_Frames;
}