/*!
 * @file
 * @brief Function's implementation for assembler file compiler.
 */



/*============================ Including headers ============================*/


#include "processor.h"
#include "../libs/others.h"
#include "../libs/logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>




/*======================== Macros & static functions ========================*/


#define PUSH(VAL__)                                                           \
{                                                                             \
	processor_value_t TO_PUSH_ = VAL__;                                       \
	stack_push(&proc->stack, &TO_PUSH_);                                      \
}

#define PUSH_ADDR(VAL__)                                                      \
{                                                                             \
	addr_t TO_PUSH_ = VAL__;                                                  \
	stack_push(&proc->address_stack, &TO_PUSH_);                              \
}

#define POP POP_FUNC_(proc)

static processor_value_t POP_FUNC_ (proc_state_t proc)
{
	processor_value_t val;
	stack_pop(&proc->stack, &val);
	return val;
}

#define TOP TOP_FUNC_(proc)

static processor_value_t TOP_FUNC_ (proc_state_t proc)
{
	processor_value_t val;
	stack_top(&proc->stack, &val);
	return val;
}

#define POP_ADDR POP_ADDR_FUNC_(proc)

static processor_value_t POP_ADDR_FUNC_ (proc_state_t proc)
{
	addr_t addr;
	stack_pop(&proc->address_stack, &addr);
	return addr;
}




/*========================= Functions implementation ========================*/


proc_error_t run (FILE* input)
{
	if_log (is_bad_mem(input, sizeof *input), ERROR,
		return ALLOC_ERR;)

	proc_state_t proc = proc_init(input);
	if (!proc)
	{
		print_error(ALLOC_ERR, "processor state");
		return 1;
	}

	if (!check_signature(proc))
	{
		print_error(WRONG_SIGNATURE, "");
		return WRONG_SIGNATURE;
	}

	while (proc_process(proc))
		continue;

	proc_error_t err = proc->error;
	proc_delete(proc);
	return err;
}


int check_signature (proc_state_t proc)
{
	if_log (is_bad_mem(proc, sizeof *proc), ERROR,
		return 0;)

	if (proc->instr_size < sizeof SIGNATURE + sizeof VERSION)
		return 0;

	if (*(signature_t*) proc->instructions != SIGNATURE)
		return 0;

	version_t version = *(version_t*) (proc->instructions + sizeof SIGNATURE);
	if (version != VERSION)
	{
		printf("Incompatible file version: %d > %d\n", version, VERSION);
		return 0;
	}

	proc->ip += sizeof SIGNATURE + sizeof VERSION;

	return 1;
}


proc_state_t proc_init (FILE* input)
{
	if_log (is_bad_mem(input, sizeof *input), ERROR,
		return NULL;)

	proc_state_t proc = (proc_state_t) calloc(1, sizeof *proc);
	if (!proc)
		return NULL;

	stack_constructor(proc->stack, processor_value_t);
	stack_constructor(proc->address_stack, addr_t);

	proc->ip = 0;
	proc->error = NO_PROC_ERR;
	
	proc->instructions = (unsigned char*) read_file(input, &proc->instr_size);
	if (!proc->instructions)
		return proc_delete(proc);

	return proc;
}


proc_state_t proc_delete (proc_state_t proc)
{
	if_log (is_bad_mem(proc, sizeof *proc), ERROR,
		return NULL;)

	stack_deconstructor(&proc->stack);
	stack_deconstructor(&proc->address_stack);

	if (proc->instructions)
		free(proc->instructions);

	free(proc);
	return NULL;
}


