/* $Id: bison_spl_y_top.y,v 1.2 2024/10/09 18:18:55 leavens Exp $ */

%code top {
#include <stdio.h>
}

%code requires {

/* Including "ast.h" must be at the top, to define the AST type */
#include "ast.h"
#include "machine_types.h"
#include "parser_types.h"
#include "lexer.h"
#include "file_location.h"
#include "symtab.h"

/* Report an error to the user on stderr */
extern void yyerror(const char *filename, const char *msg);

/* Define yytokentype as int to match lexer.h */
typedef int yytokentype;

}    /* end of %code requires */

%verbose
%define parse.lac full
%define parse.error detailed

/* the following passes file_name to yyerror,
   and declares it as a formal parameter of yyparse. */
%parse-param { char const *file_name }

%token <ident> identsym
%token <number> numbersym
%token <token> plussym    "+"
%token <token> minussym   "-"
%token <token> multsym    "*"
%token <token> divsym     "/"

%token <token> periodsym  "."
%token <token> semisym    ";"
%token <token> eqsym      "="
%token <token> commasym   ","
%token <token> becomessym ":="
%token <token> lparensym  "("
%token <token> rparensym  ")"

%token <token> constsym   "const"
%token <token> varsym     "var"
%token <token> procsym    "proc"
%token <token> callsym    "call"
%token <token> beginsym   "begin"
%token <token> endsym     "end"
%token <token> ifsym      "if"
%token <token> thensym    "then"
%token <token> elsesym    "else"
%token <token> whilesym   "while"
%token <token> dosym      "do"
%token <token> readsym    "read"
%token <token> printsym   "print"
%token <token> divisiblesym "divisible"
%token <token> bysym      "by"

%token <token> eqeqsym    "=="
%token <token> neqsym     "!="
%token <token> ltsym      "<"
%token <token> leqsym     "<="
%token <token> gtsym      ">"
%token <token> geqsym     ">="

%type <block> program

%type <block> block

%type <const_decls> constDecls
%type <const_decl> constDecl
%type <const_def_list> constDefList
%type <const_def> constDef

%type <var_decls> varDecls
%type <var_decl> varDecl
%type <ident_list> identList

%type <proc_decls> procDecls
%type <proc_decl> procDecl
%type <ident> procHeader

%type <stmts> stmts
%type <stmt_list> stmtList
%type <stmt> stmt
%type <assign_stmt> assignStmt
%type <call_stmt> callStmt
%type <if_stmt> ifStmt
%type <while_stmt> whileStmt
%type <read_stmt> readStmt
%type <print_stmt> printStmt
%type <block_stmt> blockStmt

%type <condition> condition
%type <token> relOp

%type <expr> expr
%type <expr> term
%type <expr> factor

%start program

%initial-action {
    symtab_initialize();
}

%code {

extern int yylex(void);

/* The AST for the program, set by the semantic action 
   for the nonterminal program. */
block_t progast; 

/* Set the program's ast to be t */
void setProgAST(block_t t);

/* Helper function to create a file_location* from YYLTYPE */
static file_location* make_file_location(const char *file_name, YYLTYPE loc) {
    return file_location_make(file_name, loc.first_line);
}

// /* Implementation of yyerror */
// void yyerror(const char *filename, const char *msg);

}

%%

/* Grammar rules start here */

program:
    block periodsym
    {
        setProgAST($1);
    }
    ;

block:
    beginsym
    {
        symtab_enter_scope();
    }
    constDecls varDecls procDecls stmts endsym
    {
        $$ = ast_block($1, $3, $4, $5, $6);
        symtab_exit_scope();
    }
    ;

constDecls:
    constDecls constDecl
    {
        $$ = ast_const_decls($1, $2);
    }
    | %empty
    {
        $$ = ast_const_decls_empty(ast_empty(make_file_location(file_name, @$)));
    }
    ;

constDecl:
    constsym constDefList semisym
    {
        /* Declarations are handled during scope checking */
        $$ = ast_const_decl($2);
    }
    ;

constDefList:
    constDefList commasym constDef
    {
        $$ = ast_const_def_list($1, $3);
    }
    | constDef
    {
        $$ = ast_const_def_list_singleton($1);
    }
    ;

constDef:
    identsym eqsym numbersym
    {
        $$ = ast_const_def($1, $3);
    }
    ;

