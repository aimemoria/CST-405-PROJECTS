/*
 * SECURITY.C - Security Analysis Implementation
 * CST-405 Compiler Project
 */

#include "security.h"
#include "diagnostics.h"
#include <limits.h>

/* Helper: Check if a node is a constant */
static int is_constant_node(ASTNode* node, int* value) {
    if (!node) return 0;

    if (node->type == NODE_NUMBER) {
        if (value) *value = node->data.num_value;
        return 1;
    }
    return 0;
}

/* Check for buffer overflow vulnerabilities */
void check_buffer_overflow(ASTNode* node, SymbolTable* symtab, SecurityCheckResults* results) {
    if (!node) return;

    /* Check array access with potentially out-of-bounds index */
    if (node->type == NODE_ARRAY_ACCESS) {
        const char* array_name = node->data.array_access.array_name;
        ASTNode* index = node->data.array_access.index;

        /* Look up array in symbol table */
        Symbol* sym = lookup_symbol(symtab, array_name);
        if (sym && sym->is_array) {
            int index_val;
            if (is_constant_node(index, &index_val)) {
                /* Static array bounds check */
                if (index_val < 0 || index_val >= sym->array_size) {
                    diag_security_warning(node->line_number, 0,
                        "Array '%s' access with index %d is out of bounds [0..%d]",
                        array_name, index_val, sym->array_size - 1);
                    results->buffer_overflow_risks++;
                }
            } else {
                /* Dynamic index - warn about potential overflow */
                debug_print("Array '%s' accessed with non-constant index - potential buffer overflow",
                           array_name);
                results->array_access_risks++;
            }
        }
    }

    /* Recursively check children */
    switch (node->type) {
        case NODE_PROGRAM:
            check_buffer_overflow(node->data.program.statements, symtab, results);
            break;
        case NODE_STATEMENT_LIST:
            check_buffer_overflow(node->data.stmt_list.statement, symtab, results);
            check_buffer_overflow(node->data.stmt_list.next, symtab, results);
            break;
        case NODE_BINARY_OP:
        case NODE_CONDITION:
            check_buffer_overflow(node->data.binary_op.left, symtab, results);
            check_buffer_overflow(node->data.binary_op.right, symtab, results);
            break;
        case NODE_ASSIGNMENT:
            check_buffer_overflow(node->data.assignment.expr, symtab, results);
            break;
        case NODE_PRINT:
            check_buffer_overflow(node->data.print.expr, symtab, results);
            break;
        case NODE_WHILE:
            check_buffer_overflow(node->data.while_loop.condition, symtab, results);
            check_buffer_overflow(node->data.while_loop.body, symtab, results);
            break;
        case NODE_IF:
            check_buffer_overflow(node->data.if_stmt.condition, symtab, results);
            check_buffer_overflow(node->data.if_stmt.then_branch, symtab, results);
            check_buffer_overflow(node->data.if_stmt.else_branch, symtab, results);
            break;
        case NODE_FOR:
            check_buffer_overflow(node->data.for_loop.init, symtab, results);
            check_buffer_overflow(node->data.for_loop.condition, symtab, results);
            check_buffer_overflow(node->data.for_loop.update, symtab, results);
            check_buffer_overflow(node->data.for_loop.body, symtab, results);
            break;
        case NODE_FUNCTION_CALL:
            check_buffer_overflow(node->data.func_call.args, symtab, results);
            break;
        case NODE_ARG_LIST:
            check_buffer_overflow(node->data.list.item, symtab, results);
            check_buffer_overflow(node->data.list.next, symtab, results);
            break;
        default:
            break;
    }
}

