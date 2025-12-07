/*
 * OPTIMIZER.C - Code Optimizer Implementation
 * CST-405 Compiler Project
 *
 * This file implements various optimization techniques on the
 * Three-Address Code (TAC) intermediate representation.
 */

#include "optimizer.h"
#include <ctype.h>

/* Helper function: Check if a string represents a number */
int is_number(const char* str) {
    if (!str || *str == '\0') return 0;

    /* Handle negative numbers */
    if (*str == '-') str++;

    /* Check if all remaining characters are digits */
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

/* Helper function: Evaluate binary operation on two constants */
int evaluate_binary_op(const char* op, int left, int right) {
    if (strcmp(op, "+") == 0 || strcmp(op, "ADD") == 0) {
        return left + right;
    } else if (strcmp(op, "-") == 0 || strcmp(op, "SUB") == 0) {
        return left - right;
    } else if (strcmp(op, "*") == 0 || strcmp(op, "MUL") == 0) {
        return left * right;
    } else if (strcmp(op, "/") == 0 || strcmp(op, "DIV") == 0) {
        if (right == 0) return 0; /* Avoid division by zero */
        return left / right;
    } else if (strcmp(op, "%") == 0 || strcmp(op, "MOD") == 0) {
        if (right == 0) return 0;
        return left % right;
    }
    return 0;
}

/* Constant Folding: Evaluate constant expressions at compile time
 * Example: t0 = 3 + 5 becomes t0 = 8
 */
int constant_folding(TACCode* code) {
    int optimizations = 0;
    TACInstruction* inst = code->head;

    while (inst) {
        /* Check for binary operations with constant operands */
        if ((inst->opcode == TAC_ADD || inst->opcode == TAC_SUB ||
             inst->opcode == TAC_MUL || inst->opcode == TAC_DIV ||
             inst->opcode == TAC_MOD) &&
            inst->op1 && inst->op2 &&
            is_number(inst->op1) && is_number(inst->op2)) {

            /* Both operands are constants - fold them! */
            int left = atoi(inst->op1);
            int right = atoi(inst->op2);
            int result = evaluate_binary_op(opcode_to_string(inst->opcode), left, right);

            /* Convert to LOAD_CONST instruction */
            free(inst->op1);
            free(inst->op2);

            char result_str[32];
            snprintf(result_str, sizeof(result_str), "%d", result);
            inst->opcode = TAC_LOAD_CONST;
            inst->op1 = strdup(result_str);
            inst->op2 = NULL;

            optimizations++;
            printf("[OPTIMIZER] Constant folding: Folded constant expression to %d\n", result);
        }

        /* Algebraic simplifications */
        if (inst->opcode == TAC_MUL && inst->op2 && is_number(inst->op2)) {
            int multiplier = atoi(inst->op2);

            /* x * 0 = 0 */
            if (multiplier == 0) {
                inst->opcode = TAC_LOAD_CONST;
                free(inst->op1);
                free(inst->op2);
                inst->op1 = strdup("0");
                inst->op2 = NULL;
                optimizations++;
                printf("[OPTIMIZER] Algebraic simplification: x * 0 = 0\n");
            }
            /* x * 1 = x (convert to assignment) */
            else if (multiplier == 1) {
                free(inst->op2);
                inst->opcode = TAC_ASSIGN;
                inst->op2 = NULL;
                optimizations++;
                printf("[OPTIMIZER] Algebraic simplification: x * 1 = x\n");
            }
        }

        /* x + 0 = x or x - 0 = x */
        if ((inst->opcode == TAC_ADD || inst->opcode == TAC_SUB) &&
            inst->op2 && is_number(inst->op2) && atoi(inst->op2) == 0) {
            free(inst->op2);
            inst->opcode = TAC_ASSIGN;
            inst->op2 = NULL;
            optimizations++;
            printf("[OPTIMIZER] Algebraic simplification: x +/- 0 = x\n");
        }

        inst = inst->next;
    }

    return optimizations;
}

/* Dead Code Elimination: Remove unreachable or unused code
 * - Remove code after unconditional jumps
 * - Remove unused temporary variables (basic version)
 */
int eliminate_dead_code(TACCode* code) {
    int optimizations = 0;
    TACInstruction* inst = code->head;
    TACInstruction* prev = NULL;

    while (inst && inst->next) {
        /* Remove instructions after unconditional GOTO until next label */
        if (inst->opcode == TAC_GOTO) {
            TACInstruction* next = inst->next;

            /* Skip instructions until we hit a label */
            while (next && next->opcode != TAC_LABEL &&
                   next->opcode != TAC_FUNCTION_LABEL) {
                TACInstruction* to_remove = next;
                next = next->next;

                /* Remove the dead instruction */
                inst->next = next;

                printf("[OPTIMIZER] Dead code elimination: Removed unreachable instruction after GOTO\n");

                free(to_remove->result);
                free(to_remove->op1);
                free(to_remove->op2);
                free(to_remove->label);
                free(to_remove);

                optimizations++;
                code->instruction_count--;
            }
        }

        /* Remove consecutive identical assignments: x = y; x = y; */
        if (inst->opcode == TAC_ASSIGN && inst->next &&
            inst->next->opcode == TAC_ASSIGN &&
            inst->result && inst->next->result &&
            inst->op1 && inst->next->op1 &&
            strcmp(inst->result, inst->next->result) == 0 &&
            strcmp(inst->op1, inst->next->op1) == 0) {

            TACInstruction* duplicate = inst->next;
            inst->next = duplicate->next;

            free(duplicate->result);
            free(duplicate->op1);
            free(duplicate->op2);
            free(duplicate->label);
            free(duplicate);

            optimizations++;
            code->instruction_count--;
            printf("[OPTIMIZER] Dead code elimination: Removed duplicate assignment\n");
        }

        prev = inst;
        inst = inst->next;
    }

    return optimizations;
}

/* Copy Propagation: Replace variable copies with direct references
 * Example: t0 = x; t1 = t0 + 5; becomes t1 = x + 5;
 */
int copy_propagation(TACCode* code) {
    int optimizations = 0;
    TACInstruction* inst = code->head;

    while (inst) {
        /* Look for simple assignments: t0 = x */
        if (inst->opcode == TAC_ASSIGN && inst->result && inst->op1 &&
            !inst->op2 && !is_number(inst->op1)) {

            char* temp = inst->result;
            char* original = inst->op1;

            /* Look ahead for uses of this temp and replace with original */
            TACInstruction* next = inst->next;
            int replaced = 0;

            /* Only propagate within a small window to avoid issues */
            int window = 0;
            while (next && window < 10) {
                /* Stop at labels (scope boundaries) */
                if (next->opcode == TAC_LABEL || next->opcode == TAC_FUNCTION_LABEL) {
                    break;
                }

                /* Replace uses in op1 */
                if (next->op1 && strcmp(next->op1, temp) == 0) {
                    free(next->op1);
                    next->op1 = strdup(original);
                    replaced++;
                }

                /* Replace uses in op2 */
                if (next->op2 && strcmp(next->op2, temp) == 0) {
                    free(next->op2);
                    next->op2 = strdup(original);
                    replaced++;
                }

                /* Stop if temp is reassigned */
                if (next->result && strcmp(next->result, temp) == 0) {
                    break;
                }

                /* Stop if original is reassigned */
                if (next->result && strcmp(next->result, original) == 0) {
                    break;
                }

                next = next->next;
                window++;
            }

            if (replaced > 0) {
                optimizations += replaced;
                printf("[OPTIMIZER] Copy propagation: Replaced %d uses of %s with %s\n",
                       replaced, temp, original);
            }
        }

        inst = inst->next;
    }

    return optimizations;
}

/* Peephole Optimization: Optimize small instruction sequences
 * - Remove redundant loads
 * - Combine operations
 * - Strength reduction
 */
int peephole_optimization(TACCode* code) {
    int optimizations = 0;
    TACInstruction* inst = code->head;

    while (inst && inst->next) {
        /* Remove redundant load followed by assignment
         * Pattern: t0 = 5; x = t0; becomes x = 5;
         */
        if (inst->opcode == TAC_LOAD_CONST && inst->next->opcode == TAC_ASSIGN &&
            inst->result && inst->next->op1 &&
            strcmp(inst->result, inst->next->op1) == 0) {

            /* Merge the two instructions */
            TACInstruction* assign = inst->next;

            /* Change assignment to load_const directly */
            free(assign->op1);
            assign->op1 = strdup(inst->op1);
            assign->opcode = TAC_LOAD_CONST;

            /* Remove the first instruction */
            TACInstruction* to_remove = inst;

            /* Adjust pointers */
            if (code->head == inst) {
                code->head = inst->next;
            } else {
                /* Find previous instruction */
                TACInstruction* prev = code->head;
                while (prev && prev->next != inst) {
                    prev = prev->next;
                }
                if (prev) {
                    prev->next = inst->next;
                }
            }

            inst = inst->next;

            free(to_remove->result);
            free(to_remove->op1);
            free(to_remove->op2);
            free(to_remove->label);
            free(to_remove);

            code->instruction_count--;
            optimizations++;
            printf("[OPTIMIZER] Peephole: Merged load and assignment\n");
            continue;
        }

        /* Strength reduction: Division by power of 2 -> shift
         * Note: We track this as an optimization but don't change the TAC
         * (the code generator could handle this)
         */
        if (inst->opcode == TAC_DIV && inst->op2 && is_number(inst->op2)) {
            int divisor = atoi(inst->op2);
            /* Check if divisor is power of 2 */
            if (divisor > 0 && (divisor & (divisor - 1)) == 0) {
                /* This is a power of 2 - could be optimized to shift */
                /* For now, just log it */
                printf("[OPTIMIZER] Peephole: Division by power of 2 detected (can use shift)\n");
            }
        }

        inst = inst->next;
    }

    return optimizations;
}

/* Flow Optimization: Optimize control flow
 * - Remove unreachable labels
 * - Simplify branch chains
 * - Remove redundant jumps
 */
int flow_optimization(TACCode* code) {
    int optimizations = 0;
    TACInstruction* inst = code->head;
    TACInstruction* prev = NULL;

    while (inst) {
        /* Remove jump to next instruction
         * Pattern: goto L1; L1: ... becomes L1: ...
         */
        if (inst->opcode == TAC_GOTO && inst->next &&
            inst->next->opcode == TAC_LABEL &&
            inst->label && inst->next->label &&
            strcmp(inst->label, inst->next->label) == 0) {

            /* Remove the redundant goto */
            TACInstruction* to_remove = inst;

            if (prev) {
                prev->next = inst->next;
            } else {
                code->head = inst->next;
            }

            inst = inst->next;

            free(to_remove->result);
            free(to_remove->op1);
            free(to_remove->op2);
            free(to_remove->label);
            free(to_remove);

            code->instruction_count--;
            optimizations++;
            printf("[OPTIMIZER] Flow: Removed jump to next instruction\n");
            continue;
        }

        /* Remove if_false with constant condition */
        if (inst->opcode == TAC_IF_FALSE && inst->op1 && is_number(inst->op1)) {
            int condition = atoi(inst->op1);

            if (condition == 0) {
                /* Condition is always false - convert to unconditional jump */
                inst->opcode = TAC_GOTO;
                free(inst->op1);
                inst->op1 = NULL;
                optimizations++;
                printf("[OPTIMIZER] Flow: Converted if_false with constant to goto\n");
            } else {
                /* Condition is always true - remove the if_false */
                TACInstruction* to_remove = inst;

                if (prev) {
                    prev->next = inst->next;
                } else {
                    code->head = inst->next;
                }

                inst = inst->next;

                free(to_remove->result);
                free(to_remove->op1);
                free(to_remove->op2);
                free(to_remove->label);
                free(to_remove);

                code->instruction_count--;
                optimizations++;
                printf("[OPTIMIZER] Flow: Removed if_false with constant true condition\n");
                continue;
            }
        }

        prev = inst;
        inst = inst->next;
    }

    return optimizations;
}

/* Main optimization driver: Apply all optimizations iteratively */
TACCode* optimize_tac(TACCode* original_code, OptimizationStats* stats) {
    printf("\n============ CODE OPTIMIZATION STARTED =============\n\n");

    /* Initialize statistics */
    stats->constant_folds = 0;
    stats->dead_code_eliminated = 0;
    stats->copy_propagations = 0;
    stats->peephole_opts = 0;
    stats->total_optimizations = 0;

    /* Apply optimizations multiple times until no more improvements */
    int iteration = 0;
    int total_opts = 0;

    do {
        total_opts = 0;
        iteration++;

        printf("[OPTIMIZER] === Optimization Pass %d ===\n", iteration);

        /* Constant folding */
        int cf = constant_folding(original_code);
        stats->constant_folds += cf;
        total_opts += cf;

        /* Copy propagation */
        int cp = copy_propagation(original_code);
        stats->copy_propagations += cp;
        total_opts += cp;

        /* Peephole optimization */
        int po = peephole_optimization(original_code);
        stats->peephole_opts += po;
        total_opts += po;

        /* Flow optimization */
        int fo = flow_optimization(original_code);
        total_opts += fo;

        /* Dead code elimination (should be last) */
        int dce = eliminate_dead_code(original_code);
        stats->dead_code_eliminated += dce;
        total_opts += dce;

        printf("[OPTIMIZER] Pass %d: %d optimizations applied\n\n", iteration, total_opts);

        /* Limit iterations to prevent infinite loops */
        if (iteration >= 5) break;

    } while (total_opts > 0);

    stats->total_optimizations = stats->constant_folds +
                                 stats->copy_propagations +
                                 stats->peephole_opts +
                                 stats->dead_code_eliminated;

    printf("============ CODE OPTIMIZATION COMPLETE ============\n");
    printf("Total optimization passes: %d\n", iteration);
    printf("Total optimizations applied: %d\n\n", stats->total_optimizations);

    return original_code;
}

/* Print optimization statistics */
void print_optimization_stats(OptimizationStats* stats) {
    printf("\n=============== OPTIMIZATION STATISTICS ================\n\n");
    printf("Constant folding:          %d\n", stats->constant_folds);
    printf("Copy propagations:         %d\n", stats->copy_propagations);
    printf("Peephole optimizations:    %d\n", stats->peephole_opts);
    printf("Dead code eliminations:    %d\n", stats->dead_code_eliminated);
    printf("----------------------------------------\n");
    printf("Total optimizations:       %d\n", stats->total_optimizations);
    printf("\n========================================================\n\n");
}
