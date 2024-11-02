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

    /* Report an error to the user on stderr */
extern void yyerror(const char *filename, const char *msg);

}    /* end of %code requires */

%verbose
%define parse.lac full
%define parse.error detailed

 /* the following passes file_name to yyerror,
    and declares it as an formal parameter of yyparse. */
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


%type <stmts> stmts
%type <empty> empty
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
%type <db_condition> dbCondition
%type <rel_op_condition> relOpCondition
%type <token> relOp

%type <expr> expr
%type <expr> term
%type <expr> factor

%start program

%code {
 /* extern declarations provided by the lexer */
extern int yylex(void);

 /* The AST for the program, set by the semantic action 
    for the nonterminal program. */
block_t progast; 

 /* Set the program's ast to be t */
extern void setProgAST(block_t t);
}

%%
 /* Write your grammar rules below and before the next %% */
program: block "." {setProgAST($1); } ;

block: "begin" constDecls varDecls procDecls stmts "end" {
    $$ = ast_block($1, $2, $3, $4, $5); 
} ;

constDecls: empty  {
    $$ = ast_const_decls_empty($1); 
}
| constDecl constDecls {
    $$ = ast_const_decls($1, $2); 
} ;

constDecl: constsym constDefList semisym {
    $$ = ast_const_decl($2); 
} ;

constDefList: constDef {
    $$ = ast_const_def_list_singleton($1); 
}
| constDefList commasym constDef {
    $$ = ast_const_def_list($1, $3); 
} ;

constDef: identsym eqsym numbersym {
    $$ = ast_const_def($1, $3); 
} ;

varDecls: empty  {
    $$ = ast_var_decls_empty($1); 
}
| varDecl varDecls {
    $$ = ast_var_decls($1, $2); 
} ;

varDecl: varsym identList semisym {
    $$ = ast_var_decl($2); 
} ;

identList: identsym {
    $$ = ast_ident_list_singleton($1); 
}
| identList commasym identsym {
    $$ = ast_ident_list($1, $3); 
} ;

procDecls: empty {
    $$ = ast_proc_decls_empty($1); 
}
| procDecl procDecls {
    $$ = ast_proc_decls($1, $2); 
} ;

procDecl: procsym identsym block semisym {
    $$ = ast_proc_decl($2, $3); 
} ;

stmts: empty {
    $$ = ast_stmts_empty($1); 
}
| stmtList {
    $$ = $1; 
} ;

stmtList: stmt {
    $$ = ast_stmt_list_singleton($1); 
}
| stmtList semisym stmt {
    $$ = ast_stmt_list($1, $3); 
} ;

stmt: assignStmt
| callStmt
| ifStmt
| whileStmt
| readStmt
| printStmt
| blockStmt ;

assignStmt: identsym becomessym expr {
    $$ = ast_stmt_assign($1, $3); 
} ;

callStmt: callsym identsym {
    $$ = ast_stmt_call($2); 
} ;

ifStmt: ifsym condition thensym stmts elsesym stmts endsym {
    $$ = ast_if_then_else_stmt($2, $4, $6); 
}
| ifsym condition thensym stmts endsym {
    $$ = ast_if_then_else_stmt($2, $4, NULL); 
} ;

whileStmt: whilesym condition dosym stmts endsym {
    $$ = ast_while_stmt($2, $4); 
} ;

readStmt: readsym identsym {
    $$ = ast_read_stmt($2); 
} ;

printStmt: printsym expr {
    $$ = ast_print_stmt($2); 
} ;

blockStmt: block {
    $$ = $1; 
} ;

condition: dbCondition
| relOpCondition ;

dbCondition: divisiblesym expr bysym expr {
    $$ = ast_db_condition($2, $4); 
} ;

relOpCondition: expr relOp expr {
    $$ = ast_rel_op_condition($1, $2, $3); 
} ;

relOp: eqeqsym
| neqsym
| ltsym
| leqsym
| gtsym
| geqsym ;

expr: term
| expr plussym term {
    $$ = create_bin_op($1, "+", $3); 
}
| expr minussym term {
    $$ = create_bin_op($1, "-", $3); 
} ;

term: factor
| term multsym factor {
    $$ = create_bin_op($1, "*", $3); 
}
| term divsym factor {
    $$ = create_bin_op($1, "/", $3); 
} ;

factor: identsym {
    $$ = ast_expr_ident($1); 
}
| numbersym {
    $$ = ast_expr_number($1); 
}
| lparensym expr rparensym {
    $$ = $2; 
} ;

%%

// Set the program's ast to be ast
void setProgAST(block_t ast) { progast = ast; }

