/*!
 * @file
 * @brief This file includes some hashing algorithms implementations.
 */




/*================= Connecting headers ==================*/


#include "others.h"
#include "logging.h"
#include "hash.h"




/*=================== Global functions ===================*/


uint64_t pearson_hash64 (const void* data, size_t len)
{	
	if (!len)
		return 0;

	if_log (is_bad_mem(data, len), ERROR)
		return 0;

	uint64_t hash64 = len;
	const unsigned char* uchar_data = (const unsigned char*) data;

	for (size_t j = 0; j < sizeof hash64; ++j)
	{
		unsigned char hash8 = ((uchar_data[0] + j) % 256) ^ 89;

		for (size_t i = 1; i < len; ++i)
			hash8 = (hash8 ^ uchar_data[i]) ^ 89;

		hash64 = (hash64 << 8) | hash8;
	}

	return hash64;
}
