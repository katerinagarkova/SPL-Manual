#include "scope.h"

typedef struct {
    Scope **stack;
    int stack_size;
} SymbolTable;

SymbolTable *create_symtab();
void free_symtab(SymbolTable *symtab);
void enter_scope(SymbolTable *symtab);
void exit_scope(SymbolTable *symtab);
Symbol *lookup(SymbolTable *symtab, char *name);
void declare_symbol(SymbolTable *symtab, char *name, char *type, int line);
