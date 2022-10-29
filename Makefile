CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -lm -g

all: pegas_asm pegas_disasm pegas_exec

pegas_asm: version constants.c assembler/assembler.h errors/*
	$(CC) $(CFLAGS) constants.c libs/* errors/errors.c assembler/main.c assembler/assembler.c -o pegas_asm

pegas_disasm: version constants.c disassembler/disassembler.h errors/*
	$(CC) $(CFLAGS) constants.c libs/* errors/errors.c disassembler/main.c disassembler/disassembler.c -o pegas_disasm

pegas_exec: version constants.c processor/processor.h errors/*
	$(CC) $(CFLAGS) constants.c libs/* errors/errors.c processor/main.c processor/processor.c -o pegas_exec

.PHONY: clean
clean:
	rm pegas_asm pegas_disasm pegas_exec || true

.PHONY: version
version: DEF_CMD commands.h
	#expr `cat version` + 1 >version

