/*!
 * @file
 * @brief This file contains processor errors description
 *        and functions to work with them.
 */

#ifndef PROC_ERRORS_H_
#define PROC_ERRORS_H_




/*============================ Including headers ============================*/




/*============================ Types declaration ============================*/

/*!
 * List of possible processor's errors.
 */
typedef enum proc_error_t_
{
	NO_PROC_ERR    = 0, /*!< no error.                                       */
	ALLOC_ERR       = 1, /*!< allocation error.                              */
	UNKNOWN_CMD     = 2, /*!< unknown command.                                */
	WRONG_TOKEN     = 3, /*!< wrong token.                                    */
	WRONG_ARG       = 4, /*!< wrong argument.                                 */
	MISSING_ARG     = 5, /*!< argument doesn't exists.                        */
	UNKNOWN_LABEL   = 6, /*!< unknown label.                                 */
	UNKNOWN_INSTR   = 7, /*!< unknown instruction number.                    */
	WRONG_SIGNATURE = 8  /*!< wring pegas signature.                         */
}
proc_error_t;




/*========================= Functions implementation ========================*/

/*!
 * Print processor's error in stderr stream.
 */
void print_error
(
 	proc_error_t err, /*!< [in] error.                                       */
	const char*  str  /*!< [in] string that will be printed 
	                            after error description.                     */
);




#endif // ifndef PROC_ERRORS_H_
