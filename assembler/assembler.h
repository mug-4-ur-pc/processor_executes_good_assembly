/*!
 * @file
 * @brief Header for assembler file compiler.
 */

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_




/*============================ Including headers ============================*/


#include "../errors/errors.h"
#include "../libs/text_edit.h"
#include "../commands.h"

#include <stdio.h>
#include <stdbool.h>




/*============================ Types declaration ============================*/

/*!
 * Structure includes input and output data.
 */
typedef struct io_t_
{
	char* input;  /*!< source code string.                                   */
	char* output; /*!< output data.                                          */
}
io_t;

/*!
 * Type of label.
 */
typedef struct label_t_
{
	char    name[MAX_TOKEN_SIZE]; /*!< name of label.                        */
	addr_t  address;              /*!< address of label.                     */
	size_t  use_amount;           /*!< amount of instructions 
	                                   which use this label.                 */
	size_t  capacity;             /*!< capacity of array with places 
	                                   where this label was used.            */
	addr_t* use;                  /*!< places where this label was used.     */
}
label_t;

/*!
 * Label table type.
 */
typedef struct label_table_t_
{
	label_t* table; /*!< array with labels.                                  */
	size_t size;    /*!< size of label table.                                */
	size_t capacity; /*!< capacity of label's table.                          */
}
label_table_t;

/*!
 * State's of compilation type.
 */
typedef struct assembler_state_t_
{
	io_t          io;     /*!< input/output files.                           */
	label_table_t labels; /*!< table of lables that are contained.           */
	addr_t        ip;     /*!< current instruction pointer.                  */
	size_t        pos;    /*!< position in input file.                       */
	proc_error_t  error;  /*!< error that occured during the 
	                           compilation process.                          */
}
*assembler_state_t;




/*========================== Functions declaration ==========================*/

/*!
 * This function create empty processor executable file.
 *
 * @return pointer to opened file.
 */
FILE* create_executable
(
 	const char* fname /*!< [in] file name.                                   */
);

/*!
 * This function writes bytes into output string and moves instruction pointer.
 */
void write_bytes 
(
	assembler_state_t state, /*!< [in,out] compilation state.                */
	const void*       data,  /*!< [in]     data to be written.               */
	size_t            size   /*!< [in]     size of input data.               */
);

/*!
 * assembler_state_t object constructor
 *
 * @return assembler_state_t object if success else NULL
 */
assembler_state_t asm_state_init
(
	FILE* in    /*!< [in] input of compilation.                              */
);

/*!
 * Deconstructor of assembler_state_t object.
 *
 * @return always NULL.
 */
assembler_state_t asm_state_delete
(
	assembler_state_t state /*!< [in,out] state.                             */
);

/*!
 * Removes all comments from source code.
 */
void remove_comments 
(
	assembler_state_t state /*!< [in,out] compilation state.                 */
);

/*!
 * It writes signature and version into output file.
 */
void write_header 
(
	assembler_state_t state /*!< [in,out] compilation state.                 */
);

/*!
 * Insert labels addresses instead of their ids.
 *
 * @return success of this operation.
 */
bool insert_labels_addresses
(
	assembler_state_t state /*!< [in,out] compilation state.                 */
);

/*!
 * This function compile pegas file.
 *
 * @return error code that occured during the execution.
 */
proc_error_t compile
(
 	FILE* output, /*!< [out] output file.                                    */
	FILE* input   /*!< [in]  input file.                                     */
);

/*!
 * Compile next command from input.
 *
 * @return success of this operation.
 */
bool compile_next 
(
	assembler_state_t state /*!< [in,out] compilation state.                 */
);

/*!
 * Increase capacity of label's table.
 *
 * @return success of this operation.
 */
bool increase_labels_capacity 
(
	assembler_state_t state /*!< compilation state.                          */
);

/*!
 * Update labels table.
 *
 * @return success of this operation.
 */
bool update_label 
(
	assembler_state_t state,               /*!< [in,out] compilation state.  */
	const char*       name,                /*!< [in]     label name.         */
	bool              is_label_declaration /*!< [in]     is this function 
	                                                     used with new label
	                                                     declaration.        */
);

/*!
 * This function finds particular label in label's table.
 *
 * @return pointer to label if it exists. Else it returns NULL.
 */
label_t* find_label
(
	const label_table_t labels, /*!< [in] label's table.                     */
	const char* name            /*!< [in] name of label for searching.       */
);

/*!
 * Initialize new label at the end of label table.
 *
 * @return success of this operation.
 */
bool create_label 
(
	assembler_state_t state, /*!< [in,out] compilation state                 */
	const char*       name   /*!< [in]     label's name.                     */
);

/*!
 * This function adds place in source code where the particular label 
 * was occured.
 */
bool add_code_place
(
	label_t*      label, /*!< [in,out] label that occured in source code.    */
	addr_t        addr,  /*!< [in]     address of label in source code.      */
	proc_error_t* err    /*!< [out]    pointer to variable 
	                                   in which error will be written.       */
);

/*!
 * This function compiles given token.
 *
 * @return success of this operation.
 */
bool compile_cmd 
(
	assembler_state_t state, /*!< [in,out] compilation state.                */
	const char*       token  /*!< [in]     token to be compiled.             */
);

/*!
 * Handle command without any arguments.
 *
 * @return success of this operation.
 */
bool handle_cmd_NO_ARGS 
(
	assembler_state_t state,      /*!< [in,out] compilation state.           */
	int               instruction /*!< [in]     the command's number.        */
);

/*!
 * Handle command with label argument.
 *
 * @return success of this operation.
 */
bool handle_cmd_LABEL_ARG
(
	assembler_state_t state,      /*!< [in,out] compilation state.           */
	int               instruction /*!< [in]     the command's number.        */
);

/*!
 * Handle command with memory argument.
 *
 * @return success of this operation.
 */
bool handle_cmd_MEMORY_ARG
(
	assembler_state_t state,      /*!< [in,out] compilation state.           */
	int               instruction /*!< [in]     the command's number.        */
);

/*!
 * This function writes instruction into output buffer.
 */
void write_instruction 
(
	assembler_state_t state,      /*!< [in,out] compilation state            */
	int               instruction /*!< [in]     instruction 
	                                            which will be written.       */
);

/*!
 * This function writes argument into output buffer.
 */
void write_arg
(
	assembler_state_t state, /*!< [in,out] compilation state                 */
	const void*       arg,    /*!< [in]     pointer to argument.             */
	size_t            size    /*!< [in]     argument size.                   */
);

/*!
 * This function checks is argument is address.
 *
 * If argument is address function will extract address variable into
 * extracted_addr variable.
 * If argument isn't an address function copies arg 
 * to extracted_addr variable.
 *
 * @return is argument address.
 */
bool is_addr
(
	char* arg,           /*!< [in]  input argument.                          */
	char* extracted_addr /*!< [out] string in which extracted 
	                                address variable will be written.        */
);

/*!
 * This function checks is an argument register and parse it.
 *
 * @return is an argument register.
 */
bool is_reg
(
	char*  arg, /*!< [in]  input argument.                                   */
	reg_t* reg  /*!< [out] pointer to value in which 
	                       parsed value will be written.                     */
);

/*!
 * This function checks is an argument constant value and parse it.
 *
 * @return is an argument constant value.
 */
bool is_const
(
	char*              arg, /*!< [in]  input argument.                       */
	processor_value_t* val  /*!< [out] pointer to value in which 
	                                   parsed value will be written.         */
);




#endif // ifndef ASSEMBLER_H_
