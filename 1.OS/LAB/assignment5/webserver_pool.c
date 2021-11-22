
/*
This is the main program for the simple webserver.
Your job is to modify it to take additional command line
arguments to specify the number of threads and scheduling mode,
and then, using a pthread monitor, add support for a thread pool.
All of your changes should be made to this file, with the possible
exception that you may add items to struct request in request.h
*/


//including necessary header files
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>


#include "tcp.h"
#include "request.h"



#define FCFS 0
#define SFF 1
#define MAX_SIZE 10				 //Maximum number of request which is unserved and server can hold 



//node structure 
typedef struct Node
 {
	struct request* req;		
	off_t file_size;			//  Requested file size
	struct Node* next;
	struct Node* prev;
} Node;



typedef struct
 {
	pthread_mutex_t mut; 		//  mutex for maintaining the correct number of readers/writers reading/writing data
	int writers;		  		//  count of  Writers who are writing the data currently 
	int readers;		 		//  count of Readers who are  reading the data currently 
	pthread_cond_t writeOK;		//  when it is safe to read , it signals
	pthread_cond_t readOK; 		// when it is safe to write , it singnals 
	Node* head;                 //  head of list 
	Node* tail;                 //  tail of list 
	int size;					//  current no. of request in the List 
} rwl;



rwl* init_lock(void);
void* reader(void* arg_s);
void read_lock(rwl* loc);
void read_unlock(rwl* loc);
void write_lock(rwl* loc);
void write_unlock(rwl* loc);
Node* create_Node(struct request* req);
void write_Req(rwl* lis, struct request* req);
Node* read_Req(rwl* lis);



int mode;						// mode may be  FCFS/SFF  


int main( int argc, char *argv[] )
{
	
    if(argc<4 || (strcmp(argv[3], "FCFS") && strcmp(argv[3], "SFF"))) 
    {
		fprintf(stderr,"use: %s <port> <nthread> <mode:FCFS/SFF>\n",argv[0]);
		return 1;
	}

	
    if(chdir("webdocs")!=0) 
    {
		fprintf(stderr,"couldn't change to webdocs directory: %s\n",strerror(errno));
		return 1;
	}

	
    int port = atoi(argv[1]);
	int max_Thread = atoi(argv[2]);

	
    if(strcmp(argv[3], "FCFS")==0) 
    {
    mode=FCFS;
    }
    else
    {
    mode=SFF;
    }
	
    
    struct tcp *master = tcp_listen(port);
	
    if(!master) 
    {
		fprintf(stderr,"couldn't listen on port %d: %s\n",port,strerror(errno));
		return 1;
	}
	
    
    pthread_t pool[max_Thread];
	rwl* loc=init_lock();												// Initialization of reader-writer lock
	
    for(int i=0; i<max_Thread; i++)
    {
		pthread_create(&pool[i], NULL, &reader, (void*)loc);			// Creation of  n reader threads 
    }

	
    printf("webserver: waiting for requests..\n");
	
    // writer thread 
	while(1) 
    {
		
        struct tcp *connect = tcp_accept(master,time(0)+300);
		
        if(connect) 
        {
			printf("webserver: got new connection.\n");
			struct request *req = request_create(connect);
			
            if(req)
             {
				write_lock(loc);
				printf("webserver: got request for %s\n",req->filename);
				write_Req(loc, req);
				printf("webserver: Finished writing request for %s\n\n",req->filename);
				write_unlock(loc);
			}
             else 
             {
				tcp_close(connect);
			}
		} 
        else
         {
			printf("webserver: shutting down because idle too long\n");
			break;
		}
	}

	return 0;
}


// Reader-Writer lock  intialisation

rwl* init_lock() 
{
	rwl* loc = (rwl*)malloc(sizeof(rwl));
	pthread_mutex_init(&loc->mut, NULL);
	pthread_cond_init(&loc->writeOK, NULL);
	pthread_cond_init(&loc->readOK, NULL);
	
    loc->readers = 0;
	loc->writers = 0;
	loc-> head = loc->tail = NULL;
	loc->size=0;
	return loc;
}



