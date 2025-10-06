/*
 * AST.H - Abstract Syntax Tree Header
 * CST-405 Compiler Project
 *
 * This file defines the Abstract Syntax Tree structure used to represent
 * the parsed program in memory. Each node represents a language construct.
 */

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* AST Node Types - Each represents a different language construct */
typedef enum {
    NODE_PROGRAM,           /* Root node of the entire program */
    NODE_STATEMENT_LIST,    /* List of statements */
    NODE_DECLARATION,       /* Variable declaration: int x; */
    NODE_ASSIGNMENT,        /* Assignment: x = expr; */
    NODE_PRINT,            /* Print statement: print(expr); */
    NODE_WHILE,            /* While loop: while (cond) { stmts } (NEW FEATURE) */
    NODE_CONDITION,        /* Condition for while: expr relop expr (NEW FEATURE) */
    NODE_BINARY_OP,        /* Binary operation: expr + expr */
    NODE_IDENTIFIER,       /* Identifier (variable name) */
    NODE_NUMBER            /* Integer literal */
} NodeType;

/* Forward declaration */
struct ASTNode;

/* AST Node Structure - Represents one node in the syntax tree */
typedef struct ASTNode {
    NodeType type;                /* Type of this node */

    /* Node-specific data (union to save memory) */
    union {
        /* For numbers */
        int num_value;

        /* For identifiers and operators */
        char* str_value;

        /* For binary operations and conditions */
        struct {
            char* operator;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary_op;

        /* For assignments */
        struct {
            char* var_name;
            struct ASTNode* expr;
        } assignment;

        /* For print statements */
        struct {
            struct ASTNode* expr;
        } print;

        /* For while loops (NEW FEATURE) */
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_loop;

        /* For statement lists */
        struct {
            struct ASTNode* statement;
            struct ASTNode* next;
        } stmt_list;

        /* For program node */
        struct {
            struct ASTNode* statements;
        } program;

    } data;

    /* Source location for error reporting */
    int line_number;

} ASTNode;

/* AST CONSTRUCTION FUNCTIONS - Create nodes for different language constructs */

/* Create a program node (root of AST) */
ASTNode* create_program_node(ASTNode* statements);

/* Create a statement list node (chains statements together) */
ASTNode* create_statement_list_node(ASTNode* stmt, ASTNode* next);

/* Create a declaration node: int x; */
ASTNode* create_declaration_node(char* var_name);

/* Create an assignment node: x = expr; */
ASTNode* create_assignment_node(char* var_name, ASTNode* expr);

/* Create a print node: print(expr); */
ASTNode* create_print_node(ASTNode* expr);

/* Create a while loop node: while (cond) { body } (NEW FEATURE) */
ASTNode* create_while_node(ASTNode* condition, ASTNode* body);

/* Create a condition node: expr relop expr (NEW FEATURE) */
ASTNode* create_condition_node(ASTNode* left, char* op, ASTNode* right);

/* Create a binary operation node: left op right */
ASTNode* create_binary_op_node(char* op, ASTNode* left, ASTNode* right);

/* Create an identifier node */
ASTNode* create_id_node(char* name);

/* Create a number node */
ASTNode* create_num_node(int value);

/* AST UTILITY FUNCTIONS */

/* Print the AST in a tree format (for debugging and visualization) */
void print_ast(ASTNode* node, int indent_level);

/* Free all memory used by the AST */
void free_ast(ASTNode* node);

/* Get a string representation of a node type (for debugging) */
const char* node_type_to_string(NodeType type);

#endif /* AST_H */
