
/*
This is the main program for the simple webserver.
Your job is to modify it to take additional command line
arguments to specify the number of threads and scheduling mode,
and then, using a pthread monitor, add support for a thread pool.
All of your changes should be made to this file, with the possible
exception that you may add items to struct request in request.h
*/

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "tcp.h"
#include "request.h"


int Thread_count=0;				// to keep track of  current no. of running threads 
pthread_mutex_t lock;			// mutex ensuring correct number of running threads  


// Increase Thread_count by 1 
void increase()
 {
	pthread_mutex_lock(&lock);
	Thread_count++;
	pthread_mutex_unlock(&lock);
}



// Decrease Thread_count by 1 
void decrease()
 {
	pthread_mutex_lock(&lock);
	Thread_count--;
	pthread_mutex_unlock(&lock);
}


// handle the incoming request 
void* serves_request(void* data)
 {
	increase();
	struct tcp *connect = (struct tcp*)data;
	printf("webserver: got new connection.\n");
	struct request *reqst = request_create(connect);
	if(reqst) {
		printf("webserver: got request for %s\n",reqst->filename);
		request_handle(reqst);

		printf("webserver: done sending %s\n",reqst->filename);
		request_delete(reqst);
	}
     else
     {
		tcp_close(connect);
	}

	decrease();
	return NULL;
}


int main( int argc, char *argv[] )
{
	if(argc<3) 
    {
		fprintf(stderr,"use: %s <port> <nthread>\n",argv[0]);
		return 1;
	}

	
    if(chdir("webdocs")!=0) 
    {
		fprintf(stderr,"couldn't change to webdocs directory: %s\n",strerror(errno));
		return 1;
	}

	int port = atoi(argv[1]);
	int max_Thread = atoi(argv[2]);

	struct tcp *master = tcp_listen(port);
	if(!master) 
    {
		fprintf(stderr,"couldn't listen on port %d: %s\n",port,strerror(errno));
		return 1;
	}

	printf("webserver: waiting for requests..\n");

	while(1) 
    {
		struct tcp *conn = tcp_accept(master,time(0)+300);
		if(conn) 
        {
			while(Thread_count == max_Thread);
			pthread_t thr;
			pthread_create(&thr, NULL, &serves_request, (void*)conn);
		} else 
        {
			printf("webserver: shutting down because idle too long\n");
			break;
		}
	}

	return 0;
}
