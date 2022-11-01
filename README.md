# Processor executes good assembly (PEGAS) :)

* [Description](#Description)   
* [Compilation](#Compilation)    
* [Usage](#Usage)    
* [Example](#Example)   


## Description

This is implementation of soft processor.



## Compilation

To compile it run `make asm`, `make proc` or `make disasm`.



## Usage

All commands described in file `docs.txt`. Write your assembly code
in file with extension `.asm`, compile it using `pegas_asm <filename>`.
It creates a new file with extension `.pegas`. You can
run it using `pegas_exec <filename>`.
To restore source code from compiled file run `pegas_disasm <filename>`.



## Example

You can see one example **[here](example/ "Example")**.




# Good luck!
