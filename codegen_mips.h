/*
 * CODEGEN_MIPS.H - MIPS Code Generator Header
 * CST-405 Compiler Project
 *
 * This file defines the code generation phase which translates
 * Three-Address Code (TAC) into MIPS assembly code for QtSpim/MARS.
 */

#ifndef CODEGEN_MIPS_H
#define CODEGEN_MIPS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ircode.h"
#include "symtable.h"

/* MIPS Assembly code output structure */
typedef struct {
    FILE* output_file;          /* File to write assembly code to */
    int stack_offset;           /* Current stack frame offset */
    SymbolTable* symtab;        /* Symbol table for variable locations */
    int next_register;          /* Next available temporary register */
} MIPSCodeGenerator;

/* CODE GENERATION FUNCTIONS */

/* Create a new MIPS code generator */
MIPSCodeGenerator* create_mips_code_generator(const char* output_filename, SymbolTable* symtab);

/* Generate MIPS assembly code from TAC */
void generate_mips_assembly(MIPSCodeGenerator* gen, TACCode* tac);

/* Generate MIPS prologue (program setup) */
void gen_mips_prologue(MIPSCodeGenerator* gen);

/* Generate MIPS epilogue (program cleanup) */
void gen_mips_epilogue(MIPSCodeGenerator* gen);

/* Generate code for a single TAC instruction */
void gen_mips_instruction(MIPSCodeGenerator* gen, TACInstruction* inst);

/* Get register for a variable/temporary */
const char* get_mips_register(MIPSCodeGenerator* gen, const char* name);

/* Close and cleanup MIPS code generator */
void close_mips_code_generator(MIPSCodeGenerator* gen);

#endif /* CODEGEN_MIPS_H */
