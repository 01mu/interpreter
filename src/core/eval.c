/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/eval.c
 *
 */

#define ERROR "ERROR"

typedef struct {
    char * message;
} ErrorObject;

typedef struct {
    char * type;
    void * value;
} ReturnValue;

typedef struct {

} NullObject;

typedef struct {
    int value;
} IntegerObject;

typedef struct {
    bool value;
} BooleanObject;

typedef struct {
    char * type;
    void * value;
} Object;

Object * eval_expression(char * ext, void * est);
Object * eval_statement(Statement statement);
Object * eval_statements(Statement * statements, int sc);

Object * false_bool = NULL, * true_bool = NULL;
Object * null_obj = NULL;

Object * new_error(char * msg) {
    Object * obj = malloc(sizeof(Object));
    ErrorObject * err = malloc(sizeof(ErrorObject));

    obj->type = ERROR;
    obj->value = err;

    err->message = malloc(strlen(msg) + 1);
    err->message[0] = '\0';

    strcat(err->message, msg);

    free(msg);

    return obj;
}

void init_bool(Object ** b, char * type, bool lit) {
    (*b) = malloc(sizeof(Object));
    (*b)->type = type;
    (*b)->value = malloc(sizeof(BooleanObject));
    ((BooleanObject *) (*b)->value)->value = lit;
}

char * inspect_error_object(ErrorObject * eobj) {
    return eobj->message;
}

char * inspect_null_object(NullObject * nobj) {
    return "null";
}

int inspect_integer_object(IntegerObject * iobj) {
    return iobj->value;
}

bool inspect_boolean_object(BooleanObject * bobj) {
    return bobj->value;
}

void print_object(Object * obj) {
    if(strcmp(obj->type, "INT") == 0) {
        printf("%i\n", inspect_integer_object((IntegerObject *) obj->value));
    } else if(strcmp(obj->type, "TRUE") == 0 ||
        strcmp(obj->type, "FALSE") == 0) {
        printf("%i\n", inspect_boolean_object((BooleanObject *) obj->value));
    }
}

Object * eval_integer(IntegerLiteral * il) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = il->value;

    obj->type = INT;
    obj->value = iobj;

    return obj;
}

