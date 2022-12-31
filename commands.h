/*!
 * @file
 * @brief This file includes general command's format description.
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_




/*============================ Including headers ============================*/


#include <limits.h>
#include <inttypes.h>
#include <stddef.h>




/*============================ Types declaration ============================*/

/*!
 * Assembly file signature type.
 */
typedef uint64_t signature_t;

/*!
 * Processor version type.
 */
typedef uint32_t version_t;

/*!
 * Type of values which are processed by processor.
 */
typedef int processor_value_t;

/*!
 * Type of address.
 */
typedef unsigned long long addr_t;

/*!
 * Register type
 */
typedef uint8_t reg_t;

/*!
 * List of available register names.
 */
typedef enum reg_names_
{
	REG_ax = 0,
	REG_bx = 1,
	REG_cx = 2,
	REG_dx = 3,
	REG_ex = 4,
	REG_gx = 5,
	REG_hx = 6,
	REG_ix = 7,
	REG_jx = 8,
	REG_kx = 9,
	REG_lx = 10,
	REG_mx = 11,
	REG_nx = 12,
	REG_ox = 13,
	REG_px = 14,
	REG_qx = 15,
}
reg_names;

/*!
 * Enum with argument types.
 */
typedef enum arg_t_
{
	NO_ARGS    = 0, /*!< no arguments.                                       */
	LABEL_ARG  = 1, /*!< argument is label.                                  */
	MEMORY_ARG = 2, /*!< argument is register, constant or addrees.          */
}
arg_t;

/*!
 * Enum with types of memory argument.
 */
typedef enum memory_arg_t_
{
	CONST_ARG = 0,      /*!< constant.                                       */
	REG_ARG   = 1 << 6, /*!< register.                                       */
	ADDR_ARG  = 1 << 7  /*!< address.                                         */
}
memory_arg_t;


#define DEF_CMD(CMD_, NUM_, ...) cmd_##CMD_ = NUM_,
/*!
 * Enum with commands.
 */
typedef enum commands_t_
{
	#include "DEF_CMD"   /*   e.g. CMD_hit = 0                               */
	CMD_last = UCHAR_MAX /*!< auxiliary value                                */
}
commands_t;
#undef DEF_CMD




/*=========================== Constants declaration =========================*/


extern const signature_t SIGNATURE;
extern const version_t   VERSION;
extern const char*       EXEC_EXT;
extern const char*       ASM_EXT;
extern const size_t      EXEC_EXT_SIZE;
extern const size_t      ASM_EXT_SIZE;

/*!
 * The number of registers.
 *
 * Registers have names from ax to Xx, 
 * where X has number 'a' + REGS_NUMBER - 1
 */
#define REGS_NUMBER (reg_t) 16

/*!
 * Max size of token.
 */
#define MAX_TOKEN_SIZE (size_t) 64

/*!
 * Width of screen.
 */
#define VIDEO_WIDTH  (addr_t) 256

/*!
 * Height of screen.
 */
#define VIDEO_HEIGHT (addr_t) 256
/*!
 * Size of video memory.
 */
#define VIDEO_MEM_SIZE (addr_t) (VIDEO_WIDTH * VIDEO_HEIGHT)

/*!
 * Size of processor memory.
 */
#define MEMORY_SIZE (addr_t) (VIDEO_MEM_SIZE * 2)





#endif // ifndef COMMANDS_H_
