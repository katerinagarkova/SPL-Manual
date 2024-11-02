#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h" // Include the AST definitions
#include "symtab.h" // Assume you have a symbol table implementation

// Function prototypes
void check_declarations(AST ast);
void check_block(block_t *block, SymbolTable *symtab);
void check_stmt(stmt_t *stmt, SymbolTable *symtab);
void check_expr(expr_t *expr, SymbolTable *symtab);
void declare_ident(const char *name, SymbolTable *symtab, file_location *loc);
void use_ident(const char *name, file_location *loc, SymbolTable *symtab);

// The main function to start the declaration checking
void check_declarations(AST ast) {
    // Create a new symbol table for the program
    SymbolTable *symtab = create_symtab();

    // Check the block in the AST
    if (ast.generic.type_tag == block_ast) {
        check_block(&ast.block, symtab);
    } else {
        fprintf(stderr, "Error: AST root is not a block\n");
    }

    // Free the symbol table
    free_symtab(symtab);
}

// Function to check declarations in a block
void check_block(block_t *block, SymbolTable *symtab) {
    // Check constant declarations
    for (const_decl_t *const_decl = block->const_decls.start; const_decl != NULL; const_decl = const_decl->next) {
        // Check each constant definition
        for (const_def_t *const_def = const_decl->const_def_list.start; const_def != NULL; const_def = const_def->next) {
            declare_ident(const_def->ident.name, symtab, const_def->file_loc);
        }
    }

    // Check variable declarations
    for (var_decl_t *var_decl = block->var_decls.var_decls; var_decl != NULL; var_decl = var_decl->next) {
        declare_ident(var_decl->ident_list.start->name, symtab, var_decl->file_loc); // Assumes first ident is representative
    }

    // Check procedure declarations
    for (proc_decl_t *proc_decl = block->proc_decls.proc_decls; proc_decl != NULL; proc_decl = proc_decl->next) {
        declare_ident(proc_decl->name, symtab, proc_decl->file_loc);
    }

    // Check statements in the block
    check_stmt(block->stmts.file_loc, symtab);
}

// Function to check statements
void check_stmt(stmt_t *stmt, SymbolTable *symtab) {
    while (stmt != NULL) {
        switch (stmt->stmt_kind) {
            case assign_stmt:
                // Check the assignment statement
                use_ident(stmt->data.assign_stmt.name, stmt->file_loc, symtab);
                check_expr(stmt->data.assign_stmt.expr, symtab);
                break;

            case call_stmt:
                // Check the procedure call statement
                use_ident(stmt->data.call_stmt.name, stmt->file_loc, symtab);
                break;

            case if_stmt:
                // Check the condition and then statements
                check_expr(stmt->data.if_stmt.condition.expr, symtab);
                check_stmt(stmt->data.if_stmt.then_stmts->file_loc, symtab);
                if (stmt->data.if_stmt.else_stmts) {
                    check_stmt(stmt->data.if_stmt.else_stmts->file_loc, symtab);
                }
                break;

            case while_stmt:
                // Check the condition and body
                check_expr(stmt->data.while_stmt.condition.expr, symtab);
                check_stmt(stmt->data.while_stmt.body->file_loc, symtab);
                break;

            case read_stmt:
                // Check the read statement
                use_ident(stmt->data.read_stmt.name, stmt->file_loc, symtab);
                break;

            case print_stmt:
                // Check the print statement
                check_expr(&stmt->data.print_stmt.expr, symtab);
                break;

            case block_stmt:
                // Check nested blocks
                check_block(stmt->data.block_stmt.block, symtab);
                break;

            default:
                fprintf(stderr, "Error: Unknown statement type\n");
                break;
        }
        stmt = stmt->next;
    }
}

// Function to check expressions
void check_expr(expr_t *expr, SymbolTable *symtab) {
    switch (expr->expr_kind) {
        case expr_bin:
            // Check binary expressions
            check_expr(expr->data.binary.expr1, symtab);
            check_expr(expr->data.binary.expr2, symtab);
            break;

        case expr_negated:
            // Check negated expressions
            check_expr(expr->data.negated.expr, symtab);
            break;

        case expr_ident:
            // Check the identifier expression
            use_ident(expr->data.ident.name, expr->file_loc, symtab);
            break;

        case expr_number:
            // Numbers do not need checks
            break;

        default:
            fprintf(stderr, "Error: Unknown expression type\n");
            break;
    }
}

// Function to declare an identifier
void declare_ident(const char *name, SymbolTable *symtab, file_location *loc) {
    if (declare_symbol(symtab, name)) {
        printf("Declared identifier '%s' at %s:%u\n", name, loc->filename, loc->line);
    } else {
        fprintf(stderr, "Error: Duplicate declaration of identifier '%s' at %s:%u\n", name, loc->filename, loc->line);
    }
}
//declare_symbol(SymbolTable *symtab, char *name, char *type, int line)
// Function to use an identifier
void use_ident(const char *name, file_location *loc, SymbolTable *symtab) {
    if (!lookup(symtab, name)) {
        fprintf(stderr, "Error: Undefined identifier '%s' used at %s:%u\n", name, loc->filename, loc->line);
    }
}
