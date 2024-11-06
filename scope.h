#ifndef SCOPE_H
#define SCOPE_H

void scope_initialize(void); // May not be needed if using symtab_initialize()
void scope_finalize(void);   // May not be needed if using symtab_finalize()
void scope_enter(void);
void scope_exit(void);

#endif // SCOPE_H
