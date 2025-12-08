/*
 * DIAGNOSTICS.C - Enhanced Error Detection and Reporting Implementation
 * CST-405 Compiler Project
 */

#include "diagnostics.h"
#include <time.h>

/* Global diagnostic configuration */
DiagnosticConfig diag_config = {
    .verbose_mode = 0,
    .warnings_as_errors = 0,
    .show_warnings = 1,
    .show_notes = 0,
    .color_output = 0,
    .max_errors = 10,
    .log_file = NULL
};

/* Global diagnostic statistics */
DiagnosticStats diag_stats = {0, 0, 0, 0};

/* ANSI color codes (if supported) */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN    "\033[1;36m"

/* Initialize diagnostics system */
void init_diagnostics(int verbose, int warnings_as_errors) {
    diag_config.verbose_mode = verbose;
    diag_config.warnings_as_errors = warnings_as_errors;
    diag_config.show_notes = verbose;

    /* Reset statistics */
    diag_stats.note_count = 0;
    diag_stats.warning_count = 0;
    diag_stats.error_count = 0;
    diag_stats.fatal_count = 0;
}

/* Get diagnostic level string */
static const char* get_level_string(DiagnosticLevel level) {
    switch (level) {
        case DIAG_NOTE:    return "note";
        case DIAG_WARNING: return "warning";
        case DIAG_ERROR:   return "error";
        case DIAG_FATAL:   return "fatal error";
        default:           return "unknown";
    }
}

/* Get category string */
static const char* get_category_string(DiagnosticCategory category) {
    switch (category) {
        case DIAG_CAT_LEXICAL:      return "lexical";
        case DIAG_CAT_SYNTAX:       return "syntax";
        case DIAG_CAT_SEMANTIC:     return "semantic";
        case DIAG_CAT_OPTIMIZATION: return "optimization";
        case DIAG_CAT_CODEGEN:      return "codegen";
        case DIAG_CAT_SECURITY:     return "security";
        case DIAG_CAT_GENERAL:      return "general";
        default:                    return "unknown";
    }
}

/* Get color for diagnostic level */
static const char* get_level_color(DiagnosticLevel level) {
    if (!diag_config.color_output) return "";

    switch (level) {
        case DIAG_NOTE:    return COLOR_CYAN;
        case DIAG_WARNING: return COLOR_YELLOW;
        case DIAG_ERROR:   return COLOR_RED;
        case DIAG_FATAL:   return COLOR_MAGENTA;
        default:           return COLOR_RESET;
    }
}

/* Report a diagnostic message */
void report_diagnostic(DiagnosticLevel level, DiagnosticCategory category,
                      int line, int col, const char* format, ...) {
    /* Skip notes if not in verbose mode */
    if (level == DIAG_NOTE && !diag_config.show_notes) {
        return;
    }

    /* Skip warnings if disabled */
    if (level == DIAG_WARNING && !diag_config.show_warnings) {
        return;
    }

    /* Treat warnings as errors if configured */
    if (level == DIAG_WARNING && diag_config.warnings_as_errors) {
        level = DIAG_ERROR;
    }

    /* Update statistics */
    switch (level) {
        case DIAG_NOTE:    diag_stats.note_count++; break;
        case DIAG_WARNING: diag_stats.warning_count++; break;
        case DIAG_ERROR:   diag_stats.error_count++; break;
        case DIAG_FATAL:   diag_stats.fatal_count++; break;
    }

    /* Format the message */
    va_list args;
    va_start(args, format);

    /* Print to stderr */
    const char* color = get_level_color(level);
    const char* reset = diag_config.color_output ? COLOR_RESET : "";

    if (line > 0) {
        fprintf(stderr, "%s[%s:%d:%d] %s%s: ",
                color,
                get_category_string(category),
                line, col,
                get_level_string(level),
                reset);
    } else {
        fprintf(stderr, "%s[%s] %s%s: ",
                color,
                get_category_string(category),
                get_level_string(level),
                reset);
    }

    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    /* Also log to file if configured */
    if (diag_config.log_file) {
        if (line > 0) {
            fprintf(diag_config.log_file, "[%s:%d:%d] %s: ",
                    get_category_string(category),
                    line, col,
                    get_level_string(level));
        } else {
            fprintf(diag_config.log_file, "[%s] %s: ",
                    get_category_string(category),
                    get_level_string(level));
        }
        vfprintf(diag_config.log_file, format, args);
        fprintf(diag_config.log_file, "\n");
        fflush(diag_config.log_file);
    }

    va_end(args);

    /* Fatal errors terminate immediately */
    if (level == DIAG_FATAL) {
        fprintf(stderr, "\n*** Compilation terminated ***\n");
        exit(1);
    }
}

/* Convenience functions */
void diag_note(int line, int col, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    report_diagnostic(DIAG_NOTE, DIAG_CAT_GENERAL, line, col, "%s", buffer);
    va_end(args);
}

