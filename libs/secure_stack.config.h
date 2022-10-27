/*! 
 * @file
 * @brief This is config file for secure_stack.c
 */

#define ON  1
#define OFF 0

/*!
 * Checking the stack for integrity in each operation on it.
 */
#define VALIDATION OFF

 /*!
  * Protective barriers at the edges of the structure and data of the stack.
  */
#define CANARIES   OFF

/*! 
* A unique value for each state of the stack, which is recalculated,
* when the stack changes.
*/
#define HASH       OFF

#define POISON     145
#define POISON_PTR (void *)300
#define CANARY     0x47C0DAB1EC0DEBEFULL
