/*!
 * @file
 * @brief Function's implementation for assembler file decompiler.
 */



/*============================ Including headers ============================*/


#include "disassembler.h"
#include "../libs/others.h"
#include "../libs/logging.h"
#include "../libs/text_edit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>




/*========================= Functions implementation ========================*/


FILE* create_asm_file (const char* fname)
{
	if_log(is_bad_byte_ptr(fname), ERROR,
		return NULL;)

	size_t fname_len = strlen(fname) + 1;
	char* output_name = calloc(fname_len + ASM_EXT_SIZE, 1);
	if (!output_name)
	{
		print_error(ALLOC_ERR, "output file name");
		return NULL;
	}

	strcpy(output_name, fname);
	change_ext(output_name, ASM_EXT);

	return fopen(output_name, "w");
}


proc_error_t decompile (FILE* output, FILE* input)
{
	if_log (is_bad_mem(output, sizeof *output), ERROR,
		return ALLOC_ERR;)

	if_log (is_bad_mem(input, sizeof *input), ERROR,
		return ALLOC_ERR;)

	disasm_state_t disasm = disasm_init(output, input);
	if (!disasm)
	{
		print_error(ALLOC_ERR, "disassembler state");
		return ALLOC_ERR;
	}

	if (!check_signature(disasm))
	{
		print_error(WRONG_SIGNATURE, "");
		return WRONG_SIGNATURE;
	}

	if (!get_labels(disasm))
	{
		proc_error_t err = disasm->error;
		disasm_delete(disasm);
		return err;
	}

	while (disasm_process(disasm))
		continue;

	proc_error_t err = disasm->error;
	disasm_delete(disasm);

	return err;
}


int check_signature (disasm_state_t disasm)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), ERROR,
		return 0;)

	if (disasm->instr_size < sizeof SIGNATURE + sizeof VERSION)
		return 0;

	if (*(signature_t*) disasm->instructions != SIGNATURE)
		return 0;

	version_t version = *(version_t*) (disasm->instructions + sizeof SIGNATURE);
	if (version != VERSION)
	{
		printf("Incompatible file version: %d > %d\n", version, VERSION);
		return 0;
	}

	disasm->ip += sizeof SIGNATURE + sizeof VERSION;

	return 1;
}


disasm_state_t disasm_init (FILE* output, FILE* input)
{
	if_log (is_bad_mem(input, sizeof *input), ERROR,
		return NULL;)

	if_log (is_bad_mem(output, sizeof *output), ERROR,
		return NULL;)

	disasm_state_t disasm = (disasm_state_t) calloc(1, sizeof *disasm);
	if (!disasm)
		return NULL;

	disasm->ip = 0;
	disasm->error = NO_PROC_ERR;
	
	disasm->instructions = (unsigned char*) read_file(input,
	                                                  &disasm->instr_size);
	if (!disasm->instructions)
		return disasm_delete(disasm);

	disasm->labels          = NULL;
	disasm->cur_label       = 0;
	disasm->labels_amount   = 0;
	disasm->labels_capacity = 1;
	disasm->output          = output;

	return disasm;
}


disasm_state_t disasm_delete (disasm_state_t disasm)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), WARNING,
		return NULL;)

	if (disasm->instructions)
		free(disasm->instructions);

	if (disasm->labels)
		free(disasm->labels);

	free(disasm);
	return NULL;
}


#define DEF_CMD(NAME_, NUM_, ARGS_, ...)                                      \
	case NUM_:                                                                \
		fputs("\t"#NAME_, disasm->output);                                        \
		if (!print_arg(disasm, instruction, ARGS_))                           \
		{                                                                     \
			disasm->error = WRONG_ARG;                                        \
			print_error(WRONG_ARG, #NAME_);                                   \
			return 0;                                                         \
		}                                                                     \
		break;

int disasm_process (disasm_state_t disasm)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), ERROR,
		return 0;)

	if (disasm->ip >= disasm->instr_size)
		return 0;

	if (disasm->labels[disasm->cur_label] == disasm->ip)
		fprintf(disasm->output, "L%lu:\n", disasm->cur_label++);

	unsigned char instruction = disasm->instructions[disasm->ip++];
	
	switch (instruction & (~(unsigned char) ADDR_ARG) & (~(unsigned char) REG_ARG))
	{
		#include "../DEF_CMD"
		default:
			disasm->error = UNKNOWN_INSTR;
			print_error(UNKNOWN_INSTR, "");
			return 0;
	}

	return 1;
}

#undef DEF_CMD


int print_arg (disasm_state_t disasm, char instr, arg_t arg_type)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), ERROR,
		return 0;)

	char arg_type_str[MAX_TOKEN_SIZE];
	switch (arg_type)
	{
		case NO_ARGS:
			putc('\n', disasm->output);
			return 1;

		case LABEL_ARG:
			return print_label_arg(disasm);

		case MEMORY_ARG:
			return print_mem_arg(disasm, instr);

		default:
			sprintf(arg_type_str, "%u", arg_type);
			write_log("Wrong arg type", arg_type_str, ERROR, 1);
			return 0;
	}
}


