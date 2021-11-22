#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>
#include<ctype.h>
#include<signal.h>
#include<setjmp.h>

static jmp_buf buffer;

void handler( )
{
	longjmp(buffer , 1 );
	fprintf(stderr , "Killing both children!\n");
	exit(1);
}


int main(int argc , char* argv[])
{
	signal(SIGALRM , handler);//generating signal when alarm raises
	
	int child1 , child2;
	

	if(setjmp(buffer))	
	{
		kill(child1 ,SIGTERM);
	        kill(child2 , SIGTERM);

		exit(1);	
	}

	
    	char array_1st_argument[10];	
      
         int i = 0;
        

        if(argc == 4)
	{
                for(i = 0 ; argv[1][i + 1] != '\0'; i++)
		{
			array_1st_argument[i] = argv[1][i+1];

		}
	  	int open_id = open(argv[2] , O_RDONLY);  //opening file1 to read
		if( open_id == -1 )	//handling error
		{
			fprintf(stderr,"Error! file1 not opened correctly\n");
			exit(1);
		}
		
		close (0);  //closing STDIN
		int dup_in_id = dup(open_id);//setting file1 as STDIN

		if(dup_in_id == -1)
		{
			fprintf(stderr ,"Error! file1 not duped coorectly\n");
			exit(1);
		}
		close(open_id);

	    int creat_id = creat( argv[3] , 0644) ;  //creating file2 to write
	
		if(creat_id == -1)//handling error
		{
			fprintf(stderr , "Error! file2 not created correctly\n") ;
		       exit(1);	
		}

		close(1); //closing STDOUT
		int dup_out_id = dup(creat_id);  //setting file2 as STDOUT

		if(dup_out_id == -1)	//handling error
		{
			fprintf(stderr,"Error! file2 not duped correctly\n");
			exit(1);
		}
		close(creat_id);

	}else if(argc == 3 && argv[1][0] != '-')
	{

		int open_id = open(argv[1] , O_RDONLY);  //opening file1 to read
		if( open_id == -1 )	//handling error
		{
			fprintf(stderr,"Error! file1 not opened correctly\n");
			exit(1);
		}
		
		close (0);  //closing STDIN
		int dup_in_id = dup(open_id);//setting file1 as STDIN

		if(dup_in_id == -1)
		{
			fprintf(stderr ,"Error! file1 not duped coorectly\n");
			exit(1);
		}
		close(open_id);

	    int creat_id = creat( argv[2] , 0644) ;  //creating file2 to write
	
		if(creat_id == -1)//handling error
		{
			fprintf(stderr , "Error! file2 not created correctly\n") ;
		       exit(1);	
		}

		close(1); //closing STDOUT
		int dup_out_id = dup(creat_id);  //setting file2 as STDOUT

		if(dup_out_id == -1)	//handling error
		{
			fprintf(stderr,"Error! file2 not duped correctly\n");
			exit(1);
		}
		close(creat_id);
	}else if(argc == 3 && argv[1][0] == '-')
	{
	              for(i = 0 ; argv[1][i + 1] != '\0'; i++)
		{
			array_1st_argument[i] = argv[1][i+1];

		}
	  	int open_id = open(argv[2] , O_RDONLY);  //opening file1 to read
		if( open_id == -1 )	//handling error
		{
			fprintf(stderr,"Error! file1 not opened correctly\n");
			exit(1);
		}
		
		close (0);  //closing STDIN
		int dup_in_id = dup(open_id);//setting file1 as STDIN

		if(dup_in_id == -1)
		{
			fprintf(stderr ,"Error! file1 not duped coorectly\n");
			exit(1);
		}
		close(open_id);	
	}else if(argc == 2 && argv[1][0] != '-')
	{
			int open_id = open(argv[1] , O_RDONLY);  //opening file1 to read
		if( open_id == -1 )	//handling error
		{
			fprintf(stderr,"Error! file1 not opened correctly\n");
			exit(1);
		}
		
		close (0);  //closing STDIN
		int dup_in_id = dup(open_id);//setting file1 as STDIN

		if(dup_in_id == -1)
		{
			fprintf(stderr ,"Error! file1 not duped coorectly\n");
			exit(1);
		}
		close(open_id);	

	}else if(argc == 2 && argv[1][0] == '-')
	{
		         for(i = 0 ; argv[1][i + 1] != '\0'; i++)
		{
			array_1st_argument[i] = argv[1][i+1];

		}
	}
	array_1st_argument[i] = '\0';

	
	int pipe_end[2];	
				  //parent calls pipe sysytem call to create pipe file descriptors
	if(pipe(pipe_end) < 0 )  // creating pipe and handling error
	{	fprintf(stderr , "Error! pipe doesn't created coorectly\n");
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
			close(pipe_end[0]);	//closing both pipe end
			close(pipe_end[1]);

			alarm(15); // setting a alarm for 15 second to avoid hung of children

			wait(NULL); //waiting for the children to complete
			wait(NULL);

			alarm(0);  //resetting alarm to zero

			fprintf(stderr,"\nEnd of program!\n");
			exit(1);

		}
		else if( fork_write == 0 )  //second child
		{
			child2 = getpid();
			
			close(pipe_end[0]);
			close(1);	
			dup(pipe_end[1]);//setting pipe_end[1](write end of pipe) to STDOUT
			close(pipe_end[1]);
			
			
					//now convert.c will take input from file1 and output it into pipe_end[1]

			//close(pipe_end[0]);
			fprintf(stderr , "child process with process id : %d\n" , getpid());
			execl("convert","convert",(char*) 0 ) ; //calling convert.c
		}
		else  //checking second child correctly or not
		{
			fprintf(stderr, "Error! second child doesn't created successfully\n");
			exit(1);
		}
	}
	else if( fork_read == 0 )  //first child
	{
		child1 = getpid();

		close(pipe_end[1]);

		close(0);
		dup(pipe_end[0]);  //setting pipe_end[0] (read end of pipe) to STDIN
		close(pipe_end[0]);        
		


					//now count.c will take input from pipe_end[0] and output in file2
		fprintf(stderr,"Child process with process id : %d\n",getpid());			

		if(array_1st_argument[0] == '\0')
		execl( "count" , "count" , (char*) 0 );  // calling count.c 
		else
		execl("count","count", array_1st_argument,(char*)0 ) ;

	}
	else    //checking first child created or not
	{
		fprintf(stderr ,"Error! first child doesn't created sucessfully\n");
		exit(1);
	}
			
	return 0 ;
		
}	
