/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/infix.c
 *
 */

Object * eval_string_infix_exp(char * op, Object * l, Object * r, Env * env) {
    char * m = NULL;

    Object * new = NULL;
    StringObject * nv = NULL;
    String * new_string = NULL;

    StringObject * ls = l->value, * rs = r->value;
    String * lss = ls->value, * rss = rs->value;

    if(strcmp(op, "+") != 0) {
        m = malloc(strlen(op) + strlen(r->type) + strlen(l->type) + 20);
        sprintf(m, "Unknown operator: %s%s%s", l->type, op, r->type);
        return new_error(m);
    }

    new = malloc(sizeof(Object));
    nv = malloc(sizeof(StringObject));
    new_string = string_new();

    string_cat(new_string, lss->string, 0);
    string_cat(new_string, rss->string, 0);

    new->type = STRING;
    new->value = nv;
    nv->value = new_string;

    return new;
}

Object * eval_integer_infix_exp(char * op, Object * l, Object * r, Env * env) {
    Object * obj = NULL;
    IntegerObject * int_obj = NULL;
    char * m = NULL;

    int left = ((IntegerObject *) l->value)->value;
    int right = ((IntegerObject *) r->value)->value;

    int arith = 0;

    if(strcmp(op, "+") == 0) {
        arith = left + right;
    } else if(strcmp(op, "-") == 0) {
        arith = left - right;
    } else if(strcmp(op, "*") == 0) {
        arith = left * right;
    } else if(strcmp(op, "/") == 0) {
        arith = left / right;
    } else if(strcmp(op, "<") == 0) {
        obj = eval_bool(left < right, env);
    } else if(strcmp(op, ">") == 0) {
        obj = eval_bool(left > right, env);
    } else if(strcmp(op, "==") == 0) {
        obj = eval_bool(left == right, env);
    } else if(strcmp(op, "!=") == 0) {
        obj = eval_bool(left != right, env);
    } else {
        m = malloc(strlen(op) + strlen(r->type) + strlen(l->type) + 20);
        sprintf(m, "Unknown operator: %s%s%s", l->type, op, r->type);
        return new_error(m);
    }

    if(strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 ||
        strcmp(op, "/") == 0) {

        obj = malloc(sizeof(Object));
        int_obj = malloc(sizeof(IntegerObject));
        int_obj->value = arith;
        obj->type = INT;
        obj->value = int_obj;
    }

    return obj;
}

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

    if(strcmp(l->type, INT) == 0 && strcmp(r->type, INT) == 0) {
        ret = eval_integer_infix_exp(op, l, r, env);
        infix_free_lr(lext, rext, l, r);
    } else if(strcmp(l->type, STRING) == 0 && strcmp(r->type, STRING) == 0) {
        ret = eval_string_infix_exp(op, l, r, env);
        infix_free_lr(lext, rext, l, r);
    } else if(strcmp(l->type, r->type) != 0) {
        m = malloc(strlen(l->type) + strlen(r->type) + strlen(op) + 19);
        sprintf(m, "Type mismatch: %s %s %s", l->type, op, r->type);
        infix_free_lr(lext, rext, l, r);
        ret = new_error(m);
    } else if(strcmp(op, "==") == 0) {
        ret = eval_bool(l == r, env);
    } else if(strcmp(op, "!=") == 0) {
        ret = eval_bool(l != r, env);
    } else {
        m = malloc(strlen(l->type) + strlen(r->type) + strlen(op) + 23);
        sprintf(m, "Unknown operation: %s %s %s", l->type, op, r->type);
        infix_free_lr(lext, rext, l, r);
        ret = new_error(m);
    }

    return ret;
}
