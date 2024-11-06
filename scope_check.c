#include "scope_check.h"
#include "symtab.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

static int has_error = 0;

void scope_check_program(block_t program) {
    /* Initialize the symbol table */
    symtab_initialize();

    /* Reset the error flag */
    has_error = 0;

    /* Start scope checking from the program's block */
    scope_check_block(&program);

    /* Finalize the symbol table */
    symtab_finalize();
}

void scope_check_block(block_t *block) {
    if (has_error) return;
    if (block == NULL) return;

    /* Enter a new scope */
    symtab_enter_scope();

    /* Check declarations */
    scope_check_const_decls(&block->const_decls);
    if (has_error) return;
    scope_check_var_decls(&block->var_decls);
    if (has_error) return;
    scope_check_proc_decls(&block->proc_decls);
    if (has_error) return;

    /* Check statements */
    scope_check_stmts(&block->stmts);

    /* Exit the scope */
    symtab_exit_scope();
}

void scope_check_const_decls(const_decls_t *decls) {
    if (has_error) return;
    if (decls == NULL || decls->start == NULL) return;

    const_decl_t *current_decl = decls->start;
    while (current_decl != NULL) {
        scope_check_const_decl(current_decl);
        if (has_error) return;
        current_decl = current_decl->next;
    }
}

void scope_check_const_decl(const_decl_t *decl) {
    if (has_error) return;
    if (decl == NULL) return;

    const_def_list_t *def_list = &decl->const_def_list;
    const_def_t *def = def_list->start;
    while (def != NULL) {
        scope_check_const_def(def);
        if (has_error) return;
        def = def->next;
    }
}

void scope_check_const_def(const_def_t *def) {
    if (has_error) return;
    if (def == NULL) return;

    ident_t *ident = &def->ident;
    number_t *number = &def->number;

    /* Check for duplicate declarations */
    sym_entry_t *entry = symtab_lookup_current_scope(ident->name);
    if (entry != NULL) {
        if (!has_error) {
            printf("%s: line %u constant \"%s\" is already declared as a %s\n",
                   ident->file_loc->filename, ident->file_loc->line, ident->name,
                   entry->kind == SYM_CONST ? "constant" :
                   entry->kind == SYM_VAR ? "variable" : "procedure");
            has_error = 1;
            return;
        }
    } else {
        symtab_insert(ident->name, SYM_CONST, number->value, ident->file_loc);
    }
}

void scope_check_var_decls(var_decls_t *decls) {
    if (has_error) return;
    if (decls == NULL || decls->var_decls == NULL) return;

    var_decl_t *current_decl = decls->var_decls;
    while (current_decl != NULL) {
        scope_check_var_decl(current_decl);
        if (has_error) return;
        current_decl = current_decl->next;
    }
}

void scope_check_var_decl(var_decl_t *decl) {
    if (has_error) return;
    if (decl == NULL) return;

    ident_list_t *idents = &decl->ident_list;
    ident_t *ident = idents->start;
    while (ident != NULL) {
        /* Check for duplicate declarations */
        sym_entry_t *entry = symtab_lookup_current_scope(ident->name);
        if (entry != NULL) {
            if (!has_error) {
                printf("%s: line %u variable \"%s\" is already declared as a %s\n",
                       ident->file_loc->filename, ident->file_loc->line, ident->name,
                       entry->kind == SYM_CONST ? "constant" :
                       entry->kind == SYM_VAR ? "variable" : "procedure");
                has_error = 1;
                return;
            }
        } else {
            symtab_insert(ident->name, SYM_VAR, 0, ident->file_loc);
        }
        ident = ident->next;
    }
}

void scope_check_proc_decls(proc_decls_t *decls) {
    if (has_error) return;
    if (decls == NULL || decls->proc_decls == NULL) return;

    proc_decl_t *current_decl = decls->proc_decls;
    while (current_decl != NULL) {
        scope_check_proc_decl(current_decl);
        if (has_error) return;
        current_decl = current_decl->next;
    }
}

void scope_check_proc_decl(proc_decl_t *decl) {
    if (has_error) return;
    if (decl == NULL) return;

    const char *name = decl->name;
    file_location *file_loc = decl->file_loc;

    /* Check for duplicate declarations */
    sym_entry_t *entry = symtab_lookup_current_scope(name);
    if (entry != NULL) {
        if (!has_error) {
            printf("%s: line %u procedure \"%s\" is already declared as a %s\n",
                   file_loc->filename, file_loc->line, name,
                   entry->kind == SYM_CONST ? "constant" :
                   entry->kind == SYM_VAR ? "variable" : "procedure");
            has_error = 1;
            return;
        }
    } else {
        symtab_insert(name, SYM_PROC, 0, file_loc);
    }

    /* Check the block within the procedure */
    scope_check_block(decl->block);
}

void scope_check_stmts(stmts_t *stmts) {
    if (has_error) return;
    if (stmts == NULL) return;

    if (stmts->stmts_kind == empty_stmts_e) {
        return;
    }

    stmt_t *stmt = stmts->stmt_list.start;
    while (stmt != NULL) {
        scope_check_stmt(stmt);
        if (has_error) break;  // Stop checking further statements after an error
        stmt = stmt->next;
    }
}

