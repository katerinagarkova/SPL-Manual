#ifndef SCOPE_CHECK_H
#define SCOPE_CHECK_H

#include "ast.h"          // Include your AST definitions
#include "symtab.h" // Include your symbol table definitions

// Function to check declarations in the given AST
void check_declarations(AST ast);

// Function to check declarations within a block
void check_block(block_t *block, SymbolTable *symtab);

// Function to check statements within a block
void check_stmt(stmt_t *stmt, SymbolTable *symtab);

// Function to check expressions
void check_expr(expr_t *expr, SymbolTable *symtab);

// Function to declare an identifier
void declare_ident(const char *name, SymbolTable *symtab, file_location *loc);

// Function to use an identifier
void use_ident(const char *name, file_location *loc, SymbolTable *symtab);

#endif 
