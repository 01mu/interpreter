/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/array.c
 *
 */

Object * eval_array_literal(ArrayLiteral * al, Env * env) {
    int i;
    Object * obj = malloc(sizeof(Object));
    ArrayObject * ao = malloc(sizeof(ArrayObject));
    Array * a = al->elements, * b = array_new();
    ExpressionStatement ** el = (ExpressionStatement **) a->array;
    Object ** ee = eval_expressions(el, a->size, env);

    obj->type = ARRAY;
    obj->value = ao;
    ao->elements = b;

    for(i = 0; i < a->size; i++) {
        if(strcmp(el[i]->expression_type, IDENT) == 0) {
            array_insert(b, copy_object(ee[i]));
        } else {
            array_insert(b, ee[i]);
        }
    }

    free(ee);

    return obj;
}
