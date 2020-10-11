/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/new.c
 *
 */

Object * eval_new_return() {
    Object * obj = malloc(sizeof(Object));
    ReturnValue * rv = malloc(sizeof(ReturnValue));

    obj->type = RETURN;
    obj->value = rv;
    obj->ref = 0;

    return obj;
}

Object * eval_new_array() {
    Object * new = malloc(sizeof(Object));
    ArrayObject * ao = malloc(sizeof(ArrayObject));

    new->type = ARRAY;
    new->value = ao;
    new->ref = 0;
    ao->elements = array_new();

    return new;
}

Object * eval_new_string() {
    Object * obj = malloc(sizeof(Object));
    StringObject * sobj = malloc(sizeof(StringObject));

    obj->type = STRING;
    obj->value = sobj;
    obj->ref = 0;
    sobj->value = string_new();

    return obj;
}

Object * new_error(char * msg) {
    Object * obj = malloc(sizeof(Object));
    ErrorObject * err = malloc(sizeof(ErrorObject));

    obj->type = ERROR;
    obj->value = err;
    obj->ref = 0;
    err->message = malloc(strlen(msg) + 1);
    err->message[0] = '\0';

    strcat(err->message, msg);
    free(msg);

    return obj;
}

Object * eval_new_hashmap() {
    Object * obj = malloc(sizeof(Object));
    HashObject * ho = malloc(sizeof(HashObject));

    obj->type = HASHMAP;
    obj->value = ho;
    obj->ref = 0;

    ho->pairs = hash_map_new(26);

    return obj;
}
