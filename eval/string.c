/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/string.c
 *
 */

Object * eval_string(StringLiteral * str, Env * env) {
    Object * obj = malloc(sizeof(Object));
    StringObject * str_obj = malloc(sizeof(StringObject));
    String * z = str->value;

    str_obj->value = string_new();

    string_cat(str_obj->value, z->string, 0);

    obj->type = STRING;
    obj->value = str_obj;

    return obj;
}
