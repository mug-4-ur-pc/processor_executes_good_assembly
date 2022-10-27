/*!
 * @file
 * @brief This file includes source code of some small different functions.
 */




/*================= Connecting headers ==================*/


#include "others.h"
#include "logging.h"

#include <errno.h>
#include <unistd.h>
#include <stddef.h>




/*=================== Global functions ===================*/


inline bool is_bad_byte_ptr (const void* ptr)
{
	if (access((const char*) ptr, F_OK)
	    && errno == EFAULT)
		return true;
	else
		return false;
}


bool is_bad_mem (const void* ptr, size_t size)
{
	if_log (size == 0, WARNING)
		return true;

	for (size_t i = 0; i < size; ++i)
	{
		bool result = is_bad_byte_ptr(ptr + i);
		if (result)
				return true;
	}

	return false;
}
