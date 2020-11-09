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
    obj->ref = 0;

    return obj;
}

Object * new_int_from_val(int val) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = val;
    obj->type = INT;
    obj->value = iobj;
    obj->ref = 0;

    return obj;
}
