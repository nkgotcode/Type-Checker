#ifndef AST_H
# define AST_H

typedef struct ty_node {
  enum { int_ty, string_ty, array_ty } kind;
  struct ty_node* inner_ty;  // only for array_ty
} ty_node;

ty_node* IntTyNode(); // makes a node representing type int
ty_node* StringTyNode(); // " type string
ty_node* ArrayTyNode(ty_node* ty); // takes a type t and returns t[]
// ArrayTyNode(IntTyNode()) would represent int[] equivalent to a node with kind array_ty and inner_ty int

typedef struct exp_node exp_node;

typedef struct lval_node {
  enum { id_lval, array_lval } kind;
  union { char* name; struct { struct lval_node* base; exp_node* index; } array_ops; } data;
} lval_node;

lval_node* IdNode();
lval_node* ArrayNode(lval_node* base, exp_node* index);

typedef enum { plus_op, minus_op, times_op, div_op, eq_op, neq_op, lt_op, le_op, and_op, or_op } binop;

typedef struct exp_list exp_list;

struct exp_node {
  enum { lval_exp, int_exp, string_exp, binop_exp, call_exp } kind;
  union { lval_node* lval; int ival; char* sval; struct { binop op; struct exp_node* e1; struct exp_node* e2; } bin_ops;
          struct { char* name; exp_list* args; } call_ops; } data;
};

exp_node* LvalNode(lval_node* l);
exp_node* IntNode(int val);
exp_node* StringNode(char* val);
exp_node* BinopNode(binop op, exp_node* left, exp_node* right);
exp_node* CallNode(char* name, exp_list* args);

struct exp_list {
  exp_node* head;
  struct exp_list* next;
};

exp_list* ExpList(exp_node* hd, exp_list* tl);

typedef struct param_list {
  ty_node* ty;
  char* name;
  struct param_list* next;
} param_list;

param_list* ParamList(ty_node* ty, char* name, param_list* tl);

typedef struct stmt_list stmt_list;

typedef struct stmt_node {
  enum { assign_stmt, vardec_stmt, arraydec_stmt, fundec_stmt, if_stmt, while_stmt, block_stmt, ret_stmt } kind;
  union { struct { lval_node* lhs; exp_node* rhs; } assign_ops;
          struct { ty_node* ty; char* name; exp_node* rhs; } vardec_ops;
          struct { ty_node* ty; int size; char* name; exp_node* rhs; } arraydec_ops;
          struct { ty_node* retty; char* name; param_list* params; stmt_list* body; } fundec_ops;
          struct { exp_node* cond; struct stmt_node* then_stmt; struct stmt_node* else_stmt; } if_ops;
          struct { exp_node* cond; struct stmt_node* body; } while_ops;
          stmt_list* body;
          exp_node* ret_exp;
        } data;
} stmt_node;

stmt_node* AssignNode(lval_node* lhs, exp_node* rhs);
stmt_node* VardecNode(ty_node* ty, char* name, exp_node* rhs);
stmt_node* ArraydecNode(ty_node* ty, int size, char* name, exp_node* rhs);
stmt_node* FundecNode(ty_node* retty, char* name, param_list* params, stmt_list* body);
stmt_node* IfNode(exp_node* cond, stmt_node* then_stmt, stmt_node* else_stmt);
stmt_node* WhileNode(exp_node* cond, stmt_node* body);
stmt_node* BlockNode(stmt_list* body);
stmt_node* RetNode(exp_node* e);

struct stmt_list {
  stmt_node* head;
  struct stmt_list* next;
};

stmt_list* StmtList(stmt_node* hd, stmt_list* tl);

const char* ty_to_string(ty_node* ty);
void print_ty(ty_node* ty);
void print_lval(lval_node* lval);
void print_exp(exp_node* exp);
void print_exp_list(exp_list* l);
void print_stmt(stmt_node* stmt, int indent);
void print_tree(stmt_list* program);

#endif
