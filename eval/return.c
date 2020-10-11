/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/return.c
 *
 */

Object * eval_return_statement(ExpressionStatement * est, Env * env) {
    Object * obj = eval_new_return(), * err = NULL;
    ReturnValue * rv = obj->value;
    Object * e = eval_expression(est->expression_type, est->expression, env);

    rv->value = e;

    if(strcmp(est->expression_type, IDENT) == 0 || e->ref == 1) {
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
