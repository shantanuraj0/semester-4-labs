#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>
#include<ctype.h>

int main(int argc , char* argv[])
{
	if(argc != 3)		// to check both argument is passed or not  (input filename and output filename)
	{
		fprintf(stderr , "Error! : less argument passed\n");
		exit(1);

	}

	int fd_in;								
	fd_in = open( argv[1] , O_RDONLY);	//opens file1					

	
	if(fd_in == -1)				//checking file1 opens or not								
	{	fprintf(stderr , "Error! : file1 doesn't opens correctly\n");
		exit(1);
	}

	int fd_out = creat ( argv[2] , 0644);	//creating file2
	if(fd_out == -1)			//checking file2 is created or not 
	{	fprintf(stderr , "Error! : file2 doesn't created correctly\n");
		exit(1);
	}



	close(0);	//closing standard input
	if(dup(fd_in) == -1)	//setting file1 as a standard input  and handling error 
	{
		fprintf(stderr,"Error! in setting file1 as STDIN\n");
		exit(1);
	}
	close(fd_in);


	close(1);	//closing standard output
	if(dup(fd_out) == -1 )	//setting file2 as standard output and handling error
	{
		fprintf(stderr , "Error! in setting file2 as STDOUT\n");
		exit(1);
	}
	close(fd_out);
	
	/*file descriptor table upto this point
	0 :file1
       	1 :file2	
	2 :stderr
	*/


	int pipe_end[2];	
				  //parent calls pipe sysytem call to create pipe file descriptors
	if(pipe(pipe_end) < 0 )  // creating pipe and handling error
	{	fprintf(stderr , "Error! pipe doesn't created coorectly");
		exit(1);
	}

	int fork_read ;		
	fork_read = fork();	//first child forked

	if(fork_read > 0 )	//parent process
	{
		//wait(NULL);
		int fork_write = fork();//second child forked
			
		if(fork_write > 0 )
		{
			//wait(NULL);
			//close(pipe_end[0]);

			close(pipe_end[1]);

			wait(NULL);
			wait(NULL);
		}
		else if( fork_write == 0 )  //second child
		{
			close(1);	
			dup(pipe_end[1]);//setting pipe_end[1](write end of pipe) to STDOUT

			close(pipe_end[1]);
			
			/* file descriptor table now
			    0 : file1
			    1 : pipe_end[1]
			    2 : stderr
			 */
					//now q3.c will take input from file1 and output it into pipe_end[1]

			//close(pipe_end[0]);
			execl("./q3","./q3",(char*) 0 ) ; //calling q3.c
		}
		else  //checking second child correctly or not
		{
			fprintf(stderr, "Error! second child doesn't created successfully\n");
			exit(1);
		}
	}
	else if( fork_read == 0 )  //first child
	{
		close(0);
		dup(pipe_end[0]);  //setting pipe_end[0] (read end of pipe) to STDIN
		close(pipe_end[0]);        
		
		/*file descriptor table now
		  0 :pipe_end[0] 
		  1 :file2
	  	  2 :stderr	  
		 */

					//now q2.c will take input from pipe_end[0] and output in file2
					
		close(pipe_end[1]);  //closing write end of pipe
		execl( "./q2" , "./q2" , (char*) 0 );  // calling q2.c 

	}
	else    //checking first child created or not
	{
		fprintf(stderr ,"Error! first child doesn't created sucessfully\n");
		exit(1);
	}
				//closing both end of pipe
	close(pipe_end[0]);
	close(pipe_end[1]);
	return 0 ;	
}	
