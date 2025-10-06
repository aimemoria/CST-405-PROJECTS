/*
 * SYMTABLE.H - Symbol Table Header
 * CST-405 Compiler Project
 *
 * This file defines the symbol table structure used for tracking
 * variables, their types, and scope information during compilation.
 * Essential for semantic analysis and type checking.
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Data types supported by our language */
typedef enum {
    TYPE_INT,          /* Integer type (the only type currently) */
    TYPE_UNKNOWN       /* Unknown/undefined type (for error handling) */
} DataType;

/* Symbol table entry - Represents one variable in the program */
typedef struct Symbol {
    char* name;              /* Variable name (identifier) */
    DataType type;           /* Data type (int, etc.) */
    int is_initialized;      /* Flag: has this variable been assigned a value? */
    int declaration_line;    /* Source line where variable was declared */
    struct Symbol* next;     /* Next symbol in the hash chain (for collision handling) */
} Symbol;

/* Symbol Table - Hash table for efficient variable lookup */
typedef struct SymbolTable {
    Symbol** table;          /* Array of symbol pointers (hash table buckets) */
    int size;               /* Size of the hash table */
    int num_symbols;        /* Number of symbols currently stored */
} SymbolTable;

/* SYMBOL TABLE MANAGEMENT FUNCTIONS */

/* Create a new symbol table with given size */
SymbolTable* create_symbol_table(int size);

/* Add a symbol to the table
 * Returns 1 on success, 0 if symbol already exists (redeclaration error) */
int add_symbol(SymbolTable* table, const char* name, DataType type, int line);

/* Look up a symbol by name
 * Returns pointer to symbol if found, NULL otherwise */
Symbol* lookup_symbol(SymbolTable* table, const char* name);

/* Mark a symbol as initialized (for use-before-init checking) */
void mark_initialized(SymbolTable* table, const char* name);

/* Check if a symbol is initialized */
int is_initialized(SymbolTable* table, const char* name);

/* Print the entire symbol table (for debugging and output) */
void print_symbol_table(SymbolTable* table);

/* Free all memory used by the symbol table */
void free_symbol_table(SymbolTable* table);

/* Get string representation of a data type */
const char* type_to_string(DataType type);

/* HASH FUNCTION (Internal use) */
unsigned int hash(const char* str, int table_size);

#endif /* SYMTABLE_H */
