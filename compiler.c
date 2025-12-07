/*
 * COMPILER.C - Main Compiler Driver
 * CST-405 Complete Compiler Project
 *
 * This is the main entry point for the complete compiler.
 * It orchestrates all compilation phases:
 *   1. Lexical Analysis (Scanning)
 *   2. Syntax Analysis (Parsing)
 *   3. Semantic Analysis
 *   4. Intermediate Code Generation (TAC)
 *   5. Code Optimization
 *   6. Code Generation (Assembly)
 *
 * FEATURES: Loops (while/for/do-while), if/else, functions, arrays
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtable.h"
#include "semantic.h"
#include "ircode.h"
#include "optimizer.h"
#include "codegen.h"
#include "codegen_mips.h"
#include "diagnostics.h"
#include "security.h"

/* External declarations from parser */
extern int yyparse();
extern FILE* yyin;
extern ASTNode* ast_root;
extern SymbolTable* global_symtab;
extern int syntax_errors;
extern int line_num;

/* Function prototypes */
void print_banner();
void print_phase_separator(const char* phase_name);
void print_summary(int success);

int main(int argc, char* argv[]) {
    /* Print compiler banner */
    print_banner();

    /* Check command line arguments */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file> [options]\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  --mips          Generate MIPS assembly instead of x86-64\n");
        fprintf(stderr, "  --verbose       Enable verbose output and debugging info\n");
        fprintf(stderr, "  --log <file>    Write diagnostics to log file\n");
        fprintf(stderr, "  --no-warnings   Suppress warning messages\n");
        fprintf(stderr, "  --Werror        Treat warnings as errors\n");
        fprintf(stderr, "\nExample: %s program.src --verbose --mips\n", argv[0]);
        return 1;
    }

    const char* input_filename = argv[1];
    int use_mips = 0;
    int verbose = 0;
    int warnings_as_errors = 0;
    int show_warnings = 1;
    const char* log_file = NULL;
    const char* output_filename = "output.asm";

    /* Parse command line flags */
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--mips") == 0) {
            use_mips = 1;
            output_filename = "output_mips.asm";
        } else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "--Werror") == 0) {
            warnings_as_errors = 1;
        } else if (strcmp(argv[i], "--no-warnings") == 0) {
            show_warnings = 0;
        } else if (strcmp(argv[i], "--log") == 0 && i + 1 < argc) {
            log_file = argv[++i];
        }
    }

    /* Initialize diagnostics system */
    init_diagnostics(verbose, warnings_as_errors);
    diag_config.show_warnings = show_warnings;

    if (log_file) {
        set_diagnostic_log_file(log_file);
    }

    /* Open input file */
    FILE* input_file = fopen(input_filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_filename);
        return 1;
    }

    yyin = input_file;
    printf("Input file: %s\n", input_filename);
    printf("Output file: %s\n", output_filename);
    printf("Target: %s\n\n", use_mips ? "MIPS (QtSpim/MARS)" : "x86-64 (NASM)");

    /* ===================================================================
     * PHASE 1 & 2: LEXICAL AND SYNTAX ANALYSIS
     * The lexer (scanner) and parser work together during yyparse()
     * ================================================================ */
    print_phase_separator("PHASE 1 & 2: LEXICAL AND SYNTAX ANALYSIS");

    /* Initialize symbol table before parsing */
    global_symtab = create_symbol_table(100);

    /* Run the parser (which calls the lexer) */
    int parse_result = yyparse();

    /* Check for syntax errors */
    if (parse_result != 0 || syntax_errors > 0) {
        fprintf(stderr, "\n[X] COMPILATION FAILED: Syntax errors detected\n");
        fprintf(stderr, "[X] Please fix the errors and try again\n\n");

        fclose(input_file);
        if (global_symtab) free_symbol_table(global_symtab);
        if (ast_root) free_ast(ast_root);
        return 1;
    }

    printf("[OK] Lexical analysis complete\n");
    printf("[OK] Syntax analysis complete\n");
    printf("[OK] Abstract Syntax Tree (AST) constructed\n\n");

    /* ===================================================================
     * PHASE 3: SEMANTIC ANALYSIS
     * Type checking, variable declaration/initialization checking
     * ================================================================ */
    print_phase_separator("PHASE 3: SEMANTIC ANALYSIS");

    int semantic_result = analyze_semantics(ast_root, global_symtab);

    if (semantic_result > 0) {
        fprintf(stderr, "\n[X] COMPILATION FAILED: Semantic errors detected\n");
        fprintf(stderr, "[X] Please fix the errors and try again\n\n");

        fclose(input_file);
        free_symbol_table(global_symtab);
        free_ast(ast_root);
        return 1;
    }

    print_semantic_summary();

    /* Print AST for debugging */
    printf("=============== ABSTRACT SYNTAX TREE ==================\n\n");
    print_ast(ast_root, 0);
    printf("\n");

    /* Print symbol table */
    printf("=================== SYMBOL TABLE ======================\n\n");
    print_symbol_table(global_symtab);
    printf("\n");

    /* ===================================================================
     * PHASE 4: INTERMEDIATE CODE GENERATION
     * Generate Three-Address Code (TAC) from AST
     * ================================================================ */
    print_phase_separator("PHASE 4: INTERMEDIATE CODE GENERATION");

    TACCode* tac = generate_tac(ast_root);

    if (!tac) {
        fprintf(stderr, "\n[X] COMPILATION FAILED: IR generation failed\n\n");

        fclose(input_file);
        free_symbol_table(global_symtab);
        free_ast(ast_root);
        return 1;
    }

    /* Print TAC before optimization */
    print_tac(tac);

    /* Save IR to file */
    FILE* ir_file = fopen("output.ir", "w");
    if (ir_file) {
        TACInstruction* inst = tac->head;
        while (inst) {
            fprintf(ir_file, "%s", opcode_to_string(inst->opcode));
            if (inst->result) fprintf(ir_file, " %s", inst->result);
            if (inst->op1) fprintf(ir_file, " %s", inst->op1);
            if (inst->op2) fprintf(ir_file, " %s", inst->op2);
            if (inst->label) fprintf(ir_file, " %s", inst->label);
            fprintf(ir_file, "\n");
            inst = inst->next;
        }
        fclose(ir_file);
        printf("[OK] Intermediate code saved to: output.ir\n\n");
    }

    /* ===================================================================
     * PHASE 5: CODE OPTIMIZATION
     * Optimize the intermediate representation
     * ================================================================ */
    print_phase_separator("PHASE 5: CODE OPTIMIZATION");

    OptimizationStats opt_stats;
    optimize_tac(tac, &opt_stats);
    print_optimization_stats(&opt_stats);

    /* Print optimized TAC */
    if (verbose) {
        printf("=============== OPTIMIZED TAC ==================\n\n");
        print_tac(tac);
    }

    /* ===================================================================
     * PHASE 5.5: SECURITY ANALYSIS
     * Check for unsafe constructs and security vulnerabilities
     * ================================================================ */
    print_phase_separator("PHASE 5.5: SECURITY ANALYSIS");

    SecurityCheckResults* security_results = analyze_security(ast_root, global_symtab);
    print_security_report(security_results);

    /* ===================================================================
     * PHASE 6: CODE GENERATION
     * Generate assembly code from optimized TAC
     * ================================================================ */
    print_phase_separator("PHASE 6: ASSEMBLY CODE GENERATION");

    if (use_mips) {
        /* Generate MIPS assembly */
        MIPSCodeGenerator* mips_gen = create_mips_code_generator(output_filename, global_symtab);
        generate_mips_assembly(mips_gen, tac);
        close_mips_code_generator(mips_gen);
    } else {
        /* Generate x86-64 assembly */
        CodeGenerator* codegen = create_code_generator(output_filename, global_symtab);
        generate_assembly(codegen, tac);
        close_code_generator(codegen);
    }

    /* ===================================================================
     * COMPILATION COMPLETE
     * ================================================================ */
    print_summary(1);

    printf("[OK] Compilation successful!\n");
    printf("[OK] Assembly code written to: %s\n\n", output_filename);

    if (use_mips) {
        printf("To run on QtSpim or MARS:\n");
        printf("  1. Open %s in QtSpim or MARS simulator\n", output_filename);
        printf("  2. Assemble and run the program\n\n");
    } else {
        printf("To assemble and link (on Linux):\n");
        printf("  nasm -f elf64 %s -o output.o\n", output_filename);
        printf("  gcc output.o -o program -no-pie\n");
        printf("  ./program\n\n");
    }

    /* ===================================================================
     * FINAL DIAGNOSTICS
     * ================================================================ */
    print_diagnostic_summary();

    /* Cleanup */
    fclose(input_file);
    free_ast(ast_root);
    free_symbol_table(global_symtab);
    free_tac(tac);
    free_security_results(security_results);
    close_diagnostics();

    return 0;
}

