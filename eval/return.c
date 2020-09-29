/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/return.c
 *
 */

Object * eval_return_statement(ExpressionStatement * est, Env * env) {
    Object * obj = malloc(sizeof(Object)), * err = NULL;
    ReturnValue * rv = malloc(sizeof(ReturnValue));

    obj->type = RETURN;
    obj->value = rv;

    rv->value = eval_expression(est->expression_type, est->expression, env);

    if(strcmp(est->expression_type, IDENT) == 0 &&
        strcmp(((Object *) rv->value)->type, INT) == 0) {

        rv->value = copy_integer_object(rv->value);
    }

    if(strcmp(est->expression_type, IDENT) == 0 &&
        strcmp(((Object *) rv->value)->type, STRING) == 0) {

        rv->value = copy_string_object(rv->value);
    }

    if(rv->value != NULL) {
        rv->type = ((Object *) rv->value)->type;
    }

    if(is_error(rv->value)) {
        err = rv->value;
        free(rv);
        free(obj);
        return err;
    }

    return obj;
}