Object * eval_bool(bool b) {
    if(b) {
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
    IntegerObject * r = NULL;

    if(strcmp(right->type, INT) != 0) {
        char * msg = malloc(strlen(right->type) + 20);
        sprintf(msg, "Unknown operator: -%s\n", right->type);
        return new_error(msg);
    }

    r = right->value;
    r->value = r->value * -1;

    return right;
}

Object * eval_prefix_expression(char * operator, Object * right) {
    if(strcmp(operator, "!") == 0 ) {
        return eval_bang_operator_expression(right);
    } else if(strcmp(operator, "-") == 0) {
         return eval_minus_prefix_operator_expression(right);
    } else {
        char * msg = malloc(strlen(operator) + strlen(right->type) + 20);
        sprintf(msg, "Unknown operator: %s%s\n", operator, right->type);
        return new_error(msg);
    }
}

Object * eval_integer_infix_expression(char * op, Object * l, Object * r) {
    Object * obj = NULL;
    IntegerObject * int_obj = NULL;
    BooleanObject * bool_obj = NULL;

    int left = ((IntegerObject *) l->value)->value;
    int right = ((IntegerObject *) r->value)->value;

    int arith = 0;

    if(strcmp(op, "+") == 0) {
        arith = left + right;
    } else if(strcmp(op, "-") == 0) {
        arith = left - right;
    } else if(strcmp(op, "*") == 0) {
        arith = left * right;
    } else if(strcmp(op, "/") == 0) {
        arith = left / right;
    } else if(strcmp(op, "<") == 0) {
        obj = eval_bool(left < right);
    } else if(strcmp(op, ">") == 0) {
        obj = eval_bool(left > right);
    } else if(strcmp(op, "==") == 0) {
        obj = eval_bool(left == right);
    } else if(strcmp(op, "!=") == 0) {
        obj = eval_bool(left != right);
    } else {
        char * msg = malloc(strlen(op) + strlen(r->type) + strlen(l->type) + 20);
        sprintf(msg, "Unknown operator: %s%s%s\n", l->type, op, r->type);
        return new_error(msg);
    }

    if(strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 ||
        strcmp(op, "/") == 0) {

        obj = malloc(sizeof(Object));
        int_obj = malloc(sizeof(IntegerObject));
        int_obj->value = arith;
        obj->type = INT;
        obj->value = int_obj;
    }

    return obj;
}

Object * eval_infix_expression(char * op, Object * left, Object * right) {
    if(strcmp(left->type, INT) == 0 && strcmp(right->type, INT) == 0) {
        return eval_integer_infix_expression(op, left, right);
    } else if(strcmp(op, "==") == 0) {
        return eval_bool(left == right);
    } else if(strcmp(op, "!=") == 0) {
        return eval_bool(left != right);
    } else if(strcmp(left->type, right->type) != 0) {
        char * msg = malloc(strlen(left->type) +
            strlen(right->type) + strlen(op) + 19);
        sprintf(msg, "Type mismatch: %s %s %s\n", left->type,
            op, right->type);
        return new_error(msg);
    } else {
        char * msg = malloc(strlen(left->type) +
            strlen(right->type) + strlen(op) + 23);
        sprintf(msg, "Unknown operation: %s %s %s\n", left->type,
            op, right->type);
        return new_error(msg);
    }
}

bool is_truthy(Object * obj) {
    if(obj == null_obj) {
        return false;
    } else if(obj == true_bool) {
        return true;
    } else if(obj == false_bool) {
        return false;
    } else {
        return true;
    }
}

Object * eval_if_expression(IfExpression * iex) {
    Object * condition = eval_expression(iex->condition_type, iex->condition);
    BlockStatement * bs = NULL;

    if(is_truthy(condition)) {
        bs = (BlockStatement *) iex->consequence;
        return eval_statements(bs->statements, bs->sc);
    } else if(iex->alternative != NULL) {
        bs = (BlockStatement *) iex->alternative;
        return eval_statements(bs->statements, bs->sc);
    } else {
        return null_obj;
    }
}

Object * eval_expression(char * ext, void * est) {
    Object * res = NULL, * left = NULL, * right = NULL;
    PrefixExpression * pex, * in;
    InfixExpression * iex;

    if(strcmp(ext, INT) == 0) {
        res = eval_integer((IntegerLiteral *) est);
    } else if(strcmp(ext, IDENT) == 0) {

    } else if(strcmp(ext, PREFIX) == 0) {
        pex = (PrefixExpression *) est;
        right = eval_expression(pex->expression_type, pex->right);
        res = eval_prefix_expression(pex->operator, right);
    } else if(strcmp(ext, INFIX) == 0) {
        iex = (InfixExpression *) est;
        left = eval_expression(iex->left_expression_type, iex->left);
        right = eval_expression(iex->right_expression_type, iex->right);
        res = eval_infix_expression(iex->operator, left, right);
    } else if(strcmp(ext, TRUE) == 0 || strcmp(ext, FALSE) == 0) {
        res = eval_bool(((Boolean *) est)->value);
    } else if(strcmp(ext, IF) == 0) {
        res = eval_if_expression((IfExpression *) est);
    } else if(strcmp(ext, FUNCTION) == 0) {

    } else if(strcmp(ext, CALL) == 0) {

    }

    return res;
}

Object * eval_statement(Statement statement) {
    Object * obj = NULL;
    ReturnStatement * ret = NULL;
    ReturnValue * ret_val = NULL;
    ExpressionStatement * est = NULL;

    if(strcmp(statement.type, "RETURN") == 0) {
        ret = (ReturnStatement *) statement.st;
        est = ret->value;
        obj = malloc(sizeof(Object));
        ret_val = malloc(sizeof(ReturnValue));
        obj->type = RETURN;
        obj->value = ret_val;
        ret_val->value = eval_expression(est->expression_type, est->expression);
        ret_val->type = "";

        if(ret_val->value != NULL) {
            ret_val->type = ((Object *) ret_val->value  )->type;
        }

        return obj;
    } else if(strcmp(statement.type, "EXPRESSION") == 0) {
        est = (ExpressionStatement *) statement.st;
        return eval_expression(est->expression_type, est->expression);
    }
}

Object * eval_statements(Statement * statements, int sc) {
    int i;
    Object * obj = NULL;

    for(i = 0; i < sc; i++) {
        obj = eval_statement(statements[i]);

        if(strcmp(obj->type, RETURN) == 0 || strcmp(obj->type, ERROR) == 0) {
            return obj;
        }
    }

    return obj;
}
