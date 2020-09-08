/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * env.c
 *
 */

Env * new_env() {
    Env * env = malloc(sizeof(Env));
    env->store = hash_map_new(150);
    return env;
}

Object * get(Env * env, char * name) {
    return (Object *) ((SortedList *) hash_map_find(env->store, name))->data;
}

Object * set(Env * env, char * name, Object * data) {
    return (Object *) hash_map_insert(env->store, name, "object", data);
}

void test_env() {
    const char * input = "";

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Env * env = new_env();
}