/* Check for integer overflow/underflow */
void check_integer_overflow(ASTNode* node, SecurityCheckResults* results) {
    if (!node) return;

    /* Check arithmetic operations with constants */
    if (node->type == NODE_BINARY_OP) {
        int left_val, right_val;
        if (is_constant_node(node->data.binary_op.left, &left_val) &&
            is_constant_node(node->data.binary_op.right, &right_val)) {

            const char* op = node->data.binary_op.operator;
            long long result;

            if (strcmp(op, "+") == 0) {
                result = (long long)left_val + right_val;
                if (result > INT_MAX || result < INT_MIN) {
                    diag_security_warning(node->line_number, 0,
                        "Integer overflow in addition: %d + %d",
                        left_val, right_val);
                    results->integer_overflow_risks++;
                }
            } else if (strcmp(op, "*") == 0) {
                result = (long long)left_val * right_val;
                if (result > INT_MAX || result < INT_MIN) {
                    diag_security_warning(node->line_number, 0,
                        "Integer overflow in multiplication: %d * %d",
                        left_val, right_val);
                    results->integer_overflow_risks++;
                }
            }
        }
    }

    /* Recursively check children */
    switch (node->type) {
        case NODE_PROGRAM:
            check_integer_overflow(node->data.program.statements, results);
            break;
        case NODE_STATEMENT_LIST:
            check_integer_overflow(node->data.stmt_list.statement, results);
            check_integer_overflow(node->data.stmt_list.next, results);
            break;
        case NODE_BINARY_OP:
            check_integer_overflow(node->data.binary_op.left, results);
            check_integer_overflow(node->data.binary_op.right, results);
            break;
        default:
            break;
    }
}

/* Check for division by zero */
void check_division_by_zero(ASTNode* node, SecurityCheckResults* results) {
    if (!node) return;

    /* Check division and modulo operations */
    if (node->type == NODE_BINARY_OP) {
        const char* op = node->data.binary_op.operator;

        if (strcmp(op, "/") == 0 || strcmp(op, "%") == 0) {
            int divisor;
            if (is_constant_node(node->data.binary_op.right, &divisor)) {
                if (divisor == 0) {
                    diag_error(node->line_number, 0,
                        "Division by zero detected");
                    results->division_by_zero_risks++;
                }
            } else {
                /* Non-constant divisor - potential risk */
                debug_print("Division by non-constant value - potential division by zero");
            }
        }
    }

    /* Recursively check children */
    switch (node->type) {
        case NODE_PROGRAM:
            check_division_by_zero(node->data.program.statements, results);
            break;
        case NODE_STATEMENT_LIST:
            check_division_by_zero(node->data.stmt_list.statement, results);
            check_division_by_zero(node->data.stmt_list.next, results);
            break;
        case NODE_BINARY_OP:
            check_division_by_zero(node->data.binary_op.left, results);
            check_division_by_zero(node->data.binary_op.right, results);
            break;
        default:
            break;
    }
}

/* Check for unsafe array accesses */
void check_unsafe_array_access(ASTNode* node, SymbolTable* symtab, SecurityCheckResults* results) {
    /* This is handled by check_buffer_overflow */
    check_buffer_overflow(node, symtab, results);
}

/* Check for potential infinite loops */
void check_infinite_loops(ASTNode* node, SecurityCheckResults* results) {
    if (!node) return;

    /* Check while loops with constant true condition */
    if (node->type == NODE_WHILE) {
        ASTNode* condition = node->data.while_loop.condition;

        /* Check for while(1) or similar */
        if (condition && condition->type == NODE_NUMBER) {
            if (condition->data.num_value != 0) {
                diag_warning(node->line_number, 0,
                    "Infinite loop detected: while loop with constant true condition");
                results->infinite_loop_risks++;
            }
        }
    }

    /* Recursively check children */
    switch (node->type) {
        case NODE_PROGRAM:
            check_infinite_loops(node->data.program.statements, results);
            break;
        case NODE_STATEMENT_LIST:
            check_infinite_loops(node->data.stmt_list.statement, results);
            check_infinite_loops(node->data.stmt_list.next, results);
            break;
        case NODE_WHILE:
            check_infinite_loops(node->data.while_loop.body, results);
            break;
        case NODE_IF:
            check_infinite_loops(node->data.if_stmt.then_branch, results);
            check_infinite_loops(node->data.if_stmt.else_branch, results);
            break;
        default:
            break;
    }
}

