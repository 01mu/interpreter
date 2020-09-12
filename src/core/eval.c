/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/eval.c
 *
 */

bool eval_is_bool(char * t) {
    return (strcmp(t, TRUE) == 0) || (strcmp(t, FALSE) == 0);
}

bool free_eval_expression(char * ext, Object * obj, Env * env) {
    if(strcmp(ext, INT) == 0) {
        free(obj->value);
        return true;
    } else if(strcmp(ext, IDENT) == 0) {

    } else if(strcmp(ext, PREFIX) == 0) {
        if(strcmp(obj->type, INT) == 0) {
           free(obj->value);
        }
        return true;
    } else if(strcmp(ext, INFIX) == 0) {
        if(strcmp(obj->type, INT) == 0) {
           free(obj->value);
        }
        return true;
    } else if(strcmp(ext, BOOLEAN) == 0) {

    } else if(strcmp(ext, IF) == 0) {

    } else if(strcmp(ext, FUNCTION) == 0) {

    } else if(strcmp(ext, CALL) == 0) {

    }
}

Object * update_eval_value(Object ** obj, Object * new, Env * env) {
    char * old_t = (*obj)->type, * new_t = new->type;
    IntegerObject * iob_old, * iob_new;
    bool same_type = strcmp(old_t, new_t) == 0;

    if(same_type && strcmp(new_t, INT) == 0) {
        iob_old = (IntegerObject *) (*obj)->value;
        iob_new = (IntegerObject *) new->value;
        iob_old->value = iob_new->value;
        free_eval_expression(new_t, new, env);
        free(new);
        return (*obj);
    } else if(strcmp(old_t, BOOLEAN) != 0 && strcmp(new_t, BOOLEAN) == 0) {
        free_eval_expression(old_t, *obj, env);
        free(*obj);
        (*obj) = new;
        return (*obj);
    } else if(strcmp(old_t, BOOLEAN) == 0) {
        (*obj) = new;
        return (*obj);
    }
}

bool is_error(Object * obj) {
    if(obj != NULL) {
        return strcmp(obj->type, ERROR)== 0;
    }

    return false;
}

Object * new_error(char * msg) {
    Object * obj = malloc(sizeof(Object));
    ErrorObject * err = malloc(sizeof(ErrorObject));
    obj->type = ERROR;
    obj->value = err;
    err->message = malloc(strlen(msg) + 1);
    err->message[0] = '\0';
    strcat(err->message, msg);
    free(msg);
    return obj;
}

void init_bool(Object ** b, bool lit) {
    (*b) = malloc(sizeof(Object));
    (*b)->type = BOOLEAN;
    (*b)->value = malloc(sizeof(BooleanObject));
    ((BooleanObject *) (*b)->value)->value = lit;
}

void free_bool(Object * b) {
    free(b->value);
    free(b);
}

char * inspect_error_object(ErrorObject * eobj) {
    return eobj->message;
}

char * inspect_null_object(NullObject * nobj) {
    return "null";
}

int inspect_integer_object(IntegerObject * iobj) {
    return iobj->value;
}

bool inspect_boolean_object(BooleanObject * bobj) {
    return bobj->value;
}

void print_object(Object * obj) {
    if(strcmp(obj->type, "INT") == 0) {
        printf("%i\n", inspect_integer_object((IntegerObject *) obj->value));
    } else if(strcmp(obj->type, BOOLEAN) == 0) {
        printf("%i\n", inspect_boolean_object((BooleanObject *) obj->value));
    } else if(strcmp(obj->type, "ERROR") == 0) {
        printf("%s\n", ((ErrorObject *) obj->value)->message);
    }
}

bool is_truthy(Object * obj) {
    if(obj == null_obj) {
        return false;
    } else if(obj == true_bool) {
        return true;
    } else if(obj == false_bool) {
        return false;
    } else {
        return true;
    }
}

