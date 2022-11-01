/*!
 * @file
 * @brief Header for processor file decompiler.
 */

#ifndef DISASSEMBLER_H_
#define DISASSEMBLER_H_




/*============================ Including headers ============================*/


#include "../errors/errors.h"
#include "../commands.h"

#include <stdio.h>
#include <stdbool.h>




/*============================ Types declaration ============================*/

/*!
 * State of processor.
 */
typedef struct disasm_state_t_
{
	addr_t         ip;              /*!< instruction pointer.                */
	unsigned char* instructions;    /*!< array with instructions.            */
	size_t         instr_size;      /*!< size of array with instructions.    */
	proc_error_t   error;           /*!< error code which happened during 
	                                     the execution.                      */
	addr_t*        labels;          /*!< sorted array with labels.           */
	size_t         labels_capacity; /*!< capacity of array with labels.      */
	size_t         labels_amount;   /*!< size of array with labels.          */
	size_t         cur_label;       /*!< index of next label.                */
	FILE*          output;          /*!< output file.                        */
}
*disasm_state_t;




/*========================== Functions declaration ==========================*/

/*!
 * This function create empty file with processor's assembler code.
 *
 * @return pointer to opened file.
 */
FILE* create_asm_file
(
 	const char* fname /*!< [in] file name.                                   */
);

/*!
 * Decompile binary file with .pegas extension.
 *
 * @return error code.
 */
proc_error_t decompile
(
	FILE* output, /*!< [in,out] output file.                                 */
 	FILE* input   /*!< [in]     input compiled file.                         */
);

/*!
 * Initialize disassembler state.
 *
 * @return disassembler state if success else NULL.
 */
disasm_state_t disasm_init
(
	FILE* output, /*!< [in] output file with source code.                    */
 	FILE* input   /*!< [in] input file with instructions.                    */
);

/*!
 * Check file signature for correctness.
 *
 * @return correctness of signature.
 */
int check_signature 
(
	disasm_state_t disasm /*!< [in,out] disassembler state.                  */
);

/*!
 * Delete information of disassembler state and free memory.
 *
 * @return NULL.
 */
disasm_state_t disasm_delete 
(
	disasm_state_t disasm /*!< [in,out] disassembler state.                  */
);

/*!
 * Process next instruction.
 *
 * @return Success of this operation.
 */
int disasm_process
(
	disasm_state_t disasm /*!< [in,out] disassembler state.                  */
);

/*!
 * This function gets instruction's argument, prints it and moves
 * instruction pointer.
 *
 * @return correctness of argument.
 */
int print_arg
(
	disasm_state_t disasm,  /*!< [in,out] disassembler state.                */
	char           instr,   /*!< [in]     instruction number.                */
	arg_t          arg_type /*!< [in]     argument type.                     */
);

/*!
 * Get address as label type argument, print it and move instruction pointer.
 *
 * @return success of this operation.
 */
int print_label_arg 
(
	disasm_state_t disasm /*!< [in,out] disassembler state.                  */
);

/*!
 * This function gets memory argument, prints it 
 * and moves the instruction pointer.
 *
 * @return correctness of argument.
 */
int print_mem_arg
(
	disasm_state_t disasm, /*!< [in,out] disassembler state.                 */
	char           instr   /*!< [in]     instruction number.                 */
);

/*!
 * Get labels array and assign it to disasm->labels.
 *
 * @return success of this operation.
 */
int get_labels 
(
	disasm_state_t disasm /*!< [in,out] dissambler state.                    */
);

/*!
 * Get size of instruction's argument.
 *
 * @return size of argument.
 */
size_t arg_size 
(
	unsigned char instr /*!< [in] instruction.                               */
);

/*!
 * If label doesn't exists, add it to the disasm->labels array.
 *
 * @return success of this operation.
 */
int update_label 
(
	disasm_state_t disasm,   /*!< [in,out] disassembler state.               */
	addr_t         use_place /*!< [in]     address of the place 
	                                       where this label was occured.     */
);

/*!
 * Find the label in the array using binary search.
 *
 * @return position of a find element or a max element 
 *         which is less than nedeed.
 */
size_t find_label 
(
	const disasm_state_t disasm,  /*!< [in,out] disassembler state.          */
	addr_t               addr,    /*!< [out]    address which are 
	                                            needed to find.              */
	bool*                was_find /*!< [out]    is array 
	                                            contains needed label.       */
);

/*!
 * This function inserts new label address to the labels array.
 *
 * @note this function doesn't change the array's capacity.
 * @note this function doesn't change the variable disasm->labels_amount
 */
void insert_label 
(
	disasm_state_t disasm, /*!< [in,out] dissambler state.                   */
	addr_t         addr,   /*!< [in]     address which will be inserted.     */
	size_t         where   /*!< [in]     the place where the address 
	                                     will be inserted.                   */
);

/*!
 * Increase capacity of array with labels.
 *
 * @return success of this operation.
 */
bool increase_labels_capacity 
(
	disasm_state_t disasm /*!< [in,out] disassembler state.                  */
);




#endif // ifndef DISASSEMBLER_H_
