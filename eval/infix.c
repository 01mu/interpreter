/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/infix.c
 *
 */

#include "infix/array.c"
#include "infix/index.c"
#include "infix/int.c"
#include "infix/string.c"

bool is_infix_or_call(char * lext, char * rext) {
    return strcmp(lext, INFIX) == 0 || strcmp(rext, INFIX) == 0 ||
        strcmp(lext, CALL) == 0 || strcmp(rext, CALL) == 0;
}

void eval_free_infix(char * lt, Object * lo, char * rt, Object * ro) {
    if(lt != NULL && !is_bool_or_ident(lt)) {
        free_eval_expression(lt, lo, NULL, true);
    }

    if(rt != NULL && !is_bool_or_ident(rt)) {
        free_eval_expression(rt, ro, NULL, true);
    }
}

void infix_free_lr(char * lext, char * rext, Object * l, Object * r) {
    if(is_infix_or_call(lext, rext)) {
        eval_free_infix(l->type, l, r->type, r);
    } else {
        eval_free_infix(lext, l, rext, r);
    }
}

Object * eval_infix_expression(InfixExpression * iex, Env * env) {
    Object * l = NULL, * r = NULL, * ret = NULL;
    char * op = iex->operator, * lext = iex->left_expression_type,
        * rext = iex->right_expression_type, * m = NULL;
    char * err_lt = NULL;
    InfixExpression * err_idx = NULL;

    l = eval_expression(lext, iex->left, env);

    if(is_error(l)) {
        if(is_infix_or_call(lext, rext)) {
            err_idx = iex->left;
            err_lt = err_idx->left_expression_type;

            if(strcmp(err_lt, ERROR) == 0) {
                free_eval_expression(err_lt, err_idx->left, NULL, true);
            }
        } else {
            eval_free_infix(lext, l, NULL, NULL);
        }

        return l;
    }

    r = eval_expression(rext, iex->right, env);

    if(is_error(r)) {
        eval_free_infix(lext, l, rext, r);
        return r;
    }

    if(strcmp(lext, ARRAYIDX) == 0 && strcmp(op, "=") == 0) {
        IndexExpression * ie = iex->left;

        if(strcmp(ie->left_expression_type, ARRAYIDX) == 0 ||
            strcmp(ie->left_expression_type, IDENT) == 0) {

            Object * edit = eval_get_array_edit(iex, env);

            if(edit == false_bool) {
                infix_free_lr(lext, rext, l, r);
                return null_obj;
            }

            Object * j = copy_object(r);

            free_eval_expression(edit->type, edit, NULL, 0);
            edit->value = j->value;
            edit->type = j->type;
            free(j);
            infix_free_lr(lext, rext, l, r);
            return null_obj;
        }
    }

    if(strcmp(l->type, INT) == 0 && strcmp(r->type, INT) == 0) {
        ret = eval_integer_infix_exp(op, l, r, env);
    } else if(strcmp(l->type, STRING) == 0 && strcmp(r->type, STRING) == 0) {
        ret = eval_string_infix_exp(op, l, r, env);
    } else if(strcmp(l->type, ARRAY) == 0 && strcmp(r->type, ARRAY) == 0) {
        ret = eval_array_infix_exp(op, l, r, env);
    } else if(strcmp(l->type, r->type) != 0) {
        m = malloc(strlen(l->type) + strlen(r->type) + strlen(op) + 19);
        sprintf(m, "Type mismatch: %s %s %s", l->type, op, r->type);
        ret = new_error(m);
    } else if(strcmp(op, "==") == 0) {
        ret = eval_bool(is_truthy(l) == is_truthy(r), env);
    } else if(strcmp(op, "&&") == 0) {
        ret = eval_bool(is_truthy(l) && is_truthy(r), env);
    } else if(strcmp(op, "||") == 0) {
        ret = eval_bool(is_truthy(l) || is_truthy(r), env);
    } else if(strcmp(op, "!=") == 0) {
        ret = eval_bool(is_truthy(l) != is_truthy(r), env);
    } else {
        m = malloc(strlen(l->type) + strlen(r->type) + strlen(op) + 23);
        sprintf(m, "Unknown operation: %s %s %s", l->type, op, r->type);
        ret = new_error(m);
    }

    infix_free_lr(lext, rext, l, r);

    return ret;
}
