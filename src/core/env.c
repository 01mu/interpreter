/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/env.c
 *
 */

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
    SortedList * sl = (SortedList *) hash_map_find(env->store, name);
    Object * obj;

    if(sl == NULL) {
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

void env_test() {
    const char * input = "let a = 4 + 2; a; let z = a * 10; z;";

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Env * env = new_env();

    eval_statements(program->statements, program->sc, env);
}

void env_free(Env * env) {
    int i;

    Object * obj = NULL;
    HashMap * store = env->store, * track = hash_map_new(50);
    SortedList * current = NULL;

    for(i = 0; i < store->size; i++) {
        if(store->array[i] != NULL) {
            current = store->array[i];

            while(current != NULL) {
                obj = (Object *) current->data;

                if(strcmp(obj->type, BOOLEAN) == 0) {
                    current->data = malloc(1);
                } else {
                    free_eval_expression(obj->type, obj, env, false);
                }

                current = current->next;
            }
        }
    }

    hash_map_free(env->store, track);
    hash_map_free(track, NULL);

    free(env);
}

void env_display(Env * env) {
    String * envs = hash_map_print(env->store);
    printf("Extended:\n%s", envs->string);
    string_free(envs);
}