/* Print the compiler banner */
void print_banner() {
    printf("\n");
    printf("+============================================================+\n");
    printf("|                                                           |\n");
    printf("|           CST-405 COMPLETE COMPILER SYSTEM                |\n");
    printf("|                                                           |\n");
    printf("|  A full-featured compiler with lexer, parser, semantic   |\n");
    printf("|  analyzer, intermediate code generator, and code gen.    |\n");
    printf("|                                                           |\n");
    printf("|  NEW FEATURE: While loops with relational operators      |\n");
    printf("|                                                           |\n");
    printf("+============================================================+\n\n");
}

/* Print phase separator */
void print_phase_separator(const char* phase_name) {
    printf("+============================================================+\n");
    printf("| %-57s |\n", phase_name);
    printf("+============================================================+\n\n");
}

/* Print compilation summary */
void print_summary(int success) {
    printf("+============================================================+\n");
    printf("|                   COMPILATION SUMMARY                     |\n");
    printf("+============================================================+\n");

    if (success) {
        printf("|  Status:           [OK] SUCCESS                           |\n");
        printf("|  Lexical errors:   0                                      |\n");
        printf("|  Syntax errors:    0                                      |\n");
        printf("|  Semantic errors:  0                                      |\n");
        printf("|  Optimization:     Enabled                                |\n");
        printf("|  Code generated:   Yes                                    |\n");
    } else {
        printf("|  Status:           [X] FAILED                             |\n");
    }

    printf("+============================================================+\n\n");
}
