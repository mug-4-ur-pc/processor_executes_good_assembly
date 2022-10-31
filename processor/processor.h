/*!
 * @file
 * @brief Header for bin interpreter.
 */

#ifndef PROCESSOR_H_
#define PROCESSOR_H_




/*============================ Including headers ============================*/


#include "../errors/errors.h"
#include "../libs/text_edit.h"
#include "../errors/errors.h"
#include "../commands.h"
#include "../libs/secure_stack.h"

#include <stdio.h>
#include <stdbool.h>




/*============================ Types declaration ============================*/

/*!
 * State of processor.
 */
typedef struct proc_state_t_
{
	processor_value_t mem[MEMORY_SIZE];  /*!< memory. First VIDEO_MEM_SIZE 
	                                          bytes are video memory.        */
	processor_value_t regs[REGS_NUMBER]; /*!< registers.                     */
	addr_t            ip;                /*!< instruction pointer.           */
	unsigned char*    instructions;      /*!< array with instructions.       */
	size_t            instr_size;        /*!< size of array with 
	                                          instructions.                  */
	stack_t           stack;             /*!< stack.                         */
	stack_t           address_stack;     /*!< stack with addresses 
	                                          of points of return.           */
	proc_error_t      error;             /*!< error code that occures
	                                          during the execution.          */
}
*proc_state_t;




/*========================== Functions declaration ==========================*/

/*!
 * Process instructions frmm input file.
 *
 * @return error code.
 */
proc_error_t run 
(
	FILE* input /*!< [in] input compiled file.                               */
);
/*!
 * Initilalize processor state.
 *
 * @return processor state if success else NULL.
 */
proc_state_t proc_init
(
	FILE* input /*!< [in] input file with instructions.                      */
);

/*!
 * Check file signature for correctness.
 *
 * @return correctness of signature.
 */
int check_signature 
(
	proc_state_t proc /*!< [in,out] processor state.                         */
);

/*!
 * Delete information of processor state and free memory.
 *
 * @return NULL.
 */
proc_state_t proc_delete 
(
	proc_state_t proc /*!< [in] processor state.                             */
);

/*!
 * Process next instruction.
 *
 * @return Success of this operation.
 */
int proc_process
(
	proc_state_t proc /*!< [in] processor state.                             */
);

/*!
 * This function gets instruction's argument and moves
 * instruction pointer.
 *
 * @return correctness of argument.
 */
int get_arg
(
	proc_state_t        proc,     /*!< [in,out] processor state.             */
	char                instr,    /*!< [in]     instruction number.          */
	arg_t               arg_type, /*!< [in]     argument type.               */
	addr_t*             addr,     /*!< [out]    pointer to value which will be
	                                            assigned read address.       */
	processor_value_t** val_ptr,  /*!< [out]    pointer to value which will be 
	                                           assigned read value's address.        */
	processor_value_t*  val       /*!< [out]    pointer to value which will be 
	                                           assigned read value.          */
);

/*!
 * Get address as label type argument and move instruction pointer.
 *
 * @return success of this operation.
 */
int get_label_arg 
(
	proc_state_t proc, /*!< [in,out] processor state.                        */
	addr_t*      addr  /*!< [out]    value which will be assigned address.   */
);

/*!
 * This function gets memory argument and moves the instruction pointer.
 *
 * @return correctness of argument.
 */
int get_mem_arg
(
	proc_state_t        proc,    /*!< [in,out] processor state.              */
	char                instr,   /*!< [in]     instruction number.           */
	processor_value_t** val_ptr, /*!< [out]    pointer to value which will be 
	                                           assigned read address.        */
	processor_value_t*  val      /*!< [out]    pointer to value which will be 
	                                           assigned read value.          */
);

/*!
 * Redraw picture using current state of video memory.
 */
void redraw
(
	const processor_value_t* mem /*!< [in] video memory.                     */
);




#endif // ifndef PROCESSOR_H_
