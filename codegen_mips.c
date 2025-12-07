/*
 * CODEGEN_MIPS.C - MIPS Code Generator Implementation
 * CST-405 Compiler Project
 *
 * This file implements code generation from Three-Address Code (TAC)
 * to MIPS assembly language for QtSpim or MARS simulator.
 */

#include "codegen_mips.h"

/* Register mapping for temporaries */
static const char* temp_registers[] = {
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$t8", "$t9"
};

/* Create a new MIPS code generator instance */
MIPSCodeGenerator* create_mips_code_generator(const char* output_filename, SymbolTable* symtab) {
    MIPSCodeGenerator* gen = (MIPSCodeGenerator*)malloc(sizeof(MIPSCodeGenerator));
    if (!gen) {
        fprintf(stderr, "Fatal Error: Failed to allocate MIPS code generator\n");
        exit(1);
    }

    gen->output_file = fopen(output_filename, "w");
    if (!gen->output_file) {
        fprintf(stderr, "Fatal Error: Cannot open output file '%s'\n", output_filename);
        free(gen);
        exit(1);
    }

    gen->stack_offset = 0;
    gen->symtab = symtab;
    gen->next_register = 0;

    return gen;
}

/* Generate the MIPS prologue (program initialization) */
void gen_mips_prologue(MIPSCodeGenerator* gen) {
    fprintf(gen->output_file, "# CST-405 Compiler - Generated MIPS Assembly Code\n");
    fprintf(gen->output_file, "# Target: MIPS (QtSpim/MARS)\n");
    fprintf(gen->output_file, "# Date: %s\n\n", __DATE__);

    fprintf(gen->output_file, ".data\n");
    fprintf(gen->output_file, "    # Data section for variables\n");
    fprintf(gen->output_file, "    newline: .asciiz \"\\n\"\n");

    /* Allocate space for all variables in the symbol table */
    if (gen->symtab) {
        for (int i = 0; i < gen->symtab->size; i++) {
            Symbol* sym = gen->symtab->table[i];
            while (sym) {
                /* Only allocate space for global variables, not functions */
                if (sym->kind == SYMBOL_VARIABLE && strcmp(sym->scope, "global") == 0) {
                    if (sym->is_array) {
                        /* Arrays need space for multiple words */
                        fprintf(gen->output_file, "    %s: .space %d    # Array: %s[%d]\n",
                                sym->name, sym->array_size * 4, sym->name, sym->array_size);
                    } else {
                        /* Regular variables need 1 word (4 bytes) */
                        fprintf(gen->output_file, "    %s: .word 0    # Variable: %s\n",
                                sym->name, sym->name);
                    }
                }
                sym = sym->next;
            }
        }
    }

    /* Allocate space for temporaries */
    fprintf(gen->output_file, "\n    # Temporary variables\n");
    for (int i = 0; i < 100; i++) {
        fprintf(gen->output_file, "    t%d: .word 0\n", i);
    }

    fprintf(gen->output_file, "\n.text\n");
    fprintf(gen->output_file, ".globl main\n\n");

    fprintf(gen->output_file, "main:\n");
    fprintf(gen->output_file, "    # Function prologue\n");
    fprintf(gen->output_file, "    # (MIPS doesn't require explicit frame setup for main)\n\n");
}

/* Generate the MIPS epilogue (program termination) */
void gen_mips_epilogue(MIPSCodeGenerator* gen) {
    fprintf(gen->output_file, "\n    # Program exit\n");
    fprintf(gen->output_file, "    li $v0, 10        # syscall: exit\n");
    fprintf(gen->output_file, "    syscall\n");
}

/* Get register for a temporary or variable */
const char* get_mips_register(MIPSCodeGenerator* gen, const char* name) {
    /* For simplicity, use $t0-$t9 in rotation */
    static char reg[8];

    /* Check if it's a temporary variable (t0, t1, etc.) */
    if (name && name[0] == 't' && isdigit(name[1])) {
        int temp_num = atoi(&name[1]);
        snprintf(reg, sizeof(reg), "$t%d", temp_num % 10);
        return reg;
    }

    /* For other variables, use $t0 as working register */
    return "$t0";
}