void scope_check_stmt(stmt_t *stmt) {
    if (has_error) return;
    if (stmt == NULL) return;

    switch (stmt->stmt_kind) {
        case assign_stmt:
            scope_check_assign_stmt(&stmt->data.assign_stmt);
            break;
        case call_stmt:
            scope_check_call_stmt(&stmt->data.call_stmt);
            break;
        case block_stmt:
            scope_check_block_stmt(&stmt->data.block_stmt);
            break;
        case if_stmt:
            scope_check_if_stmt(&stmt->data.if_stmt);
            break;
        case while_stmt:
            scope_check_while_stmt(&stmt->data.while_stmt);
            break;
        case read_stmt:
            scope_check_read_stmt(&stmt->data.read_stmt);
            break;
        case print_stmt:
            scope_check_print_stmt(&stmt->data.print_stmt);
            break;
        default:
            if (!has_error) {
                printf("Unknown statement kind.\n");
                has_error = 1;
            }
            break;
    }
}

void scope_check_assign_stmt(assign_stmt_t *stmt) {
    if (has_error) return;
    if (stmt == NULL) return;

    const char *name = stmt->name;
    file_location *file_loc = stmt->file_loc;

    sym_entry_t *entry = symtab_lookup(name);
    if (entry == NULL) {
        if (!has_error) {
            printf("%s: line %u identifier \"%s\" is not declared!\n",
                   file_loc->filename, file_loc->line, name);
            has_error = 1;
            return;
        }
    }

    /* If the identifier is a constant, silently ignore the assignment */
    if (entry->kind == SYM_CONST) {
        /* Assignment to a constant is ignored?; no error is reported */
        scope_check_expr(stmt->expr);
    } else if (entry->kind == SYM_VAR) {
        /* For variables, proceed to check the expression */
        scope_check_expr(stmt->expr);
    } else {
        /* Handle other kinds if necessary */
        if (!has_error) {
            printf("%s: line %u \"%s\" has an unsupported kind.\n",
                   file_loc->filename, file_loc->line, name);
            has_error = 1;
            return;
        }
    }
}

void scope_check_call_stmt(call_stmt_t *stmt) {
    if (has_error) return;
    if (stmt == NULL) return;

    const char *name = stmt->name;
    file_location *file_loc = stmt->file_loc;

    sym_entry_t *entry = symtab_lookup(name);
    if (entry == NULL) {
        if (!has_error) {
            printf("%s: line %u procedure \"%s\" is not declared!\n",
                   file_loc->filename, file_loc->line, name);
            has_error = 1;
            return;
        }
    } else if (entry->kind != SYM_PROC) {
        if (!has_error) {
            printf("%s: line %u \"%s\" is not a procedure\n",
                   file_loc->filename, file_loc->line, name);
            has_error = 1;
            return;
        }
    }
}

void scope_check_block_stmt(block_stmt_t *stmt) {
    if (has_error) return;
    if (stmt == NULL) return;

    scope_check_block(stmt->block);
}

void scope_check_if_stmt(if_stmt_t *stmt) {
    if (has_error) return;
    if (stmt == NULL) return;

    scope_check_condition(&stmt->condition);
    scope_check_stmts(stmt->then_stmts);
    if (stmt->else_stmts != NULL && stmt->else_stmts->stmts_kind != empty_stmts_e) {
        scope_check_stmts(stmt->else_stmts);
    }
}

void scope_check_while_stmt(while_stmt_t *stmt) {
    if (has_error) return;
    if (stmt == NULL) return;

    scope_check_condition(&stmt->condition);
    scope_check_stmts(stmt->body);
}

void scope_check_read_stmt(read_stmt_t *stmt) {
    if (has_error) return;
    if (stmt == NULL) return;

    const char *name = stmt->name;
    file_location *file_loc = stmt->file_loc;

    sym_entry_t *entry = symtab_lookup(name);
    if (entry == NULL) {
        if (!has_error) {
            printf("%s: line %u identifier \"%s\" is not declared!\n",
                   file_loc->filename, file_loc->line, name);
            has_error = 1;
            return;
        }
    } else if (entry->kind != SYM_VAR) {
        if (!has_error) {
            printf("%s: line %u \"%s\" is not a variable\n",
                   file_loc->filename, file_loc->line, name);
            has_error = 1;
            return;
        }
    }
}

void scope_check_print_stmt(print_stmt_t *stmt) {
    if (has_error) return;
    if (stmt == NULL) return;

    scope_check_expr(&stmt->expr);
}

void scope_check_condition(condition_t *cond) {
    if (has_error) return;
    if (cond == NULL) return;

    switch (cond->cond_kind) {
        case ck_rel:
            scope_check_expr(&cond->data.rel_op_cond.expr1);
            scope_check_expr(&cond->data.rel_op_cond.expr2);
            break;
        case ck_db:
            scope_check_expr(&cond->data.db_cond.dividend);
            scope_check_expr(&cond->data.db_cond.divisor);
            break;
        default:
            if (!has_error) {
                printf("Unknown condition kind.\n");
                has_error = 1;
            }
            break;
    }
}

void scope_check_expr(expr_t *expr) {
    if (has_error) return;
    if (expr == NULL) return;

    switch (expr->expr_kind) {
        case expr_ident: {
            ident_t *ident = &expr->data.ident;
            sym_entry_t *entry = symtab_lookup(ident->name);
            if (entry == NULL) {
                if (!has_error) {
                    printf("%s: line %u identifier \"%s\" is not declared!\n",
                           ident->file_loc->filename, ident->file_loc->line, ident->name);
                    has_error = 1;
                }
            }
            break;
        }
        case expr_number:
            /* Nothing to check */
            break;
        case expr_bin:
            scope_check_expr(expr->data.binary.expr1);
            if (has_error) return;
            scope_check_expr(expr->data.binary.expr2);
            break;
        case expr_negated:
            scope_check_expr(expr->data.negated.expr);
            break;
        default:
            if (!has_error) {
                printf("Unknown expression kind.\n");
                has_error = 1;
            }
            break;
    }
}
