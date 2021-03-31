// goals: learn to use yacc, practice using grammars to describe program syntax,
// play with precedence and ambiguity, write something that can be used for later assignments

%{
#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#include "ast.h"

int yylex(void); /* function prototype */

stmt_list* prog;

void yyerror(const char *s)
{
 EM_error(EM_tokPos, "%s", s);
}
%}


%union {
	int ival;
	string sval;
  ty_node* ty;
  lval_node* lval;
  exp_node* exp;
  exp_list* exps;
  param_list* params;
  stmt_node* stmt;
  stmt_list* stmts;
	}

%error-verbose

%token <sval> ID STRING
%token <ival> INT

%token 
  COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK 
  LBRACE RBRACE
  PLUS MINUS TIMES DIVIDE EQ NEQ LT LE GT GE
  AND OR ASSIGN
  ARRAY IF THEN ELSE WHILE FOR DO 
  INTTY STRINGTY FUN RETURNS RETURN
%left OR
%left AND
%nonassoc EQ NEQ LT GT LE GE
%left PLUS MINUS
%left TIMES DIVIDE

%type <ty> ty
%type <lval> lvalue
%type <exp> exp
%type <exps> args
%type <params> tyargs
%type <stmt> stmt
%type <stmts> stmts

%start program

%%

program:	stmts   { prog = $1; }

stmts:
    /* empty */   { $$ = NULL; }
  | stmt stmts    { $$ = StmtList($1, $2); }

stmt:
    lvalue ASSIGN exp SEMICOLON                   { $$ = AssignNode($1, $3); }
  | ty ID SEMICOLON                               { $$ = VardecNode($1, $2, NULL); }
  | ty ID ASSIGN exp SEMICOLON                    { $$ = VardecNode($1, $2, $4); }
  | ty LBRACK INT RBRACK ID ASSIGN exp SEMICOLON  { $$ = ArraydecNode($1, $3, $5, $7); }
  | IF LPAREN exp RPAREN stmt ELSE stmt           { $$ = IfNode($3, $5, $7); }
  | WHILE LPAREN exp RPAREN stmt                  { $$ = WhileNode($3, $5); }
  | FOR LPAREN ID ASSIGN exp SEMICOLON exp SEMICOLON stmt RPAREN stmt   { $$ = BlockNode(StmtList(AssignNode(IdNode($3), $5),
                                                                                StmtList(WhileNode($7, BlockNode(StmtList($11, StmtList($9, NULL)))), NULL))); }
  | LBRACE stmts RBRACE                           { $$ = BlockNode($2); }
  | RETURN exp SEMICOLON                          { $$ = RetNode($2); }
  | FUN ID RETURNS ty LPAREN tyargs RPAREN LBRACE stmts RBRACE   { $$ = FundecNode($4, $2, $6, $9); }

exp:   
    lvalue                  { $$ = LvalNode($1); }
  | INT                     { $$ = IntNode($1); }
  | STRING                  { $$ = StringNode($1); }
  | exp PLUS exp            { $$ = BinopNode(plus_op, $1, $3); }
  | exp MINUS exp           { $$ = BinopNode(minus_op, $1, $3); }
  | exp TIMES exp           { $$ = BinopNode(times_op, $1, $3); }
  | exp DIVIDE exp          { $$ = BinopNode(div_op, $1, $3); }
  | exp EQ exp              { $$ = BinopNode(eq_op, $1, $3); }
  | exp NEQ exp             { $$ = BinopNode(neq_op, $1, $3); }
  | exp LT exp              { $$ = BinopNode(lt_op, $1, $3); }
  | exp GT exp              { $$ = BinopNode(lt_op, $3, $1); }
  | exp LE exp              { $$ = BinopNode(le_op, $1, $3); }
  | exp GE exp              { $$ = BinopNode(le_op, $3, $1); }
  | exp AND exp             { $$ = BinopNode(and_op, $1, $3); }
  | exp OR exp              { $$ = BinopNode(or_op, $1, $3); }
  | ID LPAREN args RPAREN   { $$ = CallNode($1, $3); }

ty:
    INTTY                   { $$ = IntTyNode(); }
  | STRINGTY                { $$ = StringTyNode(); }
  | ty LBRACK RBRACK        { $$ = ArrayTyNode($1); }

lvalue:
    ID                        { $$ = IdNode($1); }
  | lvalue LBRACK exp RBRACK  { $$ = ArrayNode($1, $3); }

args:
    /* empty */             { $$ = NULL; }
  | exp                     { $$ = ExpList($1, NULL); }
  | exp COMMA args          { $$ = ExpList($1, $3); }

tyargs:
    /* empty */             { $$ = NULL; }
  | ty ID                   { $$ = ParamList($1, $2, NULL); }
  | ty ID COMMA tyargs      { $$ = ParamList($1, $2, $4); }
