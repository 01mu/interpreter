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
    StringObject * so = obj->value;
    String * z = str->value;

    string_cat(so->value, z->string, 0);

    return obj;
}
