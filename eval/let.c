/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/let.c
 *
 */

Object * eval_update_value(Object ** obj, Object * new, Env * env, char * n) {
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

void eval_write_env(char * name, Object * obj, Env * env) {
    char * ident_name = malloc(strlen(name) + 1);

    ident_name[0] = '\0';
    strcpy(ident_name, name);
    env_set(env, ident_name, obj);
}

Object * eval_let_statement(ExpressionStatement * est, Env * env, char * name) {
    char * ext = est->expression_type;
    Object * obj = eval_expression(ext, est->expression, env);
    Object * get = env_get(env, name);
    Object ** hmd = NULL;
    SortedList * hm_store = NULL;

    if(is_error(obj)) {
        return obj;
    }

    if(obj != get && strcmp(ext, IDENT) == 0) {
        get = env_get(env, ((Identifier *) est->expression)->value);
        get = copy_object(get);

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
                return obj;
            }

            hmd = (Object **) (&(hash_map_find(env->outer->store, name)->data));
        }

        if(obj == get) {
            return obj;
        }

        eval_update_value(hmd, obj, env, name);
        return * hmd;
    }

    eval_write_env(name, obj, env);
}
