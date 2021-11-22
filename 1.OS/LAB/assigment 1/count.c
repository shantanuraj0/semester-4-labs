#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>



void handler( )
{
	fprintf(stderr, "Child process killed!\n");
	exit(1);
}


int main(int argc , char* argv[])
{
	signal(SIGTERM ,handler);
	

	int n=0 ;

	if(argc > 1)
	{
		n = atoi(argv[1]); // storing n of argument as integer from string
	//	fprintf(stderr, "hello there! : %d", n);
	}


        int non_alphabet_count = 0 ;
        char char_input = '\0';

       
       //fprintf(stderr, " EOF value : %d \n",EOF);
	if(n == 0 )
	{

       		 while(2)
       	 	{
                	char_input = getchar();

               		 if(char_input == EOF)
               		 {
                       		 fprintf(stderr,"Non alphabet count : %d \n" ,non_alphabet_count);
                       		 exit(0);
               		 }

			printf("%c",char_input);

               		 if( !(isalpha(char_input ) ) )
                        non_alphabet_count++;

               // putchar(char_input);


         }
	// putchar(char_input);

	} 

	else	// this case will handle read and write n characters
	{      
	       	int p = 0;

		while(1)
		{
			
			char_input = getchar();
	       

			if( char_input == EOF || n <= 0 )
			{
    				fprintf(stderr , "Non alphabetic count :%d \n",non_alphabet_count);
				exit(1);
			}

			printf("%c",char_input);

			if( !isalpha(char_input))
				non_alphabet_count++;

			n--;
		}
	//	fprintf(stderr , "Non alphabetic count :%d \n",non_alphabet_count);
	}
	return 0 ;

}