varDecls:
    varDecls varDecl
    {
        $$ = ast_var_decls($1, $2);
    }
    | %empty
    {
        $$ = ast_var_decls_empty(ast_empty(make_file_location(file_name, @$)));
    }
    ;

varDecl:
    varsym identList semisym
    {
        $$ = ast_var_decl($2);
    }
    ;

identList:
    identList commasym identsym
    {
        $$ = ast_ident_list($1, $3);
    }
    | identsym
    {
        $$ = ast_ident_list_singleton($1);
    }
    ;

procDecls:
    procDecls procDecl
    {
        $$ = ast_proc_decls($1, $2);
    }
    | %empty
    {
        $$ = ast_proc_decls_empty(ast_empty(make_file_location(file_name, @$)));
    }
    ;

procDecl:
    procHeader block semisym
    {
        $$ = ast_proc_decl($1, $2);
    }
    ;

procHeader:
    procsym identsym
    {
        $$ = $2; // Store ident in $$ to pass it to procDecl
    }
    ;

stmts:
    stmtList
    {
        $$ = ast_stmts($1);
    }
    | %empty
    {
        $$ = ast_stmts_empty(ast_empty(make_file_location(file_name, @$)));
    }
    ;

stmtList:
    stmtList semisym stmt
    {
        $$ = ast_stmt_list($1, $3);
    }
    | stmt
    {
        $$ = ast_stmt_list_singleton($1);
    }
    ;

stmt:
    assignStmt
    {
        $$ = ast_stmt_assign($1);
    }
    | callStmt
    {
        $$ = ast_stmt_call($1);
    }
    | blockStmt
    {
        $$ = ast_stmt_block($1);
    }
    | ifStmt
    {
        $$ = ast_stmt_if($1);
    }
    | whileStmt
    {
        $$ = ast_stmt_while($1);
    }
    | readStmt
    {
        $$ = ast_stmt_read($1);
    }
    | printStmt
    {
        $$ = ast_stmt_print($1);
    }
    ;

assignStmt:
    identsym becomessym expr
    {
        $$ = ast_assign_stmt($1, $3);
    }
    ;

callStmt:
    callsym identsym
    {
        $$ = ast_call_stmt($2);
    }
    ;

blockStmt:
    block
    {
        $$ = ast_block_stmt($1);
    }
    ;

ifStmt:
    ifsym condition thensym stmts elsesym stmts endsym
    {
        $$ = ast_if_then_else_stmt($2, $4, $6);
    }
    | ifsym condition thensym stmts endsym
    {
        $$ = ast_if_then_stmt($2, $4);
    }
    ;

whileStmt:
    whilesym condition dosym stmts endsym
    {
        $$ = ast_while_stmt($2, $4);
    }
    ;

readStmt:
    readsym identsym
    {
        $$ = ast_read_stmt($2);
    }
    ;

printStmt:
    printsym expr
    {
        $$ = ast_print_stmt($2);
    }
    ;

condition:
    divisiblesym expr bysym expr
    {
        $$ = ast_condition_db(ast_db_condition($2, $4));
    }
    | expr relOp expr
    {
        $$ = ast_condition_rel_op(ast_rel_op_condition($1, $2, $3));
    }
    ;

relOp:
    eqeqsym
    {
        $$ = $1;
    }
    | eqsym
    {
        $$ = $1;
    }
    | neqsym
    {
        $$ = $1;
    }
    | ltsym
    {
        $$ = $1;
    }
    | leqsym
    {
        $$ = $1;
    }
    | gtsym
    {
        $$ = $1;
    }
    | geqsym
    {
        $$ = $1;
    }
    ;

expr:
    expr plussym term
    {
        $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3));
    }
    | expr minussym term
    {
        $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3));
    }
    | term
    {
        $$ = $1;
    }
    ;

term:
    term multsym factor
    {
        $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3));
    }
    | term divsym factor
    {
        $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3));
    }
    | factor
    {
        $$ = $1;
    }
    ;

factor:
    identsym
    {
        $$ = ast_expr_ident($1);
    }
    | numbersym
    {
        $$ = ast_expr_number($1);
    }
    | lparensym expr rparensym
    {
        $$ = $2;
    }
    | minussym factor
    {
        $$ = ast_expr_signed_expr($1, $2);
    }
    ;

%%

/* User code section */

/* Set the program's ast to be t */
void setProgAST(block_t t) { progast = t; }
