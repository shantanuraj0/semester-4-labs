#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

int main(int argc , char* argv[])
{

        char char_input = '\0';



        while(2)
        {
                char_input = getchar();

                if(char_input == EOF)
                {
                       // printf("\n");
                        exit(0);
                }


                if( isupper (char_input ) )
                {
                        char_input = char_input + 32;
                }
                else if(islower(char_input))
                {
                        char_input = char_input - 32 ;
                }

                putchar(char_input);


        }

return 0 ;

}
