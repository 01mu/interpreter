/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/infix/index.c
 *
 */

Object * get_index(IndexExpression * ie, Env * env) {
    ExpressionStatement * est = (ExpressionStatement *) ie->index;

    return eval_expression(ie->index_expression_type, est->expression, env);
}

Object * get_array(Object * obj, Object * idx) {
    Object * ret = NULL;
    char * stridx = NULL;
    int intidx;

    if(strcmp(idx->type, INT) == 0) {
        intidx = ((IntegerObject *) idx->value)->value;
    } else {
        stridx = ((StringObject *) idx->value)->value->string;
    }

    if(strcmp(obj->type, ARRAY) == 0) {
        ret = eval_array_get_value(obj, intidx);
    } else {
        ret = eval_hash_get_value(obj, stridx);
    }

    return ret;
}

void eval_index_hold_arr(Array * hold) {
    int i;
    Object * obj = NULL;

    for(i = 0; i < hold->size; i++) {
        obj = hold->array[i];
        free_eval_expression(obj->type, obj, NULL, 0);
    }

    array_free(hold);
}

Object * eval_get_array_edit(InfixExpression * iex, Env * env) {
    Object * get = NULL;
    IndexExpression * exp = iex->left;
    Array * hold = array_new();
    int i;

    while(strcmp(exp->left_expression_type, ARRAYIDX) == 0) {
        array_insert(hold, get_index(exp, env));
        exp = exp->left;
    }

    array_insert(hold, get_index(exp, env));

    if(strcmp(exp->left_expression_type, IDENT) != 0) {
        eval_index_hold_arr(hold);
        return false_bool;
    }

    get = env_get(env, ((Identifier *) exp->left)->value);

    for(i = hold->size - 1; i >= 0; i--) {
        get = get_array(get, hold->array[i]);
    }

    eval_index_hold_arr(hold);

    return get;
}
