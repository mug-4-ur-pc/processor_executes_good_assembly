/*!
 * @file
 * @brief Function's implementation for assembler file compiler.
 */



/*============================ Including headers ============================*/


#include "assembler.h"
#include "../errors/errors.h"
#include "../libs/others.h"
#include "../libs/logging.h"
#include "../libs/text_edit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




/*========================= Functions implementation ========================*/


FILE* create_executable (const char* fname)
{
	if_log(is_bad_byte_ptr(fname), ERROR,
		return NULL;)

	size_t fname_len = strlen(fname) + 1;
	char* output_name = calloc(fname_len + EXEC_EXT_SIZE, 1);
	if (!output_name)
	{
		print_error(ALLOC_ERR, "output file name");
		return NULL;
	}

	strcpy(output_name, fname);
	change_ext(output_name, EXEC_EXT);

	return fopen(output_name, "wb");
}


void write_bytes (assembler_state_t state, const void* data, size_t size)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return;)

	if_log (is_bad_mem(data, size), ERROR,
		return;)

	memcpy(state->io.output, data, size);
	state->ip += size;
}


assembler_state_t asm_state_init (FILE* in)
{
	if_log (is_bad_mem(in, sizeof *in), ERROR,
		return NULL;)

	assembler_state_t state = (assembler_state_t) calloc(sizeof *state, 1);
	if (!state)
		return NULL;

	size_t in_size   = 0;
	state->error     = NO_PROC_ERR;
	state->io.input  = read_file(in, &in_size);
	state->io.output = (char*) calloc(in_size, sizeof *state->io.output);
	if (!state->io.input)
		return asm_state_delete(state);

	state->ip              = 0;
	state->labels.size     = 0;
	state->labels.capacity = 100;
	state->labels.table    = (label_t*) calloc(sizeof *state->labels.table,
	                                           state->labels.capacity);
	if (!state->labels.table)
		return asm_state_delete(state);

	return state;
}


assembler_state_t asm_state_delete (assembler_state_t state)
{
	if_log (is_bad_mem(state, sizeof *state), WARNING,
		return NULL;)

	free(state->io.input);

	for (size_t i = 0; i < state->labels.size; ++i)
		free(state->labels.table[i].use);

	free(state->labels.table);
	free(state);

	return NULL;
}


void remove_comments (assembler_state_t state)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return;)

	char* ptr = state->io.input;

	while ((ptr = strchr(ptr, ';')) != NULL)
	{
		char* endl = strchr(ptr, '\n');
		if (!endl)
			endl = ptr + strlen(ptr);
		memset(ptr, '\n', endl - ptr);
	}
}


void write_header (assembler_state_t state)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return;)

	write_arg(state, &SIGNATURE, sizeof SIGNATURE);
	write_arg(state, &VERSION, sizeof VERSION);
}


bool insert_labels_addresses (assembler_state_t state)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return false;)

	for (size_t i = 0; i < state->labels.size; ++i)
	{
		label_t label = state->labels.table[i];
		if (label.address == 0)
		{
			state->error = UNKNOWN_LABEL;
			print_error(UNKNOWN_LABEL, label.name);
			return false;
		}

		for (size_t j = 0; j < label.use_amount; ++j)
		{
			state->io.output[label.use[j]] = label.address;
		}
	}
	return true;
}


bool increase_labels_capacity (assembler_state_t state)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return false;)

	label_t* new_ptr = (label_t*) realloc(state->labels.table,
			                              state->labels.capacity * 10
			                              * sizeof *state->labels.table);
		
	if (!new_ptr)
	{
		state->error = ALLOC_ERR;
		return false;
	}
	state->labels.table = new_ptr;
	state->labels.capacity *= 10;

	return true;
}


label_t* find_label (const label_table_t labels, const char* name)
{
	for (size_t i = 0; i < labels.size; ++i)
	{
		if (strncmp(labels.table[i].name, name, MAX_TOKEN_SIZE) == 0)
			return labels.table + i;
	}

	return NULL;
}