/* Generate code for a single MIPS TAC instruction */
void gen_mips_instruction(MIPSCodeGenerator* gen, TACInstruction* inst) {
    switch (inst->opcode) {
        case TAC_LOAD_CONST:
            /* Load constant into variable: result = constant */
            fprintf(gen->output_file, "    # %s = %s\n", inst->result, inst->op1);
            fprintf(gen->output_file, "    li $t0, %s\n", inst->op1);
            fprintf(gen->output_file, "    sw $t0, %s\n", inst->result);
            break;

        case TAC_ASSIGN:
            /* Assignment: result = op1 */
            fprintf(gen->output_file, "    # %s = %s\n", inst->result, inst->op1);
            fprintf(gen->output_file, "    lw $t0, %s\n", inst->op1);
            fprintf(gen->output_file, "    sw $t0, %s\n", inst->result);
            break;

        case TAC_ADD:
            /* Addition: result = op1 + op2 */
            fprintf(gen->output_file, "    # %s = %s + %s\n", inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    lw $t0, %s\n", inst->op1);
            fprintf(gen->output_file, "    lw $t1, %s\n", inst->op2);
            fprintf(gen->output_file, "    add $t0, $t0, $t1\n");
            fprintf(gen->output_file, "    sw $t0, %s\n", inst->result);
            break;

        case TAC_SUB:
            /* Subtraction: result = op1 - op2 */
            fprintf(gen->output_file, "    # %s = %s - %s\n", inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    lw $t0, %s\n", inst->op1);
            fprintf(gen->output_file, "    lw $t1, %s\n", inst->op2);
            fprintf(gen->output_file, "    sub $t0, $t0, $t1\n");
            fprintf(gen->output_file, "    sw $t0, %s\n", inst->result);
            break;

        case TAC_MUL:
            /* Multiplication: result = op1 * op2 */
            fprintf(gen->output_file, "    # %s = %s * %s\n", inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    lw $t0, %s\n", inst->op1);
            fprintf(gen->output_file, "    lw $t1, %s\n", inst->op2);
            fprintf(gen->output_file, "    mul $t0, $t0, $t1\n");
            fprintf(gen->output_file, "    sw $t0, %s\n", inst->result);
            break;

        case TAC_DIV:
            /* Division: result = op1 / op2 */
            fprintf(gen->output_file, "    # %s = %s / %s\n", inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    lw $t0, %s\n", inst->op1);
            fprintf(gen->output_file, "    lw $t1, %s\n", inst->op2);
            fprintf(gen->output_file, "    div $t0, $t1\n");
            fprintf(gen->output_file, "    mflo $t0\n");
            fprintf(gen->output_file, "    sw $t0, %s\n", inst->result);
            break;

        case TAC_MOD:
            /* Modulo: result = op1 % op2 */
            fprintf(gen->output_file, "    # %s = %s %% %s\n", inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    lw $t0, %s\n", inst->op1);
            fprintf(gen->output_file, "    lw $t1, %s\n", inst->op2);
            fprintf(gen->output_file, "    div $t0, $t1\n");
            fprintf(gen->output_file, "    mfhi $t0\n");
            fprintf(gen->output_file, "    sw $t0, %s\n", inst->result);
            break;

        case TAC_PRINT:
            /* Print statement: print(op1) */
            fprintf(gen->output_file, "    # print(%s)\n", inst->op1);
            fprintf(gen->output_file, "    lw $a0, %s\n", inst->op1);
            fprintf(gen->output_file, "    li $v0, 1        # syscall: print_int\n");
            fprintf(gen->output_file, "    syscall\n");
            fprintf(gen->output_file, "    la $a0, newline\n");
            fprintf(gen->output_file, "    li $v0, 4        # syscall: print_string\n");
            fprintf(gen->output_file, "    syscall\n");
            break;

        case TAC_LABEL:
            /* Label definition */
            fprintf(gen->output_file, "%s:\n", inst->label);
            break;

        case TAC_GOTO:
            /* Unconditional jump */
            fprintf(gen->output_file, "    j %s\n", inst->label);
            break;

        case TAC_IF_FALSE:
            /* Conditional jump: if op1 == 0 goto label */
            fprintf(gen->output_file, "    # if_false %s goto %s\n", inst->op1, inst->label);
            fprintf(gen->output_file, "    lw $t0, %s\n", inst->op1);
            fprintf(gen->output_file, "    beqz $t0, %s\n", inst->label);
            break;

        case TAC_RELOP:
            /* Relational operation: result = op1 relop op2 */
            fprintf(gen->output_file, "    # %s = %s %s %s\n",
                    inst->result, inst->op1, inst->label, inst->op2);
            fprintf(gen->output_file, "    lw $t0, %s\n", inst->op1);
            fprintf(gen->output_file, "    lw $t1, %s\n", inst->op2);

            /* Determine which relational operator */
            if (strcmp(inst->label, "<") == 0) {
                fprintf(gen->output_file, "    slt $t0, $t0, $t1\n");
            } else if (strcmp(inst->label, ">") == 0) {
                fprintf(gen->output_file, "    sgt $t0, $t0, $t1\n");
            } else if (strcmp(inst->label, "<=") == 0) {
                fprintf(gen->output_file, "    sle $t0, $t0, $t1\n");
            } else if (strcmp(inst->label, ">=") == 0) {
                fprintf(gen->output_file, "    sge $t0, $t0, $t1\n");
            } else if (strcmp(inst->label, "==") == 0) {
                fprintf(gen->output_file, "    seq $t0, $t0, $t1\n");
            } else if (strcmp(inst->label, "!=") == 0) {
                fprintf(gen->output_file, "    sne $t0, $t0, $t1\n");
            }

            fprintf(gen->output_file, "    sw $t0, %s\n", inst->result);
            break;

        case TAC_ARRAY_LOAD:
            /* Array load: result = array[index] */
            fprintf(gen->output_file, "    # %s = %s[%s]\n", inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    lw $t0, %s       # load index\n", inst->op2);
            fprintf(gen->output_file, "    sll $t0, $t0, 2  # multiply by 4 (word size)\n");
            fprintf(gen->output_file, "    la $t1, %s       # load array base\n", inst->op1);
            fprintf(gen->output_file, "    add $t0, $t0, $t1\n");
            fprintf(gen->output_file, "    lw $t0, 0($t0)\n");
            fprintf(gen->output_file, "    sw $t0, %s\n", inst->result);
            break;

        case TAC_ARRAY_STORE:
            /* Array store: array[index] = value */
            fprintf(gen->output_file, "    # %s[%s] = %s\n", inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    lw $t0, %s       # load index\n", inst->op1);
            fprintf(gen->output_file, "    sll $t0, $t0, 2  # multiply by 4\n");
            fprintf(gen->output_file, "    la $t1, %s       # load array base\n", inst->result);
            fprintf(gen->output_file, "    add $t0, $t0, $t1\n");
            fprintf(gen->output_file, "    lw $t2, %s       # load value\n", inst->op2);
            fprintf(gen->output_file, "    sw $t2, 0($t0)\n");
            break;

        case TAC_FUNCTION_LABEL:
            /* Function label */
            fprintf(gen->output_file, "\n%s:\n", inst->label);
            fprintf(gen->output_file, "    # Function: %s\n", inst->label);
            break;

        case TAC_PARAM:
            /* Function parameter (push to stack) */
            fprintf(gen->output_file, "    # param %s\n", inst->op1);
            fprintf(gen->output_file, "    lw $t0, %s\n", inst->op1);
            fprintf(gen->output_file, "    addi $sp, $sp, -4\n");
            fprintf(gen->output_file, "    sw $t0, 0($sp)\n");
            break;

        case TAC_CALL:
            /* Function call */
            fprintf(gen->output_file, "    # call %s\n", inst->label);
            fprintf(gen->output_file, "    jal %s\n", inst->label);
            /* Pop parameters */
            int param_count = atoi(inst->op1);
            fprintf(gen->output_file, "    addi $sp, $sp, %d    # pop parameters\n",
                    param_count * 4);
            fprintf(gen->output_file, "    sw $v0, %s       # save return value\n", inst->result);
            break;

        case TAC_RETURN:
            /* Return with value */
            fprintf(gen->output_file, "    # return %s\n", inst->op1);
            fprintf(gen->output_file, "    lw $v0, %s\n", inst->op1);
            fprintf(gen->output_file, "    jr $ra\n");
            break;

        case TAC_RETURN_VOID:
            /* Return without value */
            fprintf(gen->output_file, "    # return (void)\n");
            fprintf(gen->output_file, "    jr $ra\n");
            break;

        default:
            fprintf(gen->output_file, "    # Unknown opcode: %s\n",
                    opcode_to_string(inst->opcode));
            break;
    }
}

/* Generate MIPS assembly from TAC */
void generate_mips_assembly(MIPSCodeGenerator* gen, TACCode* tac) {
    printf("[CODEGEN] Generating MIPS assembly code...\n");

    gen_mips_prologue(gen);

    /* Generate code for each TAC instruction */
    TACInstruction* inst = tac->head;
    while (inst) {
        gen_mips_instruction(gen, inst);
        inst = inst->next;
    }

    gen_mips_epilogue(gen);

    printf("[CODEGEN] MIPS assembly generation complete\n");
    printf("[CODEGEN] Total instructions: %d\n", tac->instruction_count);
}

/* Close and cleanup MIPS code generator */
void close_mips_code_generator(MIPSCodeGenerator* gen) {
    if (gen->output_file) {
        fclose(gen->output_file);
    }
    free(gen);
}
