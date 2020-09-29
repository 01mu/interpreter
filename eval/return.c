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
    Object * e = eval_expression(est->expression_type, est->expression, env);

    obj->type = RETURN;
    obj->value = rv;
    rv->value = e;

    if(strcmp(est->expression_type, IDENT) == 0) {
        rv->value = copy_object(e);
    }

    if(is_error(e)) {
        err = e;
        free(rv);
        free(obj);
        return err;
    }

    return obj;
}