//  reader or worker thread 
void* reader(void* arg_s) 
{
	rwl* loc = (rwl*)arg_s;
	Node* req_uest;
	
    while(1)
     {
		read_lock(loc);
		req_uest=read_Req(loc);
		printf("webserver: Finished Reading request for %s\n", req_uest->req->filename); 
		read_unlock(loc);
		printf("webserver: Serving request for %s with file size %ld\n\n",req_uest->req->filename, req_uest->file_size);
		request_handle(req_uest->req);
		printf("webserver: done sending %s\n\n",req_uest->req->filename);
		request_delete(req_uest->req);
		free(req_uest);
	}
	return NULL;
}



void read_lock(rwl* loc)
 {
	pthread_mutex_lock(&loc->mut);
	
    // Waiting till the  readers or writers stopped working on list and the list is not empty 
	while(loc->writers || loc->readers || loc->size ==0)
		pthread_cond_wait(&loc->readOK, &loc->mut);
	loc->readers++;
	pthread_mutex_unlock(&loc->mut);
}


void read_unlock(rwl* loc)
 {
	pthread_mutex_lock(&loc->mut);
	loc->readers--;
	pthread_cond_signal(&loc->writeOK);			// Sending signal to writer that they can write in  list 
	pthread_cond_broadcast(&loc->readOK);			// Sending signal to readers that they can read from  list 
	pthread_mutex_unlock(&loc->mut);
}



void write_lock(rwl* loc) 
{
	pthread_mutex_lock(&loc->mut);
	
    // Waiting  till  the readers will stop working on list and  list is not full 
	while(loc->readers || loc->size==MAX_SIZE)
		pthread_cond_wait(&loc->writeOK, &loc->mut);
	loc->writers++;
	pthread_mutex_unlock(&loc->mut);
}



void write_unlock(rwl* loc) 
{
	pthread_mutex_lock(&loc->mut);
	loc->writers--;
	pthread_cond_broadcast(&loc->readOK);			// Sending signal to readers that they can read from list 
	pthread_mutex_unlock(&loc->mut);
}




Node* create_Node(struct request* t) 
{
	Node* tem =(Node*)malloc(sizeof(Node));
	tem->req= t;

	if(mode==FCFS) 
    {
    tem->file_size=1;				//  file size of every request is treated as equal , in fcfs
    }
    else
     {
		struct stat buf;
		stat(t->filename, &buf);
		tem->file_size=buf.st_size;				//  file size of the requested file  are stored
	}
	tem->next=NULL;
	tem->prev=NULL;
	return tem;
}



// Writing  the request in  list in non-decreasing order of file size
// file size of every request is treated as equal ,in case of fcfs file size is irrevelant 
void write_Req(rwl* li, struct request* req_st) 
{
	Node* q = create_Node(req_st);
	
    printf("webserver: size of requested file %s: %ld\n",req_st->filename, q->file_size);
	
    if(li->head == NULL)
     {
		li->head = li->tail = q;
		li->size++;
		return;
	}
	Node* t = li->tail;

	
    // Storing request in non-decreasing order 
	while(t!=NULL && t->file_size > q->file_size) 
    {
		t = t->prev;
	}

	if(t != NULL) 
    {
		q->prev=t;
		if(t->next != NULL)
         {
			q->next=t->next;
			t->next->prev=q;
		}
		t->next=q;
		
        if(t == li->tail) 
        {
            li->tail=q;
        }
    }
	else 
    {
		q->next=li->head;
		li->head->prev=q;
		li->head=q;
	}
	li->size++;
}




// reading reuest from  list 
Node* read_Req(rwl* li) 
{
	Node* t = li->head;
	li->head = t->next;
	t->next =NULL;

	if(li->head != NULL)
    { 
    li->head->prev=NULL;
    }
    else 
    {
    li->tail = NULL;
    }

    li->size--;
	return t;
}
