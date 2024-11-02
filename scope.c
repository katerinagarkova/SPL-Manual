#include "scope.h"

Scope *create_scope() {
    Scope *scope = malloc(sizeof(Scope));
    scope->symbols = NULL;
    scope->symbol_count = 0;
    return scope;
}

void add_symbol(Scope *scope, char *name, char *type, int line) {
    // Add a new symbol with attributes
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->name = strdup(name);
    symbol->type = strdup(type);
    symbol->declared_line = line;

    scope->symbols = realloc(scope->symbols, sizeof(Symbol *) * (scope->symbol_count + 1));
    scope->symbols[scope->symbol_count++] = symbol;
}

Symbol *find_symbol(Scope *scope, char *name) {
    for (int i = 0; i < scope->symbol_count; i++) {
        if (strcmp(scope->symbols[i]->name, name) == 0) {
            return scope->symbols[i];
        }
    }
    return NULL;
}
