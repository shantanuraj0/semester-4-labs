#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

int main(int argc , char* argv[])
{
	char ch = '\0';
	int count=0 ;

	while((ch = getchar()) !=EOF) 
	{
		if (! isalpha(ch))
			count++;
		putchar(ch);
	}
	fprintf(stderr, "NON alphabet : %d\n" , count);
	exit(0);
}
