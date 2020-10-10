/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/let.c
 *
 */

Object * eval_update_value(Object * obj, Object * new, Env * env, char * n) {
    free_eval_expression(obj->type, obj, env, 0);

    if(obj->ref == 0) {
        if(!is_bool_or_ident(new->type)) {
            obj->value = new->value;
            obj->type = new->type;
            free(new);
        } else {
            obj = new;
        }
    } else {
        obj->value = new->value;
        free(new);
    }

    return obj;
}

void eval_write_env(char * name, Object * obj, Env * env) {
    char * ident_name = malloc(strlen(name) + 1);

    strcpy(ident_name, name);
    env_set(env, ident_name, obj);
}

Object * eval_let_statement(ExpressionStatement * est, Env * env, char * name) {
    char * ext = est->expression_type;
    Object * obj = eval_expression(ext, est->expression, env);
    Object * get = env_get(env, name), * prev = NULL;

    if(is_error(obj)) {
        return obj;
    }

    if(obj == null_obj) {
        eval_write_env(name, obj, env);
        return null_obj;
    }

    if(obj != get && strcmp(ext, IDENT) == 0) {
        prev = copy_object(obj);

        if(env_get(env, name)) {
            free_eval_expression(get->type, get, env, 0);
            get->value = prev->value;
            get->type = prev->type;
            free(prev);
        } else {
            get = prev;
            eval_write_env(name, get, env);
        }

        return get;
    }

    if(get != NULL) {
        if(obj == get) {
            return obj;
        }

        return eval_update_value(get, obj, env, name);
    }

    eval_write_env(name, obj, env);
}
