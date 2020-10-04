/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/new.c
 *
 */

Object * eval_new_array() {
    Object * new = malloc(sizeof(Object));
    ArrayObject * ao = malloc(sizeof(ArrayObject));

    new->type = ARRAY;
    new->value = ao;
    ao->elements = array_new();

    return new;
}

Object * eval_new_string() {
    Object * obj = malloc(sizeof(Object));
    StringObject * sobj = malloc(sizeof(StringObject));

    obj->type = STRING;
    obj->value = sobj;
    sobj->value = string_new();

    return obj;
}

Object * new_error(char * msg) {
    Object * obj = malloc(sizeof(Object));
    ErrorObject * err = malloc(sizeof(ErrorObject));

    obj->type = ERROR;
    obj->value = err;
    err->message = malloc(strlen(msg) + 1);
    err->message[0] = '\0';

    strcat(err->message, msg);
    free(msg);

    return obj;
}
