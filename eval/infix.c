/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/infix.c
 *
 */

bool eval_array_infix_comp(Array * arl, Array * arr) {
    int i;
    Object * lo = NULL, * ro = NULL;
    char * lot = NULL, * rot = NULL;
    StringObject * los = NULL, * ros = NULL;

    if(arl->size != arr->size) {
        return false;
    }

    for(i = 0; i < arl->size; i++) {
        lo = arl->array[i];
        ro = arr->array[i];
        lot = lo->type;
        rot = ro->type;

        if(strcmp(lot, INT) == 0 && strcmp(rot, INT) == 0) {
            if(((IntegerObject *) lo->value)->value !=
                ((IntegerObject *) ro->value)->value) {

                return false;
            }
        } else if(strcmp(lot, STRING) == 0 && strcmp(rot, STRING) == 0) {
            los = lo->value;
            ros = ro->value;

            if(strcmp(((String *) los->value)->string,
                ((String *) ros->value)->string) != 0) {

                return false;
            }
        } else if(strcmp(lot, BOOLEAN) == 0 && strcmp(rot, BOOLEAN) == 0) {
            if(lo->value != ro->value) {
                return false;
            }
        } else if(strcmp(lot, ARRAY) == 0 && strcmp(rot, ARRAY) == 0) {
            return eval_array_infix_comp(((ArrayObject *) lo->value)->elements,
                ((ArrayObject *) ro->value)->elements);
        } else {
            return false;
        }
    }

    return true;
}

Object * eval_array_infix_exp(char * op, Object * l, Object * r, Env * env) {
    int i;
    ArrayObject * all = l->value, * alr = r->value;
    Array * arl = all->elements, * arr = alr->elements, * new_arr = NULL;
    Object * ret = NULL;

    if(strcmp(op, "==") == 0) {
        ret = eval_bool(eval_array_infix_comp(arl, arr), env);
    } else if(strcmp(op, "!=") == 0) {
        ret = eval_bool(!eval_array_infix_comp(arl, arr), env);
    } else if(strcmp(op, "+") == 0) {
        ret = eval_new_array();
        new_arr = ((ArrayObject *) ret->value)->elements;

        for(i = 0; i < arl->size; i++) {
            array_insert(new_arr, copy_object(arl->array[i]));
        }

        for(i = 0; i < arr->size; i++) {
            array_insert(new_arr, copy_object(arr->array[i]));
        }
    }

    return ret;
}

Object * eval_string_infix_exp(char * op, Object * l, Object * r, Env * env) {
    char * m = NULL;
    String * lss = ((StringObject *) l->value)->value;
    String * rss = ((StringObject *) r->value)->value;
    Object * ret = NULL;

    if(strcmp(op, "+") == 0) {
        Object * new = eval_new_string();
        StringObject * sobj = new->value;

        string_cat(sobj->value, lss->string, 0);
        string_cat(sobj->value, rss->string, 0);

        ret = new;
    } else if(strcmp(op, "==") == 0) {
        ret = eval_bool(strcmp(lss->string, rss->string) == 0, env);
    } else if(strcmp(op, "!=") == 0) {
        ret = eval_bool(strcmp(lss->string, rss->string) != 0, env);
    } else {
        m = malloc(strlen(op) + strlen(r->type) + strlen(l->type) + 25);
        sprintf(m, "Unknown operator: %s %s %s", l->type, op, r->type);
        ret = new_error(m);
    }

    return ret;
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
    } else if(strcmp(op, "<=") == 0) {
        obj = eval_bool(left <= right, env);
    } else if(strcmp(op, ">=") == 0) {
        obj = eval_bool(left >= right, env);
    } else if(strcmp(op, "<") == 0) {
        obj = eval_bool(left < right, env);
    } else if(strcmp(op, ">") == 0) {
        obj = eval_bool(left > right, env);
    } else if(strcmp(op, "==") == 0) {
        obj = eval_bool(left == right, env);
    } else if(strcmp(op, "!=") == 0) {
        obj = eval_bool(left != right, env);
    } else if(strcmp(op, "&&") == 0) {
        obj = eval_bool(is_truthy(l) && is_truthy(r), env);
    } else if(strcmp(op, "||") == 0) {
        obj = eval_bool(is_truthy(l) || is_truthy(r), env);
    } else {
        m = malloc(strlen(op) + strlen(r->type) + strlen(l->type) + 25);
        sprintf(m, "Unknown operator: %s %s %s", l->type, op, r->type);
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

int get_index(IndexExpression * ie, Env * env) {
    ExpressionStatement * est = (ExpressionStatement *) ie->index;
    Object * io = eval_integer(est->expression, env);
    int v = ((IntegerObject *) io->value)->value;

    free_eval_expression(io->type, io, NULL, 1);

    return v;
}

Object * get_array(Object * obj, int idx) {
    Array * az = ((ArrayObject *) obj->value)->elements;

    return az->array[idx];
}

Object * eval_get_array_edit(InfixExpression * iex, Env * env) {
    Object * get = NULL;
    IndexExpression * ie = iex->left;
    IndexExpression * exp = ie;
    Array * hold = array_new();
    int * j, i;

    while(strcmp(exp->left_expression_type, ARRAYIDX) == 0) {
        j = malloc(sizeof(int));
        * j = get_index(exp, env);
        array_insert(hold, j);
        exp = exp->left;
    }

    j = malloc(sizeof(int));
    * j = get_index(exp, env);
    array_insert(hold, j);

    if(strcmp(exp->left_expression_type, ARRAY) == 0) {
        array_free(hold);
        return false_bool;
    }

    get = env_get(env, ((Identifier *) exp->left)->value);

    for(i = hold->size - 1; i >= 0; i--) {
        get = get_array(get, *((int *) hold->array[i]));
    }

    array_free(hold);

    return get;
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
