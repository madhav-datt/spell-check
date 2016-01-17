/**
*
* Program to remove punctuations except (') apostrophe from file
*
* Copyright (C)   2015    Madhav Datt
* http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
*
*/

#include <stdio.h>
#include <ctype.h>

int main (void)
{
	char c;
	
	FILE* fp = fopen ("big1.txt", "r");
	FILE* fr = fopen ("words.txt","w");
	
	while ((c = fgetc (fp)) != EOF)
	{
		// Skip all punctuations except (')
		if (ispunct (c) && c != '\'')
			continue;
		else
			fputc (c, fr);
	}
	
	fclose (fp);
	fclose (fr);
	
	return 0;
}
