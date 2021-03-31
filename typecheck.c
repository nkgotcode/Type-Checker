#include <stdio.h>
#include "typecheck.h"
#include "symbol.h"

S_table types;

// question 1
ty_node* string_array_array(){return ArrayTyNode(ArrayTyNode(StringTyNode()));}

ty_node* get_inner_type(ty_node* t){
    if (t->kind == array_ty) return t->inner_ty;
    else return NULL;
}

void record_type(char* var, ty_node* ty){
    S_enter(types, S_Symbol(var), ty);
}

ty_node* lookup_type(char* var){
    return S_look(types, S_Symbol(var));
}

int same_type(ty_node* ty1, ty_node* ty2){
    if(!ty1 || !ty2) return 0;
    if(ty1->kind != ty2->kind) return 0;
    if(ty1->kind == array_ty) return same_type(ty1->inner_ty, ty2->inner_ty);
    else return 1;
}

// Call this function to check whether two types match.
// The first argument is the type that should be there, and the second is the type that you want to check against that.
void check_types(ty_node* expected, ty_node* found){
    if(!same_type(expected, found))
        printf("Expected type %s but found type %s\n", ty_to_string(expected), ty_to_string(found));
    return;
}

ty_node* type_of_exp(exp_node* exp);

ty_node* type_of_lval(lval_node* l){
    if(!l) return NULL;
    switch(l->kind){
        case id_lval: {
            ty_node* ty = lookup_type(l->data.name);
            if(!ty) printf("Undefined variable %s\n", l->data.name);
            return ty;
        }
        case array_lval: {
            ty_node* index_ty = type_of_exp(l->data.array_ops.index);
            ty_node* base_ty = type_of_lval(l->data.array_ops.base);
            ty_node* i = IntTyNode();
            check_types(i, index_ty);
            if(!base_ty || base_ty->kind != array_ty){
                printf("Type %s is not an array type\n", ty_to_string(base_ty));
                return NULL;
            }
            else return base_ty->inner_ty;
        }
    }
}

ty_node* type_of_exp(exp_node* e){
    if(!e) return NULL;
    switch(e->kind){
        case int_exp: return IntTyNode();
        case string_exp: return StringTyNode();
        case lval_exp: return type_of_lval(e->data.lval);
        case call_exp: return lookup_type(e->data.call_ops.name);
        case binop_exp:
            // question 2 
            if(e->data.bin_ops.e1 && e->data.bin_ops.e2) {
                check_types(IntTyNode(), type_of_exp(e->data.bin_ops.e1));
                check_types(IntTyNode(), type_of_exp(e->data.bin_ops.e2));
                return IntTyNode();
            }
        default: return NULL;
    }
}

void record_params(param_list* l){
    if(!l) return;
    record_type(l->name, l->ty);
    record_params(l->next);
}

void typecheck_stmts(stmt_list* l);

void typecheck_stmt(stmt_node* s){
    if(!s) return;
    switch(s->kind){
        case vardec_stmt: {
            record_type(s->data.vardec_ops.name, s->data.vardec_ops.ty);
            exp_node* rhs = s->data.vardec_ops.rhs;
            if(rhs) check_types(s->data.vardec_ops.ty, type_of_exp(s->data.vardec_ops.rhs));
            return;
        }
        case arraydec_stmt: {
            record_type(s->data.arraydec_ops.name, ArrayTyNode(s->data.arraydec_ops.ty));
            exp_node* rhs = s->data.vardec_ops.rhs;
            if(rhs) check_types(s->data.vardec_ops.ty, type_of_exp(rhs));
            return;
        }
        case assign_stmt: {
            ty_node* lhs = type_of_lval(s->data.assign_ops.lhs);
            ty_node* rhs = type_of_exp(s->data.assign_ops.rhs);
            check_types(lhs, rhs);
            return;
        }
        case fundec_stmt:
            record_type(s->data.fundec_ops.name, s->data.fundec_ops.retty);
            S_beginScope(types);
            record_params(s->data.fundec_ops.params);
            typecheck_stmts(s->data.fundec_ops.body);
            S_endScope(types);
            return;
        case block_stmt:
            typecheck_stmts(s->data.body);
            return;
        // question 3
        case if_stmt:
            check_types(IntTyNode(), type_of_exp(s->data.if_ops.cond));
            typecheck_stmt(s->data.if_ops.then_stmt);
            typecheck_stmt(s->data.if_ops.else_stmt);
            return;
        case while_stmt:
            check_types(IntTyNode(), type_of_exp(s->data.while_ops.cond));
            typecheck_stmt(s->data.while_ops.body);
            return;
        default:
            printf("Found unsupported statement type\n");
    }
}

void typecheck_stmts(stmt_list* s){
    if(!s) return;
    typecheck_stmt(s->head);
    typecheck_stmts(s->next);
}

void typecheck(stmt_list* prog){
    types = S_empty();
    typecheck_stmts(prog);
}
