/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/env.c
 *
 */

Env * new_env();
Env * env_new_enclosed(Env * outer);
Object * env_get(Env * env, char * name);
Object * env_set(Env * env, char * name, Object * data);
void env_free(Env * env);
void env_display(Env * env);

Env * new_env() {
    Env * env = malloc(sizeof(Env));

    env->store = hash_map_new(150);
    env->outer = NULL;

    return env;
}

Env * env_new_enclosed(Env * outer) {
    Env * env = new_env();

    env->outer = outer;

    return env;
}

Object * env_get(Env * env, char * name) {
    SortedList * sl = (SortedList *) hash_map_find(env->store, name), * sl2;
    Object * obj = NULL;

    if(sl == NULL) {
        if(env->outer != NULL) {
            sl2 = (SortedList *) hash_map_find(env->outer->store, name);

            if(sl2 != NULL) {
                obj = (Object *) sl2->data;
            }
        }

        if(obj != NULL) {
            return obj;
        }

        return NULL;
    }

    obj = (Object *) sl->data;

    if(obj == NULL && env->outer != NULL) {
        obj = env_get(env->outer, name);
    }

    return obj;
}

Object * env_set(Env * env, char * name, Object * data) {
    return (Object *) hash_map_insert(env->store, name, NULL, data);
}

void env_free(Env * env) {
    int i;

    Object * obj = NULL;
    HashMap * store = env->store;
    SortedList * current = NULL;

    for(i = 0; i < store->size; i++) {
        if(store->array[i] != NULL) {
            current = store->array[i];

            while(current != NULL) {
                obj = (Object *) current->data;

                if(is_bool_or_ident(obj->type)) {
                    current->data = malloc(1);
                } else {
                    free_eval_expression(obj->type, obj, env, false);
                }

                current = current->next;
            }
        }
    }

    hash_map_free(env->store, NULL);
    free(env);
}

void env_display(Env * env) {
    String * envs = hash_map_print(env->store);

    printf("%s", envs->string);
    string_free(envs);
}
