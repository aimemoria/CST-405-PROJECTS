/*
 * AST.C - Abstract Syntax Tree Implementation
 * CST-405 Compiler Project
 *
 * This file implements functions to create, manipulate, and display
 * the Abstract Syntax Tree (AST) that represents the program structure.
 */

#include "ast.h"

/* External line number from lexer for error tracking */
extern int line_num;

/* HELPER FUNCTION: Allocate and initialize a new AST node */
static ASTNode* create_ast_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Fatal Error: Memory allocation failed for AST node\n");
        exit(1);
    }
    node->type = type;
    node->line_number = line_num;
    return node;
}

/* Create a program node (root of the entire AST) */
ASTNode* create_program_node(ASTNode* statements) {
    ASTNode* node = create_ast_node(NODE_PROGRAM);
    node->data.program.statements = statements;
    return node;
}

/* Create a statement list node (links statements together) */
ASTNode* create_statement_list_node(ASTNode* stmt, ASTNode* next) {
    ASTNode* node = create_ast_node(NODE_STATEMENT_LIST);
    node->data.stmt_list.statement = stmt;
    node->data.stmt_list.next = next;
    return node;
}

/* Create a variable declaration node: int x; */
ASTNode* create_declaration_node(char* var_name) {
    ASTNode* node = create_ast_node(NODE_DECLARATION);
    node->data.str_value = strdup(var_name);  /* Copy the string */
    return node;
}

/* Create an assignment node: x = expr; */
ASTNode* create_assignment_node(char* var_name, ASTNode* expr) {
    ASTNode* node = create_ast_node(NODE_ASSIGNMENT);
    node->data.assignment.var_name = strdup(var_name);
    node->data.assignment.expr = expr;
    return node;
}

/* Create a print statement node: print(expr); */
ASTNode* create_print_node(ASTNode* expr) {
    ASTNode* node = create_ast_node(NODE_PRINT);
    node->data.print.expr = expr;
    return node;
}

/* Create a while loop node: while (condition) { body } (NEW FEATURE) */
ASTNode* create_while_node(ASTNode* condition, ASTNode* body) {
    ASTNode* node = create_ast_node(NODE_WHILE);
    node->data.while_loop.condition = condition;
    node->data.while_loop.body = body;
    return node;
}

/* Create a condition node: expr relop expr (NEW FEATURE) */
ASTNode* create_condition_node(ASTNode* left, char* op, ASTNode* right) {
    ASTNode* node = create_ast_node(NODE_CONDITION);
    node->data.binary_op.operator = strdup(op);
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

/* Create a binary operation node: left + right */
ASTNode* create_binary_op_node(char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_ast_node(NODE_BINARY_OP);
    node->data.binary_op.operator = strdup(op);
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

/* Create an identifier node (variable reference) */
ASTNode* create_id_node(char* name) {
    ASTNode* node = create_ast_node(NODE_IDENTIFIER);
    node->data.str_value = strdup(name);
    return node;
}

/* Create a number (literal) node */
ASTNode* create_num_node(int value) {
    ASTNode* node = create_ast_node(NODE_NUMBER);
    node->data.num_value = value;
    return node;
}

/* Get string name for a node type (for debugging/printing) */
const char* node_type_to_string(NodeType type) {
    switch (type) {
        case NODE_PROGRAM:        return "PROGRAM";
        case NODE_STATEMENT_LIST: return "STATEMENT_LIST";
        case NODE_DECLARATION:    return "DECLARATION";
        case NODE_ASSIGNMENT:     return "ASSIGNMENT";
        case NODE_PRINT:          return "PRINT";
        case NODE_WHILE:          return "WHILE";
        case NODE_CONDITION:      return "CONDITION";
        case NODE_BINARY_OP:      return "BINARY_OP";
        case NODE_IDENTIFIER:     return "IDENTIFIER";
        case NODE_NUMBER:         return "NUMBER";
        default:                  return "UNKNOWN";
    }
}

/* Print the AST in a tree format with indentation */
void print_ast(ASTNode* node, int indent) {
    if (!node) return;

    /* Print indentation */
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    /* Print node information based on type */
    switch (node->type) {
        case NODE_PROGRAM:
            printf("PROGRAM\n");
            print_ast(node->data.program.statements, indent + 1);
            break;

        case NODE_STATEMENT_LIST:
            printf("STATEMENT_LIST\n");
            print_ast(node->data.stmt_list.statement, indent + 1);
            print_ast(node->data.stmt_list.next, indent);
            break;

        case NODE_DECLARATION:
            printf("DECLARATION: int %s; (line %d)\n",
                   node->data.str_value, node->line_number);
            break;

        case NODE_ASSIGNMENT:
            printf("ASSIGNMENT: %s = (line %d)\n",
                   node->data.assignment.var_name, node->line_number);
            print_ast(node->data.assignment.expr, indent + 1);
            break;

        case NODE_PRINT:
            printf("PRINT (line %d)\n", node->line_number);
            print_ast(node->data.print.expr, indent + 1);
            break;

        case NODE_WHILE:
            printf("WHILE (line %d)\n", node->line_number);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("CONDITION:\n");
            print_ast(node->data.while_loop.condition, indent + 2);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("BODY:\n");
            print_ast(node->data.while_loop.body, indent + 2);
            break;

        case NODE_CONDITION:
            printf("CONDITION: %s (line %d)\n",
                   node->data.binary_op.operator, node->line_number);
            print_ast(node->data.binary_op.left, indent + 1);
            print_ast(node->data.binary_op.right, indent + 1);
            break;

        case NODE_BINARY_OP:
            printf("BINARY_OP: %s (line %d)\n",
                   node->data.binary_op.operator, node->line_number);
            print_ast(node->data.binary_op.left, indent + 1);
            print_ast(node->data.binary_op.right, indent + 1);
            break;

        case NODE_IDENTIFIER:
            printf("IDENTIFIER: %s (line %d)\n",
                   node->data.str_value, node->line_number);
            break;

        case NODE_NUMBER:
            printf("NUMBER: %d (line %d)\n",
                   node->data.num_value, node->line_number);
            break;

        default:
            printf("UNKNOWN NODE TYPE\n");
            break;
    }
}

/* Recursively free all memory used by the AST */
void free_ast(ASTNode* node) {
    if (!node) return;

    /* Free children first (post-order traversal) */
    switch (node->type) {
        case NODE_PROGRAM:
            free_ast(node->data.program.statements);
            break;

        case NODE_STATEMENT_LIST:
            free_ast(node->data.stmt_list.statement);
            free_ast(node->data.stmt_list.next);
            break;

        case NODE_DECLARATION:
            free(node->data.str_value);
            break;

        case NODE_ASSIGNMENT:
            free(node->data.assignment.var_name);
            free_ast(node->data.assignment.expr);
            break;

        case NODE_PRINT:
            free_ast(node->data.print.expr);
            break;

        case NODE_WHILE:
            free_ast(node->data.while_loop.condition);
            free_ast(node->data.while_loop.body);
            break;

        case NODE_CONDITION:
        case NODE_BINARY_OP:
            free(node->data.binary_op.operator);
            free_ast(node->data.binary_op.left);
            free_ast(node->data.binary_op.right);
            break;

        case NODE_IDENTIFIER:
            free(node->data.str_value);
            break;

        case NODE_NUMBER:
            /* No dynamic memory to free */
            break;
    }

    /* Finally, free the node itself */
    free(node);
}