int print_label_arg (disasm_state_t disasm)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), ERROR,
		return 0;)
	
	addr_t addr = 0;
	if (disasm->ip + sizeof addr > disasm->instr_size)
		return 0;

	memcpy(&addr, disasm->instructions + disasm->ip, sizeof addr);
	disasm->ip += sizeof *disasm->labels;

	bool was_find = true;
	size_t label_num = find_label(disasm, addr, &was_find);
	fprintf(disasm->output, "\tL%lu\n", label_num);

	return 1;
}


bool get_value (disasm_state_t disasm, void* buff, size_t size)
{
		if (disasm->ip + size > disasm->instr_size)
			return false;

		memcpy(buff, disasm->instructions + disasm->ip, size);
		disasm->ip += size;
		return true;
	
}


int print_mem_arg (disasm_state_t disasm, char instr)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), ERROR,
		return 0;)

	if (instr & REG_ARG)
	{
		reg_t reg = REG_ax;
		if (get_value(disasm, &reg, sizeof reg))
			return 0;

		fputc('\t', disasm->output);
		if (instr & ADDR_ARG)
		{
			addr_t offset = 0;
			if (get_value(disasm, &offset, sizeof offset))
				return 0;

			if (offset)
				fprintf(disasm->output, "%llu", offset);
			fprintf(disasm->output, "[%cx]\n", 'a' + reg);
		}

		fprintf(disasm->output, "%cx\n", 'a' + reg);
	}
	else if (instr & ADDR_ARG)
	{
		processor_value_t addr;
		if (get_value(disasm, &addr, sizeof addr))
			return 0;

		fprintf(disasm->output, "\t[%d]\n", addr);
	}
	else
	{
		processor_value_t val = 0;
		if (get_value(disasm, &val, sizeof val))
			return 0;

		fprintf(disasm->output, "\t%d\n", val);
	}

	return 1;
}


#define DEF_CMD(NAME_, NUM_, ARG_TYPE_, ...)                                  \
	case NUM_:                                                                \
		if (ARG_TYPE_ == LABEL_ARG)                                           \
		{                                                                     \
			if (!update_label(disasm, ip))                                    \
				return 0;                                                     \
			ip += sizeof *disasm->labels;                                     \
		}                                                                     \
		else if (ARG_TYPE_ == MEMORY_ARG)                                     \
			ip += arg_size(instruction);                                      \
		break;

int get_labels (disasm_state_t disasm)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), ERROR,
		return 0;)

	addr_t ip = disasm->ip;
	while (ip < disasm->instr_size)
	{
		unsigned char instruction = disasm->instructions[ip++];
		switch (instruction & ~REG_ARG & ~ADDR_ARG)
		{
			#include "../DEF_CMD"
		}
	}

	return 1;
}

#undef DEF_CMD


size_t arg_size (unsigned char instr)
{
	return (instr & REG_ARG) ? sizeof (reg_t) : sizeof (processor_value_t);
}


int update_label (disasm_state_t disasm, addr_t use_place)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), ERROR,
		return 0;)

	addr_t addr;
	memcpy(&addr, disasm->instructions + use_place, sizeof addr);

	bool was_find = false;
	size_t label_num = find_label(disasm, addr, &was_find);
	if (was_find)
		return 1;

	if (++disasm->labels_amount >= disasm->labels_capacity)
	{
		if (!increase_labels_capacity(disasm))
		{
			disasm->error = ALLOC_ERR;
			print_error(ALLOC_ERR, "disasm->labels");
			return 0;
		}
	}

	insert_label(disasm, addr, label_num);
	return 1;
}


size_t find_label (const disasm_state_t disasm, addr_t addr, bool* was_find)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), ERROR,
		return 0;)

	if_log (is_bad_mem(was_find, sizeof *was_find), ERROR,
		return 0;)

	if (!disasm->labels)
	{
		*was_find = false;
		return 0;
	}

	size_t left  = 0;
	size_t mid   = 0;
	size_t right = disasm->labels_amount - 1;

	while (right - left > 1)
	{
		mid = (left + right) / 2;

		if (disasm->labels[mid] == addr)
		{
			*was_find = true;
			return mid;
		}

		if (disasm->labels[mid] < addr)
			left = mid;
		else
			right = mid;
	}

	if (disasm->labels[left] == addr)
	{
		*was_find = true;
		return left;
	}
	else if (disasm->labels[right] == addr)
	{
		*was_find = true;
		return right;
	}

	if (disasm->labels[right] < addr)
		mid = right + 1;
	else if (addr < disasm->labels[left])
		mid = left;
	else
		mid = right;

	*was_find = false;
	return mid;
}

	
void insert_label (disasm_state_t disasm, addr_t addr, size_t where)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), ERROR,
		return;)
	
	for (size_t i = disasm->labels_amount - 1; i > where; --i)
		disasm->labels[i] = disasm->labels[i - 1];

	disasm->labels[where] = addr;
}


bool increase_labels_capacity (disasm_state_t disasm)
{
	if_log (is_bad_mem(disasm, sizeof *disasm), ERROR,
		return false;)

	addr_t* realloc_check = (addr_t*) realloc(disasm->labels,
	                                          disasm->labels_capacity << 1);

	if (!realloc_check)
		return false;

	disasm->labels = realloc_check;
	disasm->labels_capacity <<= 1;
	return true;
}
