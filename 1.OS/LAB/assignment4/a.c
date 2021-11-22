// Classical producer-consumer problem solved by using mutex and condition variable.
//  this program  prints subsequent integers , ranging from 0 to  9999 exactly twice,


//including headers files
#include <stdio.h>
#include <pthread.h>


//defining buffer size
#define BUFFER_SIZE 16



// circular buffer to store  integers

struct prodcons {
	int buffer[BUFFER_SIZE];	 // the actual data 
	pthread_mutex_t lock;	 // mutex maintains an exclusive access to the buffer 
	int readpos, writepos;	 // variable to keep track of positions for reading and writing 
	pthread_cond_t notempty;	// signaled when buffer is not empty 
	pthread_cond_t notfull;	 //signaled when buffer is not full 
};

 
 
 
 // Initialisation of  buffer that is shared by both(producer and consumer)

void init(struct prodcons *b)
{
	pthread_mutex_init(&b->lock, NULL);
	pthread_cond_init(&b->notempty, NULL);
	pthread_cond_init(&b->notfull, NULL);
	b->readpos = 0;
	b->writepos = 0;
}



 // storing an integer in the buffer 

void put(struct prodcons *b, int data)
{
	pthread_mutex_lock(&b->lock);
	    //must Wait until the buffer is not full 
	while ((b->writepos + 1) % BUFFER_SIZE == b->readpos) {
		pthread_cond_wait(&b->notfull, &b->lock);
		// this pthread_cond_wait reacquired b->lock before returning 
	}

	// Writing the data and moving the write pointer 
	b->buffer[b->writepos] = data;
	b->writepos++;
	if (b->writepos >= BUFFER_SIZE)
		b->writepos = 0;
	
    //it generates Signal that the buffer is now not empty 
	pthread_cond_signal(&b->notempty);
	pthread_mutex_unlock(&b->lock);
}



// this function reads and remove an integer from the buffer 

int get(struct prodcons *b)
{
	int data;
	pthread_mutex_lock(&b->lock);

	// must Wait until the buffer is not empty 
	while (b->writepos == b->readpos) {
		pthread_cond_wait(&b->notempty, &b->lock);
	}

	// reading the data  from buffer and moving the read pointer 
	data = b->buffer[b->readpos];
	b->readpos++;
	if (b->readpos >= BUFFER_SIZE)
		b->readpos = 0;

	// it generates Signal that the buffer is now not full 
	pthread_cond_signal(&b->notfull);
	pthread_mutex_unlock(&b->lock);
	return data;
}



//program: one thread will inserts integers ranging from 1 - 10000 and the other will reads them and prints . 

#define OVER (-1)

struct prodcons buffer;


//producer function
void *producer(void *data)
{
	int n;
	for (n = 0; n < 10000; n++) {
		printf("%d --->\n", n);
		put(&buffer, n);
	}
	put(&buffer, OVER);
	return NULL;
}



//consumer function
void *consumer(void *data)
{
	int d;
	while (1) 
	{
		d = get(&buffer);
		if (d == OVER)
		{
		    break;
		}
		printf("---> %d\n", d);
	}
	return NULL;
}



int main(void)
{
	pthread_t th_a, th_b;
	void *retval;

	init(&buffer);

	 
     // threads creation
	if ((pthread_create(&th_a, NULL, producer, 0)) != 0 ||
	    (pthread_create(&th_b, NULL, consumer, 0)) != 0) {
		fprintf(stderr, "pthread_create failed\n");
		return 1;
	}

	
    // must Wait until the execution of  producer and consumer finishes 
	pthread_join(th_a, &retval);
	pthread_join(th_b, &retval);
	return 0;
}