void diag_warning(int line, int col, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    report_diagnostic(DIAG_WARNING, DIAG_CAT_GENERAL, line, col, "%s", buffer);
    va_end(args);
}

void diag_error(int line, int col, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    report_diagnostic(DIAG_ERROR, DIAG_CAT_GENERAL, line, col, "%s", buffer);
    va_end(args);
}

void diag_fatal(int line, int col, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    report_diagnostic(DIAG_FATAL, DIAG_CAT_GENERAL, line, col, "%s", buffer);
    va_end(args);
}

void diag_security_warning(int line, int col, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    report_diagnostic(DIAG_WARNING, DIAG_CAT_SECURITY, line, col, "%s", buffer);
    va_end(args);
}

/* Print diagnostic summary */
void print_diagnostic_summary(void) {
    printf("\n");
    printf("======================================================\n");
    printf("||        COMPILATION DIAGNOSTICS SUMMARY         ||\n");
    printf("======================================================\n");
    printf("|| Notes:          %-4d                           ||\n", diag_stats.note_count);
    printf("|| Warnings:       %-4d                           ||\n", diag_stats.warning_count);
    printf("|| Errors:         %-4d                           ||\n", diag_stats.error_count);
    printf("|| Fatal Errors:   %-4d                           ||\n", diag_stats.fatal_count);
    printf("======================================================\n");

    if (diag_stats.error_count == 0 && diag_stats.fatal_count == 0) {
        printf("\n[OK] No errors detected - compilation successful!\n");
    } else {
        printf("\n[FAILED] Compilation failed with %d error(s)\n",
               diag_stats.error_count + diag_stats.fatal_count);
    }
}

/* Check if compilation should stop */
int should_stop_compilation(void) {
    if (diag_config.max_errors > 0 &&
        diag_stats.error_count >= diag_config.max_errors) {
        fprintf(stderr, "\n*** Too many errors (%d), stopping compilation ***\n",
                diag_stats.error_count);
        return 1;
    }
    return 0;
}

/* Get diagnostic statistics */
DiagnosticStats get_diagnostic_stats(void) {
    return diag_stats;
}

/* Reset diagnostic counters */
void reset_diagnostics(void) {
    diag_stats.note_count = 0;
    diag_stats.warning_count = 0;
    diag_stats.error_count = 0;
    diag_stats.fatal_count = 0;
}

/* Set diagnostic log file */
void set_diagnostic_log_file(const char* filename) {
    if (diag_config.log_file) {
        fclose(diag_config.log_file);
    }

    diag_config.log_file = fopen(filename, "w");
    if (!diag_config.log_file) {
        fprintf(stderr, "Warning: Could not open log file '%s'\n", filename);
    } else {
        time_t now = time(NULL);
        fprintf(diag_config.log_file, "=== Compilation Log ===\n");
        fprintf(diag_config.log_file, "Time: %s\n", ctime(&now));
    }
}

/* Close diagnostics system */
void close_diagnostics(void) {
    if (diag_config.log_file) {
        fprintf(diag_config.log_file, "\n=== End of Log ===\n");
        fclose(diag_config.log_file);
        diag_config.log_file = NULL;
    }
}

/* MEMORY SAFETY FUNCTIONS */

void* safe_malloc(size_t size, const char* context) {
    void* ptr = malloc(size);
    if (!ptr && size > 0) {
        diag_fatal(0, 0, "Memory allocation failed: %s (requested %zu bytes)",
                   context ? context : "unknown", size);
    }
    return ptr;
}

void* safe_calloc(size_t count, size_t size, const char* context) {
    void* ptr = calloc(count, size);
    if (!ptr && count > 0 && size > 0) {
        diag_fatal(0, 0, "Memory allocation failed: %s (requested %zu x %zu bytes)",
                   context ? context : "unknown", count, size);
    }
    return ptr;
}

void* safe_realloc(void* ptr, size_t size, const char* context) {
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr && size > 0) {
        diag_fatal(0, 0, "Memory reallocation failed: %s (requested %zu bytes)",
                   context ? context : "unknown", size);
    }
    return new_ptr;
}

char* safe_strdup(const char* str, const char* context) {
    if (!str) return NULL;

    char* new_str = strdup(str);
    if (!new_str) {
        diag_fatal(0, 0, "String duplication failed: %s",
                   context ? context : "unknown");
    }
    return new_str;
}

void safe_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

/* DEBUGGING SUPPORT */

void debug_print(const char* format, ...) {
    if (!diag_config.verbose_mode) return;

    va_list args;
    va_start(args, format);

    fprintf(stderr, "[DEBUG] ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);
}

void debug_ast_node(const char* label, void* node) {
    if (!diag_config.verbose_mode) return;
    fprintf(stderr, "[DEBUG] AST Node: %s at %p\n", label, node);
}

void debug_symbol(const char* label, void* symbol) {
    if (!diag_config.verbose_mode) return;
    fprintf(stderr, "[DEBUG] Symbol: %s at %p\n", label, symbol);
}
