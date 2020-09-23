/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/eval.c
 *
 */

void env_store_add(Env * env) {
    env_store->store = realloc(env_store->store, sizeof(FunctionLiteral *) *
        (env_store->count + 1));
    env_store->store[env_store->count++] = env;
}

void eval_write_env(char * name, Object * obj, Env * env) {
    char * ident_name = malloc(strlen(name) + 1);

    ident_name[0] = '\0';
    strcpy(ident_name, name);
    env_set(env, ident_name, obj);
}

bool is_error(Object * obj) {
    if(obj != NULL) {
        return strcmp(obj->type, ERROR) == 0;
    }
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

void init_null(Object ** n) {
    (*n) = malloc(sizeof(Object));
    (*n)->type = "NULL";
    (*n)->value = malloc(sizeof(NullObject));
}

void free_stat(Object * b) {
    free(b->value);
    free(b);
}

String * print_object(Object * obj) {
    String * s = string_new();
    char * c = malloc(sizeof(char) + 40);

    c[0] = '\0';

    if(strcmp(obj->type, "INT") == 0) {
        sprintf(c, "%i ", ((IntegerObject *) obj->value)->value);
    } else if(strcmp(obj->type, BOOLEAN) == 0) {
        sprintf(c, "%i ", ((BooleanObject *) obj->value)->value);
    } else if(strcmp(obj->type, "NULL") == 0) {
        sprintf(c, "NULL ");
    } else if(strcmp(obj->type, "ERROR") == 0) {
        sprintf(c, "%s ", ((ErrorObject *) obj->value)->message);
    } else if(strcmp(obj->type, "STRING") == 0) {
        sprintf(c, "%s ", ((StringObject *) obj->value)->value->string);
    }

    string_cat(s, c, true);

    return s;
}

Object * copy_string_object(Object * obj) {
    Object * new = malloc(sizeof(Object));
    StringObject * nv = malloc(sizeof(StringObject));
    String * old_string = ((StringObject *) obj->value)->value;
    String * new_string = string_new();

    string_cat(new_string, old_string->string, 0);

    new->type = STRING;
    new->value = nv;
    nv->value = new_string;

    return new;
}

Object * copy_integer_object(Object * fr) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = ((IntegerObject *) fr->value)->value;
    obj->type = INT;
    obj->value = iobj;

    return obj;
}

bool is_bool_or_ident(char * t) {
    bool eval_is_bool = strcmp(t, TRUE) == 0 || strcmp(t, FALSE) == 0 ||
        strcmp(t, BOOLEAN) == 0;

    if(strcmp(t, IDENT) == 0 || eval_is_bool || strcmp(t, "NULL") == 0) {
        return true;
    }

    return false;
}

void free_eval_expression(char * ext, Object * obj, Env * env, bool free_obj) {
    if(strcmp(ext, INT) == 0) {
        free(obj->value);
    } else if(strcmp(ext, PREFIX) == 0 && strcmp(obj->type, INT) == 0) {
        free(obj->value);
    } else if(strcmp(ext, INFIX) == 0) {
        free(obj->value);
    } else if(strcmp(ext, CALL) == 0 && strcmp(obj->type, INT) == 0) {
        free(obj->value);
    } else if(strcmp(ext, FUNCTION) == 0) {
        free(obj->value);
    } else if(strcmp(ext, STRING) == 0) {
        string_free(((StringObject *) obj->value)->value);
        free(obj->value);
    } else {
        return;
    }

    if(free_obj) {
        free(obj);
    }
}

Object * update_eval_value(Object ** obj, Object * new, Env * env, char * n) {
    char * old_t = (*obj)->type, * new_t = new->type;
    Object * ret = NULL;
    IntegerObject * iob_old = NULL, * iob_new = NULL;
    bool same_type = strcmp(old_t, new_t) == 0;

    if(same_type && strcmp(new_t, INT) == 0) {
        iob_old = (IntegerObject *) (*obj)->value;
        iob_new = (IntegerObject *) new->value;
        iob_old->value = iob_new->value;
        free_eval_expression(new_t, new, env, true);
        ret = * obj;
    } else {
        free_eval_expression(old_t, * obj, env, true);
        env_set(env, n, new);
        ret = * obj;
    }

    return ret;
}

/*
 * =============================================================================
 * call expression
 * =============================================================================
 */

Object * unwrap_return_value(Object * obj) {
    ReturnValue * rv = NULL;
    Object * ret_obj = NULL;

    if(strcmp(obj->type, RETURN) == 0) {
        ret_obj = ((ReturnValue *) obj->value)->value;
        free(obj->value);
        free(obj);
        return ret_obj;
    }

    return obj;
}

