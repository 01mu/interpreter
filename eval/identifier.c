/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/identifier.c
 *
 */

Object * new_builtin(char * fn) {
    Object * obj = malloc(sizeof(Object));
    BuiltIn * bi = malloc(sizeof(BuiltIn));

    bi->fn = fn;
    obj->type = BUILTIN;
    obj->value = bi;
    obj->ref = 0;

    return obj;
}

Object * eval_identifier(Identifier * ident, Env * env) {
    Object * get = NULL;
    char * msg = NULL;

    if(strcmp(ident->value, "NULL") == 0) {
        return null_obj;
    }

    get = env_get(env, ident->value);

    if(hash_map_find(fn_map, ident->value)) {
        return new_builtin(ident->value);
    }

    if(get == NULL) {
        msg = malloc(strlen(ident->value) + 28);
        sprintf(msg, "Identifier not found: %s", ident->value);
        return new_error(msg);
    }

    return get;
}
