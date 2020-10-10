/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/string.c
 *
 */

Object * eval_string(StringLiteral * str, Env * env) {
    Object * obj = eval_new_string();
    StringObject * str_obj = obj->value;
    String * z = str->value;

    string_cat(str_obj->value, z->string, 0);

    obj->type = STRING;
    obj->value = str_obj;
    obj->ref = 0;

    return obj;
}
