/*!
 * @file Main file for disassembler.
 */



#include "disassembler.h"
#include "../libs/text_edit.h"

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

	FILE* input = fopen(argv[1], "rb");
	if (!input)
	{
		fputs("File cannot be opened.\n", stderr);
		return 1;
	}
	
	FILE* output = create_asm_file(argv[1]);
	if (!output)
	{
		fputs("Output file cannot be created.\n", stderr);
		return 1;
	}

	int success = (decompile(output, input) == NO_PROC_ERR) ? 0 : 1;

	fclose(input);
	fclose(output);
	return success;
}
