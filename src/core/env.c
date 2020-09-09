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
    SortedList * data = ((SortedList *) hash_map_find(env->store, name))->data;
    Object * obj  = (Object *) data;

    if(obj == NULL && env->outer != NULL) {
        obj = env_get(env->outer, name);
    }

    return obj;
}

Object * env_set(Env * env, char * name, Object * data) {
    return (Object *) hash_map_insert(env->store, name, "object", data);
}

void env_test() {
    const char * input = "let a = 4 + 2; a; let z = a * 10; z;";

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Env * env = new_env();

    eval_statements(program->statements, program->sc, env);
}