Object * eval_integer(IntegerLiteral * il, Env * env) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));
    iobj->value = il->value;
    obj->type = INT;
    obj->value = iobj;
    return obj;
}

Object * eval_bool(bool b, Env * env) {
    if(b) {
        return true_bool;
    }

    return false_bool;
}

Object * eval_bang_operator_expression(Object * right, Env * env) {
    Object * ret = NULL;

    if(right == true_bool) {
        ret = false_bool;
    } else if(right == false_bool) {
        ret = true_bool;
    } else if(right == null_obj) {
        ret = true_bool;
    } else {
        ret = false_bool;
    }

    return ret;
}

Object * eval_minus_prefix_operator_expression(Object * right, Env * env) {
    IntegerObject * r = NULL;
    char * m = NULL;

    if(strcmp(right->type, INT) != 0) {
        m = malloc(strlen(right->type) + 25);
        sprintf(m, "Unknown operator: -%s", right->type);
        return new_error(m);
    }

    r = right->value;
    r->value = r->value * -1;

    return right;
}

Object * eval_prefix_expression(PrefixExpression * pex, Env * env) {
    Object * right = eval_expression(pex->expression_type, pex->right, env);
    Object * ret = NULL;
    char * operator = pex->operator, * m;

    if(is_error(right)) {
        return right;
    }

    if(strcmp(operator, "!") == 0 ) {
        ret = eval_bang_operator_expression(right, env);

        if(strcmp(pex->expression_type, IDENT) != 0 &&
            !eval_is_bool(pex->expression_type)) {

            if(free_eval_expression(pex->expression_type, right, env)) {
                free(right);
            }
        }
    } else if(strcmp(operator, "-") == 0) {
        ret = eval_minus_prefix_operator_expression(right, env);
    } else {
        m = malloc(strlen(operator) + strlen(right->type) + 20);
        sprintf(m, "Unknown operator: %s%s", operator, right->type);
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
        sprintf(m, "Unknown operator: %s%s%s\n", l->type, op, r->type);
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

Object * eval_infix_expression(InfixExpression * iex, Env * env) {
    Object * l, * r, * ret;
    bool left_is_ident = false, right_is_ident = false;
    char * m = NULL;
    char * op = iex->operator, * lext = iex->left_expression_type,
        * rext = iex->right_expression_type;

    l = eval_expression(lext, iex->left, env);

    if(is_error(l)) {
        return l;
    }

    r = eval_expression(rext, iex->right, env);

    if(is_error(r)) {
        return r;
    }

    if(strcmp(lext, IDENT) == 0 || eval_is_bool(lext)) {
        left_is_ident = true;
    }

    if(strcmp(rext, IDENT) == 0 || eval_is_bool(rext)) {
        right_is_ident = true;
    }

    if(strcmp(l->type, INT) == 0 && strcmp(r->type, INT) == 0) {
        ret = eval_integer_infix_exp(op, l, r, env);

        if(!left_is_ident) {
            if(free_eval_expression(lext, l, env)) {
                free(l);
            }
        }

        if(!right_is_ident) {
            if(free_eval_expression(rext, r, env)) {
                free(r);
            }
        }
    } else if(strcmp(l->type, r->type) != 0) {
        m = malloc(strlen(l->type) + strlen(r->type) + strlen(op) + 19);
        sprintf(m, "Type mismatch: %s %s %s", l->type, op, r->type);

        if(!left_is_ident) {
            if(free_eval_expression(rext, l, env)) {
                free(l);
            }
        }

        if(!right_is_ident) {
            if(free_eval_expression(rext, r, env)) {
                free(r);
            }
        }

        ret = new_error(m);
    } else if(strcmp(op, "==") == 0) {
        ret = eval_bool(l == r, env);
    } else if(strcmp(op, "!=") == 0) {
        ret = eval_bool(l != r, env);
    } else {
        m = malloc(strlen(l->type) + strlen(r->type) + strlen(op) + 23);
        sprintf(m, "Unknown operation: %s %s %s", l->type, op, r->type);
        ret = new_error(m);
    }

    return ret;
}

Object * eval_if_expression(IfExpression * iex, Env * env) {
    Object * cond = eval_expression(iex->condition_type,iex->condition, env);
    BlockStatement * bs = NULL;

    if(is_error(cond)) {
        return cond;
    }

    if(is_truthy(cond)) {
        bs = (BlockStatement *) iex->consequence;
        return eval_statements(bs->statements, bs->sc, env);
    } else if(iex->alternative != NULL) {
        bs = (BlockStatement *) iex->alternative;
        return eval_statements(bs->statements, bs->sc, env);
    } else {
        return null_obj;
    }
}

Object * eval_identifier(Identifier * ident, Env * env) {
    Object * get = env_get(env, ident->value);
    char * msg = NULL;

    if(get == NULL) {
        msg = malloc(strlen(ident->value) + 28);
        sprintf(msg, "Identifier not found: %s", ident->value);
        return new_error(msg);
    }

    return get;
}

Object * eval_function_literal(FunctionLiteral * fl, Env * env) {
    Object * obj = malloc(sizeof(Object));
    Function * function = malloc(sizeof(Function));

    obj->type = FUNCTION;
    obj->value = function;

    function->parameters = fl->parameters;
    function->body = fl->body;
    function->env = env;
    function->pc = fl->pc;

    return obj;
}

Object ** eval_expressions(ExpressionStatement ** args, int c, Env * env) {
    Object ** objects = malloc(sizeof(Object *)), ** err = NULL;
    Object * eval = NULL;
    ExpressionStatement * est = NULL;
    int i;

    for(i = 0; i < c; i++) {
        objects = realloc(objects, sizeof(Object *) * (i + 1));
        est = (ExpressionStatement *) args[i];
        eval = eval_expression(est->expression_type, est->expression, env);
        objects[i] = eval;

        if(is_error(eval)) {
            err = malloc(sizeof(Object *));
            err[0] = eval;
            return err;
        }
    }

    return objects;
}

Env * extend_function_env(Function * func, Object ** args) {
    Env * env = env_new_enclosed(func->env);
    Identifier * ident = NULL;
    int i;

    for(i = 0; i < func->pc; i++) {
        ident = (Identifier *) func->parameters[i];
        env_set(env, ident->value, args[i]);
    }

    return env;
}

Object * unwrap_return_value(Object * obj) {
    ReturnValue * rv = NULL;

    if(strcmp(obj->type, RETURN) == 0) {
        rv = (ReturnValue *) obj->value;
        return rv->value;
    }

    return obj;
}

Object * apply_function(Object * obj, Object ** args) {
    char * m = NULL;
    Object * evaluated = NULL;
    Function * func = (Function *) obj->value;
    Env * extended_env = extend_function_env(func, args);
    BlockStatement * bs = func->body;

    if(strcmp(FUNCTION, obj->type) != 0) {
        m = malloc(strlen(obj->type) + 23);
        sprintf(m, "Not a function: %s\n", obj->type);
        return new_error(m);
    }

    evaluated = eval_statements(bs->statements, bs->sc, extended_env);

    return unwrap_return_value(evaluated);
}

Object * eval_call_expression(CallExpression * ce, Env * env) {
    Object * obj = eval_expression(ce->function_type, ce->function, env);
    Object ** args;

    if(is_error(obj)) {
        return obj;
    }

    args = eval_expressions(ce->arguments, ce->ac, env);

    if((sizeof(args) / sizeof(args[0])) == 1 && is_error(args[0])) {
        return args[0];
    }

    return apply_function(obj, args);
}

Object * eval_expression(char * ext, void * est, Env * env) {
    Object * res = NULL, * left = NULL, * right = NULL;
    PrefixExpression * pex = NULL, * in = NULL;
    InfixExpression * iex = NULL;

    if(strcmp(ext, INT) == 0) {
        res = eval_integer((IntegerLiteral *) est, env);
    } else if(strcmp(ext, IDENT) == 0) {
        res = eval_identifier((Identifier *) est, env);
    } else if(strcmp(ext, PREFIX) == 0) {
        pex = (PrefixExpression *) est;
        res = eval_prefix_expression(pex, env);
    } else if(strcmp(ext, INFIX) == 0) {
        iex = (InfixExpression *) est;
        res = eval_infix_expression(iex, env);
    } else if(strcmp(ext, TRUE) == 0 || strcmp(ext, FALSE) == 0) {
        res = eval_bool(((Boolean *) est)->value, env);
    } else if(strcmp(ext, IF) == 0) {
        res = eval_if_expression((IfExpression *) est, env);
    } else if(strcmp(ext, FUNCTION) == 0) {
        res = eval_function_literal((FunctionLiteral *) est, env);
    } else if(strcmp(ext, CALL) == 0) {
        res = eval_call_expression((CallExpression *) est, env);
    }

    return res;
}

Object * eval_return_statement(ExpressionStatement * est, Env * env) {
    Object * obj = malloc(sizeof(Object));
    ReturnValue * rv = malloc(sizeof(ReturnValue));

    obj->type = RETURN;
    obj->value = rv;

    rv->value = eval_expression(est->expression_type, est->expression, env);

    if(rv->value != NULL) {
        rv->type = ((Object *) rv->value)->type;
    }

    if(is_error(rv->value)) {
        return rv->value;
    }

    return obj;
}

Object * eval_let_statement(ExpressionStatement * est, Env * env, char * name) {
    Object * obj = eval_expression(est->expression_type, est->expression, env);
    Object * get  = env_get(env, name);
    Object ** t = NULL;
    char * th = NULL;

    if(is_error(obj)) {
        return obj;
    }

    if(get != NULL) {
        t = (Object **) (&(hash_map_find(env->store, name)->data));
        return update_eval_value(t, obj, env);
    }

    th = malloc(strlen(name) + 1);
    th[0] = '\0';
    strcpy(th, name);

    env_set(env, th, obj);
}

Object * eval_statement(Statement statement, Env * env) {
    Object * obj = NULL;
    LetStatement * let = NULL;
    ReturnStatement * ret = NULL;
    ExpressionStatement * est = NULL;

    if(strcmp(statement.type, LET) == 0) {
        let = (LetStatement *) statement.st;
        est = (ExpressionStatement *) let->value;
        return eval_let_statement(est, env, let->name.value);
    } else if(strcmp(statement.type, RETURN) == 0) {
        ret = (ReturnStatement *) statement.st;
        est = (ExpressionStatement *) ret->value;
        return eval_return_statement(est, env);
    } else if(strcmp(statement.type, EXPRESSION) == 0) {
        est = (ExpressionStatement *) statement.st;
        return eval_expression(est->expression_type, est->expression, env);
    }
}

Object * eval_statements(Statement * statements, int sc, Env * env) {
    int i;
    Object * obj = NULL;
    ErrorObject * err = NULL;
    ExpressionStatement * est = NULL;
    char * stt = NULL;

    for(i = 0; i < sc; i++) {
        obj = eval_statement(statements[i], env);
        stt = statements[i].type;
        print_object(obj);

        if(strcmp(obj->type, RETURN) == 0 || strcmp(obj->type, ERROR) == 0) {
            err = (ErrorObject *) obj->value;
            free(err->message);
            free(err);
            free(obj);
            return obj;
        }

        if(strcmp(stt, EXPRESSION) == 0) {
            est = (ExpressionStatement *) statements[i].st;

            if(strcmp(est->expression_type, IDENT) == 0 ||
                strcmp(obj->type, BOOLEAN) == 0 ||
                eval_is_bool(est->expression_type)) {
                continue;
            }

            if(free_eval_expression(est->expression_type, obj, env)) {
                free(obj);
            }
        }
    }

    return obj;
}
