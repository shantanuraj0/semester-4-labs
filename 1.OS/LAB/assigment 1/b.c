#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

int main(int argc , char* argv[])
{
        int non_alphabet_count = 0 ;
        char char_input = '\0';

       // printf(" EOF value : %d \n",EOF);

        while(2)
        {
                char_input = getchar();

                if(char_input == EOF)
                {
                        fprintf(stderr,"Non alphabet count : %d \n" ,non_alphabet_count);
                        exit(0);
                }

                if( !(isalpha (char_input ) ) )
                        non_alphabet_count++;

                putchar(char_input);


        }

return 0 ;

}