#ifndef SCOPE_CHECK_H
#define SCOPE_CHECK_H

#include "ast.h"

void scope_check_program(block_t program);
void scope_check_block(block_t *block);
void scope_check_const_decls(const_decls_t *decls);
void scope_check_const_decl(const_decl_t *decl);
void scope_check_const_def(const_def_t *def);
void scope_check_var_decls(var_decls_t *decls);
void scope_check_var_decl(var_decl_t *decl);
void scope_check_proc_decls(proc_decls_t *decls);
void scope_check_proc_decl(proc_decl_t *decl);
void scope_check_stmts(stmts_t *stmts);
void scope_check_stmt(stmt_t *stmt);
void scope_check_assign_stmt(assign_stmt_t *stmt);
void scope_check_call_stmt(call_stmt_t *stmt);
void scope_check_block_stmt(block_stmt_t *stmt);
void scope_check_if_stmt(if_stmt_t *stmt);
void scope_check_while_stmt(while_stmt_t *stmt);
void scope_check_read_stmt(read_stmt_t *stmt);
void scope_check_print_stmt(print_stmt_t *stmt);
void scope_check_condition(condition_t *cond);
void scope_check_expr(expr_t *expr);

#endif // SCOPE_CHECK_H
