#include<stdio.h>
#include<stdlib.h>
#include <limits.h>


#define PROCESS_MAXIMUM 16
#define PROCESS_NAME 8


typedef struct
{
	char *name;
	int arrival;
	int cpuburst;
	int turnaround;
	int wait;
} Process;


Process process_table[PROCESS_MAXIMUM];  //  to store info of different process
int current_total_process = 0; // total count of processes 



// to print input table
void print_process_table()
{
	printf("Process table: \n");
	for (int p = 0; p < current_total_process; p++)
	{
		printf("At index[%d], name : %s, arrival : %d, cpuburst : %d\n", p, process_table[p].name, process_table[p].arrival, process_table[p].cpuburst);
	}
}



// resetting turnaround and wait time of each process after execution 
void reset()
{
	for (int p = 0; p < current_total_process; p++)
	{
		process_table[p].wait = 0;
		process_table[p].turnaround = 0;
	}
}



//to print statistics of  turnaround time and waiting time 
void Print_Statistics()
{
	
	printf("Turn Around times: ");
	for( int p =0 ; p< current_total_process ; p++ )
	{
		printf("%s[%d]  ",process_table[p].name  , process_table[p].turnaround);
	}
	printf("\n");

	
	printf("Wait times : ");
	for( int p =0 ; p< current_total_process ; p++ )
	{
		printf("%s[%d]  ",process_table[p].name  , process_table[p].wait);
	}
	printf("\n");

	
	int total_turnAround_time = 0 , total_wait_time = 0 ;
	for(int p =0 ; p<current_total_process ; p++)
	{
		total_turnAround_time = total_turnAround_time + process_table[p].turnaround ;
		total_wait_time = total_wait_time + process_table[p].wait ;
	}

	
	printf("Average turnaround time : %.2f\n",(float)total_turnAround_time / current_total_process) ;
	printf("Average wait time : %.2f\n",(float)total_wait_time / current_total_process) ;
}




//first come first serve scheduling
void FCFS()
{
	int start_time = 0 ,end_time = 0 ;
	int current_time = 0 ;

	
	for(int p =0 ; p < current_total_process ; p++)
	{
		if(current_time < process_table[p].arrival )
		{
			current_time = process_table[p].arrival ;
		}	

		start_time = current_time ;
		end_time = start_time + process_table[p].cpuburst ;
		
		process_table[p].turnaround = end_time - process_table[p].arrival ;
		process_table[p].wait = process_table[p].turnaround - process_table[p].cpuburst ;
		current_time = end_time;
		
		printf("[%d-%d]\t %s  running\n",start_time , current_time ,process_table[p].name );
		
		
	}

	Print_Statistics();
	reset();
}



//finding the next RR process to be executed
int next_RR_process(int remaining_cpuburst[], int current_time, int pos )
{
	int next_pos = ( pos + 1) % current_total_process;

	// checking  next process is arrived or not
	if (current_time >= process_table[next_pos].arrival)
	{
		return next_pos;
	}


	//next process is not arrived  so move  for any previous or current remaining process
	for (int p = 0; p <= pos ; p++ )
	{
		if ( remaining_cpuburst[p] != 0  &&  current_time >= process_table[p].arrival )
		{
			return p ;
		}
	}

	return -1;
}



//Round Robin scheduling
void RR(int quantum)
{
	int remaining_process = current_total_process;
	int current_time = 0, running_time = 0;
	int pos = 0;
	
	int remaining_cpuburst[current_total_process];  

	//intialising remaining_cpuburst array with given intial cpuburst values of given processes
	for( int p=0 ; p< current_total_process ; p++) 
	{
		remaining_cpuburst[p] = process_table[p].cpuburst ;
	}


	while(remaining_process)
	{
		if(current_time >= process_table[pos].arrival && remaining_cpuburst[pos] != 0 )
		{
			//calculating running time for the process
			if(remaining_cpuburst[pos] - quantum >= 0 )
			{
				running_time = quantum;
			}
			else
			{
				running_time = remaining_cpuburst[pos];
			}

			//checking whether the process is completed or not .And decrementing count of remaining processes if completed
			if (running_time == remaining_cpuburst[pos])
				remaining_process--;

			
			for(int p =0 ; p< current_total_process ; p++)
			{	
				//updating remaining burst time for current process
				if(pos == p)
				{
					remaining_cpuburst[p] = remaining_cpuburst[p] - running_time ;
				}
				else 
				{	
					//updating wait time of only those processes  which have remaining cpuburst
					if (current_time + running_time >= process_table[p].arrival && remaining_cpuburst[p] != 0)
					{
						//updating wait time of those process which arrived in between the execution of current process
						if (current_time < process_table[p].arrival)
						{
								process_table[p].wait  += current_time + running_time - process_table[p].arrival;
						}
						else  //updating wait time of those process which arrived before  execution of current process
						{												
							process_table[p].wait = process_table[p].wait + running_time;
						}
					}
				}
			}

			
		}

		
		if (running_time)
		{
			printf("[%d-%d]\t %s is running\n", current_time, current_time + running_time, process_table[pos].name);
			current_time = current_time +  running_time;
		}

		

		//next process is not present at current_time if it returns -1
		if ((pos = next_RR_process ( remaining_cpuburst, current_time, pos ) ) == -1)
		{
			current_time++;
		}

		running_time = 0;  //resetting running time for next process

	}

	
	
	//finding turnaround time
	for (int p = 0; p <current_total_process ; p++)
	{
		process_table[p].turnaround = process_table[p].wait + process_table[p].cpuburst;
	}

	Print_Statistics();
	reset();
}



