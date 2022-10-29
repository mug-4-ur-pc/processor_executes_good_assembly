/*!
 * @file This file contains all constants from project.
 */

#include "commands.h"

#include <stddef.h>

/*!
 * Signature of assembly file.
 *
 * In ASCII it equals to ITSPEGAS
 */
const signature_t SIGNATURE = 0x5341474550535449ULL;

/*!
 * Assembly version.
 */
const version_t   VERSION =
#include "version"
;

/*!
 * Executable file extension.
 */
const char* EXEC_EXT = "pegas";

/*!
 * Executable file extension's string length.
 */
const size_t EXEC_EXT_SIZE = 5;

/*!
 * Assembly file extension.
 */
const char* ASM_EXT = "asm";

/*!
 * Assembly file extension's string length.
 */
const size_t ASM_EXT_SIZE = 3;

/*!
 * Width of screen where video buffer will be printed.
 */
const addr_t VIDEO_WIDTH = 16;

