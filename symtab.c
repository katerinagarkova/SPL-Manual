#include "symtab.h"

SymbolTable *create_symtab() {
    SymbolTable *symtab = malloc(sizeof(SymbolTable));
    symtab->stack = NULL;
    symtab->stack_size = 0;
    return symtab;
}

// Free the symbol table
void free_symtab(SymbolTable *symtab) {
    for (int i = 0; i < symtab->stack_size; ++i) {
        free(symtab->stack[i]);
    }
    free(symtab->stack);
    free(symtab);
}

void enter_scope(SymbolTable *symtab) {
    Scope *scope = create_scope();
    symtab->stack = realloc(symtab->stack, sizeof(Scope *) * (symtab->stack_size + 1));
    symtab->stack[symtab->stack_size++] = scope;
}

void exit_scope(SymbolTable *symtab) {
    if (symtab->stack_size > 0) {
        Scope *scope = symtab->stack[--symtab->stack_size];
        free(scope);
    }
}

Symbol *lookup(SymbolTable *symtab, char *name) {
    for (int i = symtab->stack_size - 1; i >= 0; i--) {
        Symbol *symbol = find_symbol(symtab->stack[i], name);
        if (symbol) {
            return symbol;
        }
    }
    return NULL;
}

void declare_symbol(SymbolTable *symtab, char *name, char *type, int line) {
    add_symbol(symtab->stack[symtab->stack_size - 1], name, type, line);
}
