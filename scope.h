#include <string.h>
#include <stdlib.h>

typedef struct {
    char *name;
    char *type;
    int declared_line;
} Symbol;

typedef struct {
    Symbol **symbols;  // Array of symbol pointers
    int symbol_count;
} Scope;

Scope *create_scope();
void add_symbol(Scope *scope, char *name, char *type, int line);
Symbol *find_symbol(Scope *scope, char *name);