bool create_label (assembler_state_t state, const char* name)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return false;)

	if_log (is_bad_byte_ptr(name), ERROR,
		return false;)

	label_t* label = state->labels.table + state->labels.size;
	label->use_amount = 0;
	label->capacity   = 1;
	label->use        = NULL;
	label->address    = 0;
	strncpy(label->name, name, MAX_TOKEN_SIZE - 1);
	return true;
}


bool add_code_place (label_t* label, addr_t addr, proc_error_t* err)
{
	if_log (is_bad_mem(label, sizeof *label), ERROR,
		return false;)

	if_log (is_bad_mem(err, sizeof *err), ERROR,
		return false;)

	if (label->use_amount + 1 >= label->capacity)
	{
		addr_t* new_ptr = (addr_t*) realloc(label->use, label->capacity 
		                                    * 10 * sizeof *label->use);

		if (!new_ptr)
		{
			*err = ALLOC_ERR;
			return false;
		}
		label->use = new_ptr;
		label->capacity *= 10;
	}

	label->use[label->use_amount] = addr + 1;
	++label->use_amount;

	return true;
}


bool update_label (assembler_state_t state, const char* name,
                   bool is_label_declaration)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return false;)

	if_log (is_bad_byte_ptr(name), ERROR,
		return false;)

	label_t* label = find_label(state->labels, name);
	if (!label)
	{
		size_t new_index = state->labels.size;

		if (new_index + 1 >= state->labels.capacity
		    || !increase_labels_capacity(state))
		{
			state->error = ALLOC_ERR;
			return false;
		}

		if (!create_label(state, name))
			return false;
		
		if (is_label_declaration)
		{
			state->labels.table[new_index].address = state->ip;
		}
		else
		{
			if (!add_code_place(state->labels.table + new_index,
			                    state->ip, &state->error))
			{
				state->error = ALLOC_ERR;
				return false;
			}
		}

		++state->labels.size;
	}
	else
	{
		if (is_label_declaration)
		{
			label->address = state->ip;
		}
		else
		{
			if (!add_code_place(label, state->ip, &state->error))
			{
				state->error = ALLOC_ERR;
				return false;
			}
		}
	}

	return true;
}


#define DEF_CMD(CMD_, NUM_, ARG_, ...) else if (strcmp(#CMD_, token) == 0) \
                                        return handle_cmd_##ARG_(state, NUM_);

bool compile_cmd (assembler_state_t state, const char* token)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return false;)

	if_log (is_bad_byte_ptr(token), ERROR,
		return false;)

	if (false);
	#include "../DEF_CMD" // e.g. else if (strcmp("add", token) == 0)
	                      //      	return handle_cmd_NO_ARGS(state, 11);
	else
	{
		print_error(UNKNOWN_CMD, token);
		state->error = UNKNOWN_CMD;
		return false;
	}
}

#undef DEF_CMD


bool handle_cmd_NO_ARGS (assembler_state_t state, int instruction)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return false;)

	write_instruction(state, instruction);
	return true;
}


bool handle_cmd_LABEL_ARG (assembler_state_t state, int instruction)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return false;)

	int  was_read = 0;
	char label[MAX_TOKEN_SIZE];
	if (sscanf(state->io.input + state->pos, "%s%n", label, &was_read) != 1)
	{
		print_error(MISSING_ARG, "LABEL");
		state->error = MISSING_ARG;
		return false;
	}
	
	state->pos += was_read;
	if (!update_label(state, label, false))
	{
		state->error = ALLOC_ERR;
		print_error(ALLOC_ERR, label);
		return false;
	}

	write_instruction(state, instruction);
	write_arg(state, &state->ip, sizeof state->ip);

	return true;
}


