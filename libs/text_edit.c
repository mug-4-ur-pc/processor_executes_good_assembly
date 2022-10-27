/*!
 * @file
 * @brief This file contains implementation of functions which
 * work with files and strings.
 */




/*============================ Including headers ============================*/


#include "text_edit.h"
#include "logging.h"
#include "others.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




/*========================= Functions implementation ========================*/


char* get_ext (const char* fname)
{
	if_log (is_bad_byte_ptr(fname), ERROR)
		return NULL;

	const char* last_point = NULL;
	const char* ptr = fname - 1;
	while ((ptr = strchr(ptr + 1, '.')) != NULL)
		last_point = ptr;

	if (!last_point)
		last_point = fname + strlen(fname);

	return (char*) ++last_point;
}


void change_ext (char* fname, const char* ext)
{
	if_log (is_bad_byte_ptr(fname), ERROR)
		return;

	if_log (is_bad_byte_ptr(ext), ERROR)
		return;
	
	char* ext_ptr = get_ext(fname);
	strcpy(ext_ptr, ext);
}


char* read_file (FILE* in, size_t* size)
{
	if_log (is_bad_mem(in, sizeof *in), ERROR)
		return NULL;

	fseek(in, 0, SEEK_END);
	size_t len = ftell(in);
	fseek(in, 0, SEEK_SET);

	char* str = (char*) calloc(len + 1, sizeof *str);
	if (!str)
	{
		*size = 0;
		return NULL;
	}

	fread(str, len, sizeof *str, in);
	*size = len;
	return str;
}
