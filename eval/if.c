/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/if.c
 *
 */

bool is_truthy(Object * obj) {
    if(obj == null_obj) {
        return false;
    } else if(obj == true_bool) {
        return true;
    } else if(obj == false_bool) {
        return false;
    } else if(strcmp(obj->type, INT) == 0  &&
        ((IntegerObject *) obj->value)->value == 0) {

        return false;
    } else {
        return true;
    }
}

Object * eval_if_expression(IfExpression * iex, Env * env) {
    Object * cond = eval_expression(iex->condition_type, iex->condition, env);
    Object * ret = NULL;
    BlockStatement * bs = NULL;

    if(is_error(cond)) {
        return cond;
    }

    if(is_truthy(cond)) {
        bs = (BlockStatement *) iex->consequence;
        ret = eval_statements(bs->statements, bs->sc, env);
    } else if(iex->alternative != NULL) {
        bs = (BlockStatement *) iex->alternative;
        ret = eval_statements(bs->statements, bs->sc, env);
    } else {
        ret = null_obj;
    }

    if(bs->sc == 0) {
        return null_obj;
    }

    if(!is_bool_or_ident(iex->condition_type)) {
        free_eval_expression(cond->type, cond, env, true);
    }

    return ret;
}
