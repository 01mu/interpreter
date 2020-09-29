/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/integer.c
 *
 */

Object * eval_integer(IntegerLiteral * il, Env * env) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = il->value;
    obj->type = INT;
    obj->value = iobj;

    return obj;
}
