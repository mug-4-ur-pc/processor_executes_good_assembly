/*!
 * @file
 * @brief This file includes implementation of functions that 
 *        handle processor errors.
 */



/*============================ Including headers ============================*/


#include "../libs/logging.h"
#include "errors.h"

#include <stdio.h>




/*============================= Static functions ============================*/


static void print_err_text (const char* s1, const char* s2)
{
	fputs("===> ", stderr);
	fputs(s1, stderr);
	fputs(s2, stderr);
	fputc('\n', stderr);
}




/*========================= Functions implementation ========================*/


void print_error (proc_error_t err, const char *str)
{
	switch (err)
	{
		case NO_PROC_ERR: break;
		case ALLOC_ERR:
			print_err_text("Memory allocation error on ", str);
			break;

		case UNKNOWN_CMD:
			print_err_text("Wrong command ", str);
			break;

		case WRONG_TOKEN:
			print_err_text("Wrong token ", str);
			break;

		case WRONG_ARG:
			print_err_text("Wrong argument ", str);
			break;

		case MISSING_ARG:
			print_err_text("You missed the argument of type ", str);
			break;

		case UNKNOWN_LABEL:
			print_err_text("Label was not declared: ", str);
			break;

		case UNKNOWN_INSTR:
			print_err_text("Wrong instruction!", str);
			break;

		case WRONG_SIGNATURE:
			print_err_text("Wrong pegas signature.", str);
			break;
	}
}
