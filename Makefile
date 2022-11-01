CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -lm -g

all: asm disasm proc

.PHONY: asm
asm: pegas_asm
pegas_asm: cmd_version constants.c assembler/* errors/* libs/*
	$(CC) $(CFLAGS) constants.c libs/* errors/errors.c assembler/* -o pegas_asm

.PHONY: disasm
disasm: pegas_disasm
pegas_disasm: cmd_version constants.c disassembler/* errors/* libs/*
	$(CC) $(CFLAGS) constants.c libs/* errors/errors.c disassembler/* -o pegas_disasm

.PHONY: proc
proc: pegas_exec
pegas_exec: cmd_version constants.c processor/* errors/* libs/*
	$(CC) $(CFLAGS) constants.c libs/* errors/errors.c processor/* -o pegas_exec

.PHONY: clean
clean:
	rm pegas_asm pegas_disasm pegas_exec || true

.PHONY: cmd_version
cmd_version: DEF_CMD commands.h
	expr `cat version` + 1 >version

