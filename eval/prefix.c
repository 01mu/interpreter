/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/prefix.c
 *
 */

Object * eval_minus_prefix_operator_expression(Object * right, Env * env) {
    IntegerObject * r = NULL;
    char * m = NULL;

    if(strcmp(right->type, INT) != 0) {
        m = malloc(strlen(right->type) + 25);
        sprintf(m, "Unknown operator: -%s", right->type);
        return new_error(m);
    }

    r = right->value;
    r->value = r->value * -1;

    return right;
}

Object * eval_bang_operator_expression(Object * right, Env * env) {
    Object * ret = NULL;

    if(right == true_bool) {
        ret = false_bool;
    } else if(right == false_bool) {
        ret = true_bool;
    } else if(right == null_obj) {
        ret = true_bool;
    } else {
        ret = false_bool;
    }

    return ret;
}

Object * eval_prefix_expression(PrefixExpression * pex, Env * env) {
    Object * right = eval_expression(pex->expression_type, pex->right, env);
    Object * ret = NULL;
    char * operator = pex->operator, * m;

    if(strcmp(pex->expression_type, IF) == 0) {
        return null_obj;
    }

    if(is_error(right)) {
        return right;
    }

    if(strcmp(operator, "!") == 0 ) {
        ret = eval_bang_operator_expression(right, env);

        if(!is_bool_or_ident(pex->expression_type)) {
            free_eval_expression(pex->expression_type, right, env, true);
        }
    } else if(strcmp(operator, "-") == 0) {
        ret = eval_minus_prefix_operator_expression(right, env);
    } else {
        m = malloc(strlen(operator) + strlen(right->type) + 20);
        sprintf(m, "Unknown operator: %s%s", operator, right->type);
        ret = new_error(m);
    }

    return ret;
}