#define DEF_CMD(NAME_, NUM_, ARGS_, CODE_)                                    \
	case NUM_:                                                                \
		if (!get_arg(proc, instruction, ARGS_, &ADDR, &VAL_PTR, &VAL))        \
		{                                                                     \
			proc->error = WRONG_ARG;                                          \
			print_error(WRONG_ARG, #NAME_);                                   \
			return 0;                                                         \
		}                                                                     \
		CODE_;                                                                \
		break;

int proc_process (proc_state_t proc)
{
	if_log (is_bad_mem(proc, sizeof *proc), ERROR,
		return 0;)

	if (proc->ip >= proc->instr_size)
		return 0;

	processor_value_t* VAL_PTR;
	processor_value_t  VAL;
	addr_t             ADDR;
	unsigned char      instruction = proc->instructions[proc->ip++];
	
	switch (instruction & (~(unsigned char) ADDR_ARG) & (~(unsigned char) REG_ARG))
	{
		#include "../DEF_CMD"
		default:
			proc->error = UNKNOWN_INSTR;
			print_error(UNKNOWN_INSTR, "");
			return 0;
	}

	return 1;
}

#undef DEF_CMD


int get_arg (proc_state_t proc, char instr, arg_t arg_type,
             addr_t* addr, processor_value_t** val_ptr, processor_value_t* val)
{
	if_log (is_bad_mem(proc, sizeof *proc), ERROR,
		return 0;)

	if_log (is_bad_mem(val_ptr, sizeof *val_ptr), ERROR,
		return 0;)

	if_log (is_bad_mem(val, sizeof *val), ERROR,
		return 0;)

	char arg_type_str[MAX_TOKEN_SIZE];
	switch (arg_type)
	{
		case NO_ARGS:
			return 1;

		case LABEL_ARG:
			return get_label_arg(proc, addr);

		case MEMORY_ARG:
			return get_mem_arg(proc, instr, val_ptr, val);

		default:
			sprintf(arg_type_str, "%u", arg_type);
			write_log("Wrong arg type", arg_type_str, ERROR, 1);
			return 0;
	}
}


int get_label_arg (proc_state_t proc, addr_t* addr)
{
	if_log (is_bad_mem(proc, sizeof *proc), ERROR,
		return 0;)

	if_log (is_bad_mem(addr, sizeof *addr), ERROR,
		return 0;)

	if (proc->ip + sizeof *addr > proc->instr_size)
		return 0;

	memcpy(addr, proc->instructions + proc->ip, sizeof *addr);
	proc->ip += sizeof *addr;

	return 1;
}


int get_mem_arg (proc_state_t proc, char instr,
                 processor_value_t** val_ptr, processor_value_t* val)
{
	if_log (is_bad_mem(proc, sizeof *proc), ERROR,
		return 0;)

	if_log (is_bad_mem(val_ptr, sizeof *val_ptr), ERROR,
		return 0;)

	if_log (is_bad_mem(val, sizeof *val), ERROR,
		return 0;)

	if (instr & REG_ARG)
	{
		reg_t reg;
		if (proc->ip + sizeof reg > proc->instr_size)
			return 0;

		memcpy(&reg, proc->instructions + proc->ip, sizeof reg);
		*val_ptr  = proc->regs + reg;
		*val      = **val_ptr;
		proc->ip += sizeof reg;
		
		if (instr & ADDR_ARG)
		{
			*val_ptr = proc->mem + *val;
			*val     = **val_ptr;
		}
	}
	else if (instr & ADDR_ARG)
	{
		processor_value_t addr;
		if (proc->ip + sizeof addr > proc->instr_size)
			return 0;

		memcpy(&addr, proc->instructions + proc->ip, sizeof addr);
		*val_ptr  = proc->mem + addr;
		*val      = **val_ptr;
		proc->ip += sizeof addr;
	}
	else
	{
		if (proc->ip + sizeof *val > proc->instr_size)
			return 0;

		memcpy(val, proc->instructions + proc->ip, sizeof *val);
		*val_ptr  = NULL;
		proc->ip += sizeof *val;
	}

	return 1;
}


void redraw (const processor_value_t* mem)
{
	for (addr_t i = 0; i < VIDEO_MEM_SIZE; ++i)
	{
		putchar((mem[i]) ? '#' : ' ');
		putchar(' ');
		if (i % VIDEO_WIDTH == VIDEO_WIDTH - 1)
			putchar('\n');
	}
}
