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

Object * get_built_in_fn(char * type, Object * obj, Object ** args, int argc) {
    if(strcmp(type, "len") == 0) {
        return bi_len(obj, args, argc);
    } else if(strcmp(type, "find") == 0) {
        return bi_find(obj, args, argc);
    } else if(strcmp(type, "str") == 0) {
        return bi_str(obj, args, argc);
    } else if(strcmp(type, "push") == 0) {
        return bi_push(obj, args, argc);
    }

    return null_obj;
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

Object * apply_function(Object * obj, Object ** args, int argc) {
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
        return get_built_in_fn(((BuiltIn *) obj->value)->fn, obj, args, argc);
    } else if(strcmp(FUNCTION, obj->type) == 0) {
        func = (Function *) obj->value;
        bs = func->body;
        out = extend_function_env(func, args);
        evaluated = eval_statements(bs->statements, bs->sc, out);

        eval_env_store_add(out);

        /*HashMap * store = out->store;
        SortedList * current = NULL;

        for(int i = 0; i < store->size; i++) {
            current = store->array[i];

            while(current != NULL) {
                obj = (Object *) current->data;

                if(strcmp(obj->type, ARRAY) == 0) {
                    current->data = malloc(1);
                }

                current = current->next;
            }
        }*/

        Statement a = bs->statements[bs->sc - 1];
        char * t = a.type;

        if(strcmp(evaluated->type, RETURN) != 0) {
            if(strcmp(t, EXPRESSION) == 0) {
                ExpressionStatement * e = a.st;
                char * tt = e->expression_type;

                if(!is_bool_or_ident(tt) && strcmp(tt, IF) != 0) {
                    free_eval_expression(tt, evaluated, NULL, 0);

                    if(strcmp(tt, ARRAYIDX) == 0) {
                        free(evaluated);
                    }
                }
            }

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

        if(strcmp(est->expression_type, IDENT) == 0 && !is_error(eval)) {
            //if(strcmp(eval->type, ARRAY) != 0) {
                eval = copy_object(eval);
            //}
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