// finding next process arrival time and return it and if no more process to arrive then it will return -1
int next_arrival_time(int current_time)
{
	for (int p = 0; p < current_total_process ; p++)
	{
		if (current_time < process_table[p].arrival)
		{
			return process_table[p].arrival;
		}
	}

	return -1;
}



//finding next shortest burst process and  return its position
int shortest_burst_remaining(int remaining_cpuburst[] , int current_time )
{
	int pos = -1 , burst_value = INT_MAX;

	for( int p =0 ; p< current_total_process ; p++)
	{
		// if cpuburst of remaining process is already arrived and which has shortest cpuburst
		if( current_time >= process_table[p].arrival &&   remaining_cpuburst[p] &&  burst_value > remaining_cpuburst[p]  )
		{
			burst_value = remaining_cpuburst[p];
			pos = p ;
		}
	}

	return pos ;
}





//Shortest Remaining Burst First (SRBF)
void SRBF()
{
	int remaining_process = current_total_process;
	int current_time = 0, running_time = 0 , next_interrupt= 0 ;
	int pos = 0;
	
	int remaining_cpuburst[current_total_process];  

	//intialising remaining_cpuburst array with given intial cpuburst values of given processes
	for( int p=0 ; p< current_total_process ; p++) 
	{
		remaining_cpuburst[p] = process_table[p].cpuburst ;
	}


	while(remaining_process)
	{
		// next process has not arrived yet then it will return -1
		if (( pos = shortest_burst_remaining( remaining_cpuburst , current_time) )  != -1)
		{
			//next_arrival return -1 then it  means no new processes are arriving
			if ( ( next_interrupt = next_arrival_time (current_time ) ) != -1)
			{
				// current process will be completed before next process arrives or not
				if (current_time + remaining_cpuburst[pos] <= next_interrupt )
				{
					running_time = remaining_cpuburst[pos];
				}
				else
				{
					running_time = next_interrupt - current_time;
				}
			}
			else
			{
				running_time = remaining_cpuburst[pos];
			}

			//checking if the current process will be over in this cycle or not
			if (running_time == remaining_cpuburst[pos])
			{
				remaining_process--;
			}

			
			for (int p = 0; p < current_total_process ; p++)
			{
				if (pos == p)
				{
					remaining_cpuburst[p] =  remaining_cpuburst[p] - running_time;
				} 
				else
				{
					//updating waiting time of all the  process which are not completed and has already arrived
					if (remaining_cpuburst[p] != 0  &&  current_time >= process_table[p].arrival)
					{
						process_table[p].wait =  process_table[p].wait +  running_time;
					}
				}

			}

			printf("[%d-%d]\t%s is running\n", current_time, current_time + running_time, process_table[pos].name);
			current_time =  current_time + running_time;
		} 
		else
		{
			current_time++;
		}
	}

	//finding turnaround time of process
	for (int p = 0; p < current_total_process ; p++)
	{
		process_table[p].turnaround = process_table[p].wait + process_table[p].cpuburst;
	}
	
	Print_Statistics();
	reset();

}



int main(int argc , char *argv[] )
{
	//checking input file is present or not
	if(argc != 2)
	{
		fprintf(stderr,"Error: Input file is missing!\n") ;
		exit(0);
	}

	FILE *finput = fopen(argv[1] ,"r"); //opening a input file

	
	if( finput == NULL )  //Error handling in opening input file
	{
		fprintf(stderr , "Error: in opening input file\n");
		exit(0);
	}

	
	//allocating memory 
	for (int p = 0; p < PROCESS_MAXIMUM ; p++)
	{
		process_table[p].name = (char *) calloc (PROCESS_NAME, sizeof(char));
	}	

	
	//reading input file and storing the values in different arrays
	for (int p = 0; p < PROCESS_MAXIMUM; p++)
	{
		
		if (!feof(finput))
		{
			fscanf(finput, " %s %d %d", process_table[p].name, &(process_table[p].arrival), &(process_table[p].cpuburst));
		} 
		else// storing count of different process 
		{
			current_total_process = p - 1;
			break;
		}
	}

	
	//printing input file
	print_process_table();
	

	
	while(1)
	{


	printf("*****************************************\n");
	printf("        CPU Scheduling Simulation        \n");
    printf("*****************************************\n");
	printf("Select the scheduling algorithm [1,2,3 or 4]:\n");
	printf("1. First Come First Served (FCFS)\n2. Round Robin (RR)\n3. SRBF\n4. Exit\n");
	

		int choice;
	    scanf("%d",&choice) ;

		switch( choice )
		{
			case 1: printf("*****************************************\n");
	                printf("      First Come First Served Scheduling        \n");
                    printf("*****************************************\n");
					FCFS();
					break;

			case 2: printf("Enter the time quantum: ");
					int quantum ;
					scanf("%d",&quantum);
					printf("\n");

					printf("*****************************************\n");
	                printf("        Round Robin Scheduling       \n");
                    printf("*****************************************\n");
					RR(quantum);
					break;	

			case 3: printf("*****************************************\n");
	                printf("        Shortest Remaining Burst First       \n");
                    printf("*****************************************\n");
					SRBF();
					break;			

			case 4: exit(1);

			default: printf("Not a valid choice!\n");
					 break;
		}
		printf("Hit any key to continue ...\n");
	}
	fclose(finput); //closing file
	return 0;
}
