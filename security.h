/*
 * SECURITY.H - Security Analysis and Unsafe Construct Detection
 * CST-405 Compiler Project
 *
 * This module detects potentially unsafe constructs and security issues
 * in the source code, helping prevent common programming vulnerabilities.
 */

#ifndef SECURITY_H
#define SECURITY_H

#include "ast.h"
#include "symtable.h"

/* Security check results */
typedef struct {
    int buffer_overflow_risks;      /* Potential buffer overflows */
    int integer_overflow_risks;     /* Potential integer overflows */
    int uninitialized_use_risks;    /* Use before initialization */
    int division_by_zero_risks;     /* Potential division by zero */
    int array_access_risks;         /* Unsafe array accesses */
    int infinite_loop_risks;        /* Potential infinite loops */
    int total_security_issues;      /* Total issues found */
} SecurityCheckResults;

/* SECURITY CHECK FUNCTIONS */

/* Perform comprehensive security analysis on AST */
SecurityCheckResults* analyze_security(ASTNode* root, SymbolTable* symtab);

/* Check for buffer overflow vulnerabilities */
void check_buffer_overflow(ASTNode* node, SymbolTable* symtab, SecurityCheckResults* results);

/* Check for integer overflow/underflow */
void check_integer_overflow(ASTNode* node, SecurityCheckResults* results);

/* Check for division by zero */
void check_division_by_zero(ASTNode* node, SecurityCheckResults* results);

/* Check for unsafe array accesses */
void check_unsafe_array_access(ASTNode* node, SymbolTable* symtab, SecurityCheckResults* results);

/* Check for potential infinite loops */
void check_infinite_loops(ASTNode* node, SecurityCheckResults* results);

/* Check for uninitialized variable use */
void check_uninitialized_use(ASTNode* node, SymbolTable* symtab, SecurityCheckResults* results);

/* Print security analysis report */
void print_security_report(SecurityCheckResults* results);

/* Free security results */
void free_security_results(SecurityCheckResults* results);

#endif /* SECURITY_H */
