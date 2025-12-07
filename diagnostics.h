/*
 * DIAGNOSTICS.H - Enhanced Error Detection and Reporting
 * CST-405 Compiler Project
 *
 * This module provides comprehensive error detection, reporting,
 * warnings, and debugging support for the compiler.
 */

#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Diagnostic levels */
typedef enum {
    DIAG_NOTE,       /* Informational note */
    DIAG_WARNING,    /* Warning (non-fatal) */
    DIAG_ERROR,      /* Error (compilation fails) */
    DIAG_FATAL       /* Fatal error (immediate termination) */
} DiagnosticLevel;

/* Diagnostic categories */
typedef enum {
    DIAG_CAT_LEXICAL,
    DIAG_CAT_SYNTAX,
    DIAG_CAT_SEMANTIC,
    DIAG_CAT_OPTIMIZATION,
    DIAG_CAT_CODEGEN,
    DIAG_CAT_SECURITY,
    DIAG_CAT_GENERAL
} DiagnosticCategory;

/* Diagnostic statistics */
typedef struct {
    int note_count;
    int warning_count;
    int error_count;
    int fatal_count;
} DiagnosticStats;

/* Global diagnostics configuration */
typedef struct {
    int verbose_mode;           /* Verbose output */
    int warnings_as_errors;     /* Treat warnings as errors */
    int show_warnings;          /* Show warning messages */
    int show_notes;             /* Show informational notes */
    int color_output;           /* Use colored output (if terminal supports) */
    int max_errors;             /* Maximum errors before stopping (0 = unlimited) */
    FILE* log_file;             /* Optional log file */
} DiagnosticConfig;

/* Global diagnostic state */
extern DiagnosticConfig diag_config;
extern DiagnosticStats diag_stats;

/* DIAGNOSTIC FUNCTIONS */

/* Initialize diagnostics system */
void init_diagnostics(int verbose, int warnings_as_errors);

/* Report a diagnostic message */
void report_diagnostic(DiagnosticLevel level, DiagnosticCategory category,
                      int line, int col, const char* format, ...);

/* Convenience functions for common diagnostics */
void diag_note(int line, int col, const char* format, ...);
void diag_warning(int line, int col, const char* format, ...);
void diag_error(int line, int col, const char* format, ...);
void diag_fatal(int line, int col, const char* format, ...);

/* Security-specific diagnostics */
void diag_security_warning(int line, int col, const char* format, ...);

/* Print diagnostic summary */
void print_diagnostic_summary(void);

/* Check if compilation should stop (too many errors) */
int should_stop_compilation(void);

/* Get diagnostic statistics */
DiagnosticStats get_diagnostic_stats(void);

/* Reset diagnostic counters */
void reset_diagnostics(void);

/* Enable/disable log file */
void set_diagnostic_log_file(const char* filename);

/* Close diagnostics system */
void close_diagnostics(void);

/* MEMORY SAFETY CHECKS */

/* Safe malloc with error checking */
void* safe_malloc(size_t size, const char* context);

/* Safe calloc with error checking */
void* safe_calloc(size_t count, size_t size, const char* context);

/* Safe realloc with error checking */
void* safe_realloc(void* ptr, size_t size, const char* context);

/* Safe strdup with error checking */
char* safe_strdup(const char* str, const char* context);

/* Safe free (checks for NULL) */
void safe_free(void* ptr);

/* DEBUGGING SUPPORT */

/* Debug print (only shown in verbose mode) */
void debug_print(const char* format, ...);

/* Debug print AST node */
void debug_ast_node(const char* label, void* node);

/* Debug print symbol table entry */
void debug_symbol(const char* label, void* symbol);

#endif /* DIAGNOSTICS_H */
