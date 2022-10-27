/*!
 * @file Main file for assembler.
 */



#include "assembler.h"

#include <stdio.h>
#include <string.h>


int main (int argc, char* argv[])
{
	if (argc != 2)
	{
		fputs("Wrong amount of arguments.\n", stderr);
		return 1;
	}

	if (strcmp(get_ext(argv[1]), ASM_EXT) != 0)
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

	FILE* output = create_executable(argv[1]);
	if (!output)
	{
		fputs("Output file cannot be created.\n", stderr);
		return 1;
	}

	if (compile(output, input) != NO_PROC_ERR)
		return 1;

	fclose(output);
	fclose(input);

	return 0;
}