/* Check for uninitialized variable use */
void check_uninitialized_use(ASTNode* node, SymbolTable* symtab, SecurityCheckResults* results) {
    if (!node) return;

    /* This is a simplified check - a full implementation would require data flow analysis */
    /* For now, we just recursively traverse the tree */

    switch (node->type) {
        case NODE_PROGRAM:
            check_uninitialized_use(node->data.program.statements, symtab, results);
            break;
        case NODE_STATEMENT_LIST:
            check_uninitialized_use(node->data.stmt_list.statement, symtab, results);
            check_uninitialized_use(node->data.stmt_list.next, symtab, results);
            break;
        case NODE_BINARY_OP:
        case NODE_CONDITION:
            check_uninitialized_use(node->data.binary_op.left, symtab, results);
            check_uninitialized_use(node->data.binary_op.right, symtab, results);
            break;
        case NODE_ASSIGNMENT:
            check_uninitialized_use(node->data.assignment.expr, symtab, results);
            break;
        case NODE_PRINT:
            check_uninitialized_use(node->data.print.expr, symtab, results);
            break;
        case NODE_WHILE:
            check_uninitialized_use(node->data.while_loop.condition, symtab, results);
            check_uninitialized_use(node->data.while_loop.body, symtab, results);
            break;
        case NODE_IF:
            check_uninitialized_use(node->data.if_stmt.condition, symtab, results);
            check_uninitialized_use(node->data.if_stmt.then_branch, symtab, results);
            check_uninitialized_use(node->data.if_stmt.else_branch, symtab, results);
            break;
        case NODE_FOR:
            check_uninitialized_use(node->data.for_loop.init, symtab, results);
            check_uninitialized_use(node->data.for_loop.condition, symtab, results);
            check_uninitialized_use(node->data.for_loop.update, symtab, results);
            check_uninitialized_use(node->data.for_loop.body, symtab, results);
            break;
        default:
            break;
    }
}

/* Main security analysis function */
SecurityCheckResults* analyze_security(ASTNode* root, SymbolTable* symtab) {
    SecurityCheckResults* results = (SecurityCheckResults*)safe_calloc(1,
        sizeof(SecurityCheckResults), "security results");

    debug_print("Starting security analysis...");

    /* Perform all security checks */
    check_buffer_overflow(root, symtab, results);
    check_integer_overflow(root, results);
    check_division_by_zero(root, results);
    check_infinite_loops(root, results);

    /* Calculate total */
    results->total_security_issues =
        results->buffer_overflow_risks +
        results->integer_overflow_risks +
        results->division_by_zero_risks +
        results->array_access_risks +
        results->infinite_loop_risks;

    debug_print("Security analysis complete: %d issues found",
               results->total_security_issues);

    return results;
}

/* Print security analysis report */
void print_security_report(SecurityCheckResults* results) {
    printf("\n======================================================\n");
    printf("||          SECURITY ANALYSIS REPORT               ||\n");
    printf("======================================================\n");
    printf("|| Buffer Overflow Risks:      %-4d               ||\n", results->buffer_overflow_risks);
    printf("|| Integer Overflow Risks:     %-4d               ||\n", results->integer_overflow_risks);
    printf("|| Division by Zero Risks:     %-4d               ||\n", results->division_by_zero_risks);
    printf("|| Unsafe Array Accesses:      %-4d               ||\n", results->array_access_risks);
    printf("|| Infinite Loop Risks:        %-4d               ||\n", results->infinite_loop_risks);
    printf("======================================================\n");
    printf("|| Total Security Issues:      %-4d               ||\n", results->total_security_issues);
    printf("======================================================\n");

    if (results->total_security_issues == 0) {
        printf("\n[OK] No security issues detected!\n");
    } else {
        printf("\n[WARNING] Security analysis found %d potential issue(s)\n",
               results->total_security_issues);
        printf("  Review warnings above for details.\n");
    }
}

/* Free security results */
void free_security_results(SecurityCheckResults* results) {
    if (results) {
        free(results);
    }
}