Env * extend_function_env(Function * func, Object ** args) {
    int i;
    Env * env = env_new_enclosed(func->env);
    Identifier * ident = NULL;
    char * new = NULL;

    for(i = 0; i < func->pc; i++) {
        ident = (Identifier *) func->parameters[i];
        new = malloc(strlen(ident->value) + 1);
        new[0] = '\0';
        strcpy(new, ident->value);
        env_set(env, new, args[i]);
    }

    return env;
}

Object * apply_function(Object * obj, Object ** args) {
    char * m = NULL;
    Object * evaluated = NULL;
    Function * func = (Function *) obj->value;
    BlockStatement * bs = func->body;
    Env * out = extend_function_env(func, args);

    env_store_add(out);

    if(strcmp(FUNCTION, obj->type) != 0) {
        m = malloc(strlen(obj->type) + 23);
        sprintf(m, "Not a function: %s\n", obj->type);
        return new_error(m);
    }

    evaluated = eval_statements(bs->statements, bs->sc, out);

    if(strcmp(evaluated->type, RETURN) != 0) {
        return null_obj;
    }

    return unwrap_return_value(evaluated);
}

Object ** eval_expressions(ExpressionStatement ** args, int c, Env * env) {
    int i;
    Object ** objects = malloc(sizeof(Object *)), ** err = NULL;
    Object * eval = NULL, * ret;
    ExpressionStatement * est = NULL;

    for(i = 0; i < c; i++) {
        objects = realloc(objects, sizeof(Object *) * (i + 1));
        est = (ExpressionStatement *) args[i];
        eval = eval_expression(est->expression_type, est->expression, env);

        if(strcmp(est->expression_type, IDENT) == 0 && !is_error(eval)) {
            if(strcmp(eval->type, INT) == 0) {
                eval = copy_integer_object(eval);
            } else if(strcmp(eval->type, STRING) == 0) {
                Object * copy = copy_string_object(eval);
                eval = copy;
            }
        }

        if(is_error(eval)) {
            free(objects);
            err = malloc(sizeof(Object *));
            err[0] = eval;
            return err;
        } else {
            objects[i] = eval;
        }
    }

    if(c == 0) {
        free(objects);
        objects = NULL;
    }

    return objects;
}

Object * eval_call_expression(CallExpression * ce, Env * env) {
    Object * obj = eval_expression(ce->function_type, ce->function, env);
    Object * ret = NULL;
    Object ** args = NULL;
    ErrorObject * err = NULL;

    if(is_error(obj)) {
        return obj;
    }

    args = eval_expressions(ce->arguments, ce->ac, env);

    if(args != NULL && is_error(args[0])) {
        err = args[0]->value;
        ret = new_error(err->message);
        free(err);
        free(args[0]);
        free(args);
        return ret;
    }

    ret = apply_function(obj, args);

    if(strcmp(ce->function_type, IDENT) != 0) {
        free(obj->value);
        free(obj);
    }

    if(args != NULL) {
        free(args);
    }

    return ret;
}

/*
 * =============================================================================
 * if expression
 * =============================================================================
 */

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

Object * eval_if_expression(IfExpression * iex, Env * env) {
    Object * cond = eval_expression(iex->condition_type, iex->condition, env);
    Object * ret = NULL;
    BlockStatement * bs = NULL;

    if(is_error(cond)) {
        return cond;
    }

    if(is_truthy(cond)) {
        bs = (BlockStatement *) iex->consequence;
        ret = eval_statements(bs->statements, bs->sc, env);
    } else if(iex->alternative != NULL) {
        bs = (BlockStatement *) iex->alternative;
        ret = eval_statements(bs->statements, bs->sc, env);
    } else {
        ret = null_obj;
    }

    if(bs->sc == 0) {
        return null_obj;
    }

    if(!is_bool_or_ident(iex->condition_type)) {
        free_eval_expression(cond->type, cond, env, true);
    }

    return ret;
}

