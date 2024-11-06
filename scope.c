#include "scope.h"
#include "symtab.h"

// Since scope functions are essentially wrappers around symbol table scope functions
void scope_initialize(void) {
    symtab_initialize();
}

void scope_finalize(void) {
    symtab_finalize();
}

void scope_enter(void) {
    symtab_enter_scope();
}

void scope_exit(void) {
    symtab_exit_scope();
}
