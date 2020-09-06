/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/eval.c
 *
 */

typedef struct {
    char * type;
    void * value;
} Object;

typedef struct {
    int value;
} IntegerObject;

typedef struct {
    bool value;
} BooleanObject;

typedef struct {

} NullObject;

Object * false_bool = NULL, * true_bool = NULL;
Object * null_obj;

int inspect_integer_object(IntegerObject * iobj) {
    return iobj->value;
}

char * inspect_null_object(NullObject * nobj) {
    return "null";
}

bool inspect_boolean_object(BooleanObject * bobj) {
    return bobj->value;
}

Object * eval_integer(IntegerLiteral * il) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = il->value;

    obj->type = INT;
    obj->value = iobj;

    return obj;
}

Object * eval_bool(Boolean * b) {
    if(b->value) {
        return true_bool;
    }

    return false_bool;
}

Object * eval_bang_operator_expression(Object * right) {
    if(right == true_bool) {
        return false_bool;
    } else if(right == false_bool) {
        return true_bool;
    } else if(right == null_obj) {
        return true_bool;
    } else {
        return false_bool;
    }
}

Object * eval_minus_prefix_operator_expression(Object * right) {
    IntegerObject * r;

    if(strcmp(right->type, INT) != 0) {
        return null_obj;
    }

    r = right->value;
    r->value = r->value * -1;

    return right;
}

Object * eval_prefix_expression(char * operator, Object * right) {
    if(strcmp(operator, "!") == 0 ) {
        return eval_bang_operator_expression(right);
    } else if(strcmp(operator, "-") == 0 ) {
         return eval_minus_prefix_operator_expression(right);
    } else {
        return null_obj;
    }
}

Object * eval_integer_infix_expression(char * op, Object * l, Object * r) {
    IntegerObject * left = (IntegerObject *) l->value;
    IntegerObject * right = (IntegerObject *) r->value;
    Object * obj = malloc(sizeof(Object));
    IntegerObject * ret = malloc(sizeof(IntegerObject));

    obj->type = INT;
    obj->value = ret;

    if(strcmp(op, "+") == 0) {
        ret->value = left->value + right->value;
    } else if(strcmp(op, "-") == 0) {
        ret->value = left->value - right->value;
    } else if(strcmp(op, "*") == 0) {
        ret->value = left->value * right->value;
    } else if(strcmp(op, "/") == 0) {
        ret->value = left->value / right->value;
    }

    return obj;
}

Object * eval_infix_expression(char * op, Object * left, Object * right) {
    if(strcmp(left->type, INT) == 0 && strcmp(right->type, INT) == 0) {
        return eval_integer_infix_expression(op, left, right);
    } else {
        return null_obj;
    }
}

Object * eval_expression(char * ext, void * est) {
    char * pre_ex;
    PrefixExpression * pex, * in;
    Object * left;
    Object * right;

    if(strcmp(ext, INT) == 0) {
        return eval_integer((IntegerLiteral *) est);
    } else if(strcmp(ext, IDENT) == 0) {

    } else if(strcmp(ext, PREFIX) == 0) {
        pex = (PrefixExpression *) est;
        pre_ex = pex->expression_type;

        if(strcmp(pre_ex, TRUE) == 0 || strcmp(pre_ex, FALSE) == 0) {
            right = eval_bool((Boolean *) pex->right);
        } else if(strcmp(pre_ex, INT) == 0) {
            right = eval_integer((IntegerLiteral *) pex->right);
        } else if(strcmp(pre_ex, PREFIX) == 0) {
            in = (PrefixExpression *) pex->right;
            right = eval_prefix_expression(in->operator, in->right);
        }

        return eval_prefix_expression(pex->operator, right);
    } else if(strcmp(ext, INFIX) == 0) {
        InfixExpression * iex = (InfixExpression *) est;

        left = eval_expression(iex->left_expression_type, iex->left);
        right = eval_expression(iex->right_expression_type, iex->right);

        return eval_infix_expression(iex->operator, left, right);
    } else if(strcmp(ext, TRUE) == 0 || strcmp(ext, FALSE) == 0) {
        return eval_bool((Boolean *) est);
    } else if(strcmp(ext, IF) == 0) {

    } else if(strcmp(ext, FUNCTION) == 0) {

    } else if(strcmp(ext, CALL) == 0) {

    }
}

Object * eval_statement(Statement statement) {
    char * type = statement.type;
    ExpressionStatement * est;

    if(strcmp(type, "EXPRESSION") == 0) {
        est = (ExpressionStatement *) statement.st;
        return eval_expression(est->expression_type, est->expression);
    }
}

Object * eval_statements(Statement * statements, int sc) {
    int i;

    for(i = 0; i < sc; i++) {
        eval_statement(statements[i]);
    }
}
