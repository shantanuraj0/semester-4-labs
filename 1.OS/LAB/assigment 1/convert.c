#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>


void handler2( )
{
	fprintf(stderr , "Child process killed\n");
	exit(1);
}

void handler3( )
{
	fprintf(stderr , "Killed due to pipe reader died!\n");
	exit(1);
}


int main(int argc , char* argv[])
{
	signal(SIGTERM , handler2 );
	signal(SIGPIPE , handler3 ) ;

	char buf[BUFSIZ];

	int fd  = read(0 , buf , BUFSIZ);

        //char char_input = '\0';

	int p=0 ;

        while(p < fd )
        {
                /*char_input = getchar();

                if(char_input == EOF)
                {
                       // printf("\n");
                        exit(0);
                } */
		

                if( isupper (buf[p] ) )
                {
                        buf[p] = buf[p] + 32;
                }
                else if(islower(buf[p]))
                {
                        buf[p] = buf[p] - 32 ;
                }

                p++;


        }
	write(1 , buf , fd );

exit(0);

}
