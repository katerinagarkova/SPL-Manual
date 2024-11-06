#include "symtab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_SCOPE_DEPTH 100

static sym_entry_t *symtab_stack[MAX_SCOPE_DEPTH];
static int current_scope = -1;

extern const char *file_name; // For error reporting

void symtab_initialize(void) {
    current_scope = -1;
}

void symtab_finalize(void) {
    while (current_scope >= 0) {
        symtab_exit_scope();
    }
}

void symtab_enter_scope(void) {
    current_scope++;
    if (current_scope >= MAX_SCOPE_DEPTH) {
        fprintf(stderr, "Error: Maximum scope depth exceeded.\n");
        exit(EXIT_FAILURE);
    }
    symtab_stack[current_scope] = NULL;
}

void symtab_exit_scope(void) {
    sym_entry_t *entry = symtab_stack[current_scope];
    while (entry != NULL) {
        sym_entry_t *temp = entry;
        entry = entry->next;
        free(temp->name);
        free(temp);
    }
    symtab_stack[current_scope] = NULL;
    current_scope--;
}

void symtab_insert(const char *name, sym_kind_t kind, int value, file_location *loc) {
    sym_entry_t *new_entry = malloc(sizeof(sym_entry_t));
    if (new_entry == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for sym_entry_t.\n");
        exit(EXIT_FAILURE);
    }
    new_entry->name = malloc(strlen(name) + 1);
    if (new_entry->name == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for symbol name.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_entry->name, name);
    new_entry->kind = kind;
    new_entry->value = value;
    new_entry->next = symtab_stack[current_scope];
    symtab_stack[current_scope] = new_entry;
}

sym_entry_t *symtab_lookup(const char *name) {
    for (int i = current_scope; i >= 0; i--) {
        sym_entry_t *entry = symtab_stack[i];
        while (entry != NULL) {
            if (strcmp(entry->name, name) == 0) {
                return entry;
            }
            entry = entry->next;
        }
    }
    return NULL; // Not found
}

sym_entry_t *symtab_lookup_current_scope(const char *name) {
    sym_entry_t *entry = symtab_stack[current_scope];
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL; // Not found in current scope
}