bool handle_cmd_MEMORY_ARG (assembler_state_t state, int instruction)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return false;)

	char arg[MAX_TOKEN_SIZE] = {};
	int was_read = 0;
	if (sscanf(state->io.input + state->pos, "%s%n", arg, &was_read) != 1)
	{
		print_error(MISSING_ARG, "MEMORY");
		state->error = MISSING_ARG;
		return false;
	}

	state->pos              += was_read;
	reg_t             reg    = REG_ax;
	processor_value_t val    = 0;
	addr_t            offset = 0;
	char              extracted_addr[MAX_TOKEN_SIZE];
	if (is_addr(arg, extracted_addr, &offset))
		instruction |= ADDR_ARG;
	
	if (is_reg(extracted_addr, &reg))
	{
		instruction |= REG_ARG;
		write_instruction(state, instruction);
		write_arg(state, &reg, sizeof reg);
	}
	else if (is_const(extracted_addr, &val))
	{
		write_instruction(state, instruction);
		write_arg(state, &val, sizeof val);
	}
	else
	{
		print_error(WRONG_ARG, arg);
		state->error = WRONG_ARG;
		return false;
	}

	if (instruction & ADDR_ARG)
		write_arg(state, &offset, sizeof offset);

	return true;
}


bool compile_next (assembler_state_t state)
{
	if_log (is_bad_mem(state, sizeof *state), ERROR,
		return false;)

	char token[MAX_TOKEN_SIZE];
	char tmp[10];
	int  was_read = 0;

	if (sscanf(state->io.input + state->pos,
	           " %[^: \n\t\r\f\v]%1[:]%n", token, tmp, &was_read) == 2)
	{
		state->pos += was_read;
		if (!update_label(state, token, true))
		{
			if (state->error == ALLOC_ERR)
				print_error(ALLOC_ERR, "changing label's table");
			return false;
		}
		else
			return true;
	}
	else if (sscanf(state->io.input + state->pos, " %[A-Za-z]%n",
	                token, &was_read) == 1)
	{
		state->pos += was_read;
		return compile_cmd(state, token);
	}
	else if (sscanf(state->io.input + state->pos, " %s%n",
	                token, &was_read) == 1)
	{
		state->pos += was_read;
		print_error(WRONG_TOKEN, token);
		state->error = WRONG_TOKEN;
	}

	return false;
}


proc_error_t compile (FILE* output, FILE* input)
{
	if_log (is_bad_mem(output, sizeof *output), ERROR,
		return WRONG_ARG;)

	if_log (is_bad_mem(input, sizeof *input), ERROR,
		return WRONG_ARG;)

	assembler_state_t state = asm_state_init(input);

	remove_comments(state);

	write_header(state);

	while (compile_next(state))
		continue;

	insert_labels_addresses(state);

	proc_error_t err = state->error;
	if (err == NO_PROC_ERR)
		fwrite(state->io.output, sizeof *state->io.output, state->ip, output);

	asm_state_delete(state);
	return err;
}


void write_instruction (assembler_state_t state, int instruction)
{
	state->io.output[state->ip] = (char) instruction;
	++state->ip;
}


void write_arg (assembler_state_t state, const void* arg, size_t size)
{
	memcpy(state->io.output + state->ip, arg, size);
	state->ip += size;
}


bool is_addr (char* arg, char* extracted_val, addr_t* offset)
{
	char tmp[10];
	
	// 0x5B == '[' && 0x5D == ']'
	if (sscanf(arg, "\x5B%[^\x5D]%1[\x5D]", extracted_val, tmp) == 2)
	{
		*offset = 0;
		return true;
	}

	if (sscanf(arg, "%llu\x5B%[^\x5D]%1[\x5D]", offset, extracted_val, tmp) == 3)
		return true;

	strcpy(extracted_val, arg);
	return false;
}


bool is_reg (char* arg, reg_t* reg)
{
	char reg_letter = 0;
	char tmp;
	if (sscanf(arg, "%cx%c", &reg_letter, &tmp) == 1)
	{
		if ('a' <= reg_letter && reg_letter < 'a' + REGS_NUMBER)
		{
			*reg = reg_letter - 'a';
			return true;
		}
	}

	return false;
}


bool is_const (char* arg, processor_value_t* val)
{
	char tmp;
	if (sscanf(arg, "%d%c", val, &tmp) == 1)
		return true;

	return false;
}
