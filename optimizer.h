/*
 * OPTIMIZER.H - Code Optimizer Header
 * CST-405 Compiler Project
 *
 * This file defines the optimization phase which improves the
 * intermediate representation (TAC) through various optimization techniques:
 * - Constant folding
 * - Dead code elimination
 * - Copy propagation
 * - Peephole optimization
 */

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ircode.h"

/* Optimization statistics */
typedef struct {
    int constant_folds;         /* Number of constant folding optimizations */
    int dead_code_eliminated;   /* Number of dead code instructions removed */
    int copy_propagations;      /* Number of copy propagations */
    int peephole_opts;          /* Number of peephole optimizations */
    int total_optimizations;    /* Total optimizations performed */
} OptimizationStats;

/* OPTIMIZATION FUNCTIONS */

/* Main optimization driver - applies all optimizations */
TACCode* optimize_tac(TACCode* original_code, OptimizationStats* stats);

/* Constant folding: evaluate constant expressions at compile time */
int constant_folding(TACCode* code);

/* Dead code elimination: remove unreachable or unused code */
int eliminate_dead_code(TACCode* code);

/* Copy propagation: replace copies with original values */
int copy_propagation(TACCode* code);

/* Peephole optimization: improve small sequences of instructions */
int peephole_optimization(TACCode* code);

/* Flow optimization: optimize control flow structures */
int flow_optimization(TACCode* code);

/* Print optimization statistics */
void print_optimization_stats(OptimizationStats* stats);

/* Helper function to check if a string is a number */
int is_number(const char* str);

/* Helper function to evaluate binary operation on constants */
int evaluate_binary_op(const char* op, int left, int right);

#endif /* OPTIMIZER_H */
