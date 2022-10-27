/*!
 * @file
 * @brief This file contains functions to work with file and strings.
 */

#ifndef FILE_EDIT_H_
#define FILE_EDIT_H_




/*============================ Including headers ============================*/


#include <stdio.h>




/*========================== Functions declaration ==========================*/

/*!
 * It gets file extension from its name.
 *
 * @return file extension. If an error occured, it returns NULL.
 */
char* get_ext
(
	const char* fname /*!< [in] file name.                                   */
);

/*!
 * This function changes the extension in file name.
 *
 * @note This function doesn't allocate any memory. You need to do it
 *       by yourself.
 */
void change_ext
(
	char*       fname, /*!< [in,out] file name whose extension 
	                                 will be changed.                        */ 
	const char* ext   /*!< [in] new file extension.                         */
);

/*!
 * Read file.
 *
 * @return string that was read.
 */
char* read_file
(
	FILE*   in,  /*!< [in]  input file.                                      */
	size_t* size /*!< [out] pointer to variable which will be assigned
	                        readed bytes amount if success else 0.           */
);




#endif // ifndef FILE_EDIT_H_