/*
 * =============================================================================
 * infix expression
 * =============================================================================
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

void eval_free_infix(char * lt, Object * lo, char * rt, Object * ro) {
    if(lt != NULL && !is_bool_or_ident(lt)) {
        free_eval_expression(lt, lo, NULL, true);
    }

    if(rt != NULL && !is_bool_or_ident(rt)) {
        free_eval_expression(rt, ro, NULL, true);
    }
}

Object * eval_infix_expression(InfixExpression * iex, Env * env) {
    Object * l, * r, * ret;
    char * op = iex->operator, * lext = iex->left_expression_type,
        * rext = iex->right_expression_type, * m = NULL;

    l = eval_expression(lext, iex->left, env);

    if(is_error(l)) {
        eval_free_infix(lext, l, NULL, NULL);
        return l;
    }

    r = eval_expression(rext, iex->right, env);

    if(is_error(r)) {
        eval_free_infix(lext, l, rext, r);
        return r;
    }

    if(strcmp(l->type, INT) == 0 && strcmp(r->type, INT) == 0) {
        ret = eval_integer_infix_exp(op, l, r, env);
        eval_free_infix(lext, l, rext, r);
    } else if(strcmp(l->type, STRING) == 0 && strcmp(r->type, STRING) == 0) {
        ret = eval_string_infix_exp(op, l, r, env);

        if(strcmp(lext, INFIX) == 0) {
            string_free(((StringObject *) l->value)->value);
        }

        eval_free_infix(lext, l, rext, r);
    } else if(strcmp(l->type, r->type) != 0) {
        m = malloc(strlen(l->type) + strlen(r->type) + strlen(op) + 19);
        sprintf(m, "Type mismatch: %s %s %s", l->type, op, r->type);
        eval_free_infix(lext, l, rext, r);
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

/*
 * =============================================================================
 * prefix expression
 * =============================================================================
 */

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

