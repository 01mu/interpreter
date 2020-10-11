/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/call.c
 *
 */

void eval_env_store_add(Env * env) {
    env_store->store = realloc(env_store->store, sizeof(FunctionLiteral *) *
        (env_store->count + 1));
    env_store->store[env_store->count++] = env;
}

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

void env_tag_reference(Env * env) {
    int i;
    HashMap * store = env->store;
    SortedList * current = NULL;
    Object * obj = NULL;

    for(i = 0; i < store->size; i++) {
        current = store->array[i];

        while(current != NULL) {
            obj = current->data;

            if(obj->ref == 1) {
                current->data = malloc(sizeof(Object));
                ((Object *) current->data)->type = "REFERENCE";
            }

            current = current->next;
        }
    }
}

void args_tag_reference(Object ** args, ExpressionStatement ** ea, int argc) {
    int i;
    char * obj = NULL, * ext = NULL;

    for(i = 0; i < argc; i++) {
        obj = args[i]->type;
        ext = ea[0]->expression_type;

        if((strcmp(obj, HASHMAP) == 0 || strcmp(obj, ARRAY) == 0)
            && strcmp(ext, IDENT) == 0) {

            args[i]->ref = 1;
        }
    }
}

Object * apply_function(Object * obj, Object ** args, int c) {
    char * m = NULL, * type = NULL;
    Object * evaluated = NULL;
    Function * func = NULL;
    BlockStatement * bs = NULL;
    Env * out = NULL;

    if(strcmp(FUNCTION, obj->type) != 0 && strcmp(BUILTIN, obj->type) != 0) {
        m = malloc(strlen(obj->type) + 23);
        sprintf(m, "Not a function: %s", obj->type);
        return new_error(m);
    } else if(strcmp(BUILTIN, obj->type) == 0) {
        return get_built_in_fn(((BuiltIn *) obj->value)->fn, obj, args, c);
    } else if(strcmp(FUNCTION, obj->type) == 0) {
        func = (Function *) obj->value;
        bs = func->body;
        out = extend_function_env(func, args);
        evaluated = eval_statements(bs->statements, bs->sc, out);

        eval_env_store_add(out);
        env_tag_reference(out);

        if(strcmp(evaluated->type, RETURN) != 0) {
            return null_obj;
        }

        return unwrap_return_value(evaluated);
    }
}

Object ** eval_expressions(ExpressionStatement ** args, int c, Env * env) {
    int i;
    Object ** objects = malloc(sizeof(Object *)), ** err = NULL;
    Object * eval = NULL, * ret = NULL;
    ExpressionStatement * est = NULL;

    for(i = 0; i < c; i++) {
        objects = realloc(objects, sizeof(Object *) * (i + 1));
        est = (ExpressionStatement *) args[i];
        eval = eval_expression(est->expression_type, est->expression, env);

        if(strcmp(est->expression_type, IDENT) == 0 && !is_error(eval) &&
            !is_array(eval) && !eval_is_hashmap(eval)) {

            eval = copy_object(eval);
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
    args_tag_reference(args, ce->arguments, ce->ac);

    if(args != NULL && is_error(args[0])) {
        err = args[0]->value;
        ret = new_error(err->message);
        free(err);
        free(args[0]);
        free(args);
        return args[0];
    }

    ret = apply_function(obj, args, ce->ac);

    if(strcmp(ce->function_type, IDENT) != 0) {
        free(obj->value);
        free(obj);
    }

    if(args != NULL) {
        free(args);
    }

    return ret;
}
