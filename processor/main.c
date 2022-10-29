/*!
 * @file Main file for soft processpr.
 */



#include "processor.h"

#include <stdio.h>
#include <string.h>


int main (int argc, char* argv[])
{
	if (argc != 2)
	{
		fputs("Wrong amount of arguments.\n", stderr);
		return 1;
	}

	if (strcmp(get_ext(argv[1]), EXEC_EXT) != 0)
	{
		fputs("Wrong file extension.\n", stderr);
		return 1;
	}

	FILE* input = fopen(argv[1], "r");
	if (!input)
	{
		fputs("File cannot be opened.\n", stderr);
		return 1;
	}

	int success = (run(input) == NO_PROC_ERR) ? 0 : 1;

	fclose(input);

	return success;
}