Object * eval_prefix_expression(PrefixExpression * pex, Env * env) {
    Object * right = eval_expression(pex->expression_type, pex->right, env);
    Object * ret = NULL;
    char * operator = pex->operator, * m;

    if(strcmp(pex->expression_type, IF) == 0) {
        return null_obj;
    }

    if(is_error(right)) {
        return right;
    }

    if(strcmp(operator, "!") == 0 ) {
        ret = eval_bang_operator_expression(right, env);

        if(!is_bool_or_ident(pex->expression_type)) {
            free_eval_expression(pex->expression_type, right, env, true);
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

/*
 * =============================================================================
 *
 * =============================================================================
 */

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

Object * eval_bool(bool b, Env * env) {
    if(b) {
        return true_bool;
    }

    return false_bool;
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

Object * eval_string(StringLiteral * str, Env * env) {
    Object * obj = malloc(sizeof(Object));
    StringObject * str_obj = malloc(sizeof(StringObject));
    String * z = str->value;

    str_obj->value = string_new();

    string_cat(str_obj->value, z->string, 0);

    obj->type = STRING;
    obj->value = str_obj;

    return obj;
}

Object * eval_integer(IntegerLiteral * il, Env * env) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = il->value;
    obj->type = INT;
    obj->value = iobj;

    return obj;
}

Object * eval_expression(char * ext, void * est, Env * env) {
    Object * res = NULL;

    if(strcmp(ext, INT) == 0) {
        res = eval_integer((IntegerLiteral *) est, env);
    } else if(strcmp(ext, STRING) == 0) {
        res = eval_string((StringLiteral *) est, env);
    } else if(strcmp(ext, IDENT) == 0) {
        res = eval_identifier((Identifier *) est, env);
    } else if(strcmp(ext, PREFIX) == 0) {
        res = eval_prefix_expression((PrefixExpression *) est, env);
    } else if(strcmp(ext, INFIX) == 0) {
        res = eval_infix_expression((InfixExpression *) est, env);
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
    Object * obj = malloc(sizeof(Object)), * err = NULL;
    ReturnValue * rv = malloc(sizeof(ReturnValue));

    obj->type = RETURN;
    obj->value = rv;

    rv->value = eval_expression(est->expression_type, est->expression, env);

    if(strcmp(est->expression_type, IDENT) == 0 &&
        strcmp(((Object *) rv->value)->type, INT) == 0) {

        rv->value = copy_integer_object(rv->value);
    }

    if(strcmp(est->expression_type, IDENT) == 0 &&
        strcmp(((Object *) rv->value)->type, STRING) == 0) {

        rv->value = copy_string_object(rv->value);
    }

    if(rv->value != NULL) {
        rv->type = ((Object *) rv->value)->type;
    }

    if(is_error(rv->value)) {
        err = rv->value;
        free(rv);
        free(obj);
        return err;
    }

    return obj;
}

Object * eval_let_statement(ExpressionStatement * est, Env * env, char * name) {
    char * ext = est->expression_type;
    Object * obj = eval_expression(ext, est->expression, env);
    Object * get = env_get(env, name);
    Object ** hmd = NULL;
    SortedList * hm_store = NULL;

    if(strcmp(ext, IF) == 0) {
        return null_obj;
    }

    if(is_error(obj)) {
        return null_obj;
    }

    if(obj != get && strcmp(ext, IDENT) == 0) {
        get = env_get(env, ((Identifier *) est->expression)->value);

        if(strcmp(obj->type, STRING) == 0) {
            get = copy_string_object(get);
        } else if(strcmp(obj->type, INT) == 0) {
            get = copy_integer_object(get);
        }

        eval_write_env(name, get, env);
        return null_obj;
    }

    if(get != NULL) {
        hm_store = hash_map_find(env->store, name);

        if(hm_store != NULL) {
            hmd = (Object **) (&(hash_map_find(env->store, name)->data));
        } else {
            if(strcmp(get->type, FUNCTION) == 0) {
                eval_write_env(name, obj, env);
                return null_obj;
            }

            hmd = (Object **) (&(hash_map_find(env->outer->store, name)->data));
        }

        if(obj == get) {
            return null_obj;
        }

        update_eval_value(hmd, obj, env, name);
        return null_obj;
    }

    eval_write_env(name, obj, env);
}

Object * eval_statement(Statement statement, Env * env) {
    Object * obj = NULL;
    LetStatement * let = NULL;
    ReturnStatement * ret = NULL;
    ExpressionStatement * est = NULL;

    if(strcmp(statement.type, LET) == 0) {
        let = (LetStatement *) statement.st;
        est = (ExpressionStatement *) let->value;
        obj = eval_let_statement(est, env, let->name.value);
    } else if(strcmp(statement.type, RETURN) == 0) {
        ret = (ReturnStatement *) statement.st;
        est = (ExpressionStatement *) ret->value;
        obj = eval_return_statement(est, env);
    } else if(strcmp(statement.type, EXPRESSION) == 0) {
        est = (ExpressionStatement *) statement.st;
        obj = eval_expression(est->expression_type, est->expression, env);
    }

    return obj;
}

bool eval_free_error(Object * obj, Env * env) {
    ErrorObject * err = NULL;
    bool is_error = strcmp(obj->type, ERROR) == 0;

    if(is_error) {
        err = (ErrorObject *) obj->value;
        free(err->message);
        free(err);
        free(obj);
        return true;
    }

    return false;
}

bool eval_free_return(Object * obj, Env * env, char * ety) {
    ReturnValue * rv = NULL;
    bool is_return = strcmp(obj->type, RETURN) == 0;

    if(env == env_store->store[0] && is_return) {
        rv = (ReturnValue *) obj->value;
        free_eval_expression(ety, rv->value, env, true);
        free(obj->value);
        free(obj);
        return true;
    }

    return false;
}

Object * eval_statements(Statement * statements, int sc, Env * env) {
    int i;
    Object * obj = NULL;
    ExpressionStatement * est = NULL;
    char * stt = NULL, * ety = NULL;
    Env * henv = env_store->store[0];
    String * s = NULL;

    for(i = 0; i < sc; i++) {
        obj = eval_statement(statements[i], env);
        stt = statements[i].type;
        est = (ExpressionStatement *) statements[i].st;
        ety = est->expression_type;

        if(strcmp(ety, IF) != 0 && obj != null_obj) {
            s = print_object(obj);

            if(is_repl_test_string) {
                //printf("%s", s->string);
                string_append(repl_test_string, s);
            } else {
                printf("%s", s->string);

                if(strcmp(s->string, "") != 0) {
                   printf("\n");
                }

                string_free(s);
            }
        } else {
            continue;
        }

        //printf("[%p: %i] %s %s %s\n", env, i, stt, ety, obj->type);

        if(eval_free_error(obj, env) || eval_free_return(obj, env, ety)) {
            return null_obj;
        } else if(env != henv && strcmp(obj->type, RETURN) == 0) {
            return obj;
        } else if(env != henv && !is_bool_or_ident(ety) && sc == 1) {
            free_eval_expression(obj->type, obj, env, true);
            return null_obj;
        } else if((strcmp(ety, INFIX) == 0 ||
            strcmp(ety, CALL) == 0) && strcmp(obj->type, STRING) == 0) {

            free_eval_expression(obj->type, obj, env, true);
            return null_obj;
        } else if(env != henv && !is_bool_or_ident(ety) && i != sc - 1) {
            free_eval_expression(obj->type, obj, env, true);
        } else if(env == henv && !is_bool_or_ident(obj->type)) {
            free_eval_expression(ety, obj, env, true);
        }
    }

    return obj;
}
