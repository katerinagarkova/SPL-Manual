#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdbool.h>
#include "ast.h"           // For ast_id type
#include "file_location.h" // For file_location

// Enum for symbol kinds
typedef enum {
    SYM_VAR,
    SYM_CONST,
    SYM_PROC
} sym_kind_t;

// Symbol table entry
typedef struct sym_entry {
    char *name;
    sym_kind_t kind;
    int value;                // For constants; variables and procedures may not need this
    struct sym_entry *next;   // For chaining in case of hash collisions
} sym_entry_t;

// Symbol table functions
void symtab_initialize(void);
void symtab_finalize(void);
void symtab_enter_scope(void);
void symtab_exit_scope(void);
void symtab_insert(const char *name, sym_kind_t kind, int value, file_location *loc);
sym_entry_t *symtab_lookup(const char *name);
sym_entry_t *symtab_lookup_current_scope(const char *name);

#endif // SYMTAB_H
