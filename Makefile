CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -lm -g

all: asm disasm proc debugger

.PHONY: asm
asm: pegas_asm
pegas_asm: constants.c assembler/* errors/* libs/*
	$(CC) $(CFLAGS) constants.c libs/* errors/errors.c assembler/* -o pegas_asm

.PHONY: disasm
disasm: pegas_disasm
pegas_disasm: constants.c disassembler/* errors/* libs/*
	$(CC) $(CFLAGS) constants.c libs/* errors/errors.c disassembler/* -o pegas_disasm

.PHONY: proc
proc: pegas_exec
pegas_exec: constants.c processor/* errors/* libs/*
	$(CC) $(CFLAGS) -lSDL2 constants.c libs/* errors/errors.c processor/* -o pegas_exec

.PHONY: debugger
debugger: pegas_debugger
pegas_debugger: constants.c processor/* errors/* libs/*
	$(CC) $(CFLAGS) -lSDL2 -DDEBUGGER constants.c libs/* errors/errors.c processor/* -o pegas_debugger

.PHONY: clean
clean:
	rm pegas_asm pegas_disasm pegas_exec || true
