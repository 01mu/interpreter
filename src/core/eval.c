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

Object * false_bool, * true_bool;
Object * null_obj;

int inspect_integer_object(IntegerObject * iobj) {
    return iobj->value;
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
    Object * obj;
    BooleanObject * bool_type;

    if(false_bool == NULL) {
        false_bool = malloc(sizeof(Object));
        false_bool->type = FALSE;
        bool_type = malloc(sizeof(BooleanObject));
        bool_type->value = false;
        obj->value = bool_type;
    } else if(true_bool == NULL) {
        true_bool = malloc(sizeof(Object));
        true_bool->type = TRUE;
        bool_type = malloc(sizeof(BooleanObject));
        bool_type->value = true;
        obj->value = bool_type;
    }

    obj = false_bool;

    if(b->value) {
        obj = true_bool;
    }

    return obj;
}

Object * eval_bang_operator_expression(Object * right) {
    if(right == true_bool) {
        return true_bool;
    } else if(right == false_bool) {
        return false_bool;
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
    } else if(strcmp(operator, "!") == 0 ) {
         return eval_minus_prefix_operator_expression(right);
    } else {
        return null_obj;
    }
}

Object * eval_expression(char * ext, ExpressionStatement * est) {
    if(strcmp(ext, INT) == 0) {
        return eval_integer((IntegerLiteral *) est->expression);
    } else if(strcmp(ext, IDENT) == 0) {

    } else if(strcmp(ext, PREFIX) == 0) {
        PrefixExpression* pex = (PrefixExpression *) est->expression;
        Object * right = eval_expression(pex->expression_type, pex->right);
        return eval_prefix_expression(pex->operator, right);
    } else if(strcmp(ext, INFIX) == 0) {

    } else if(strcmp(ext, TRUE) == 0 || strcmp(ext, FALSE) == 0) {
        return eval_bool((Boolean *) est->expression);
    } else if(strcmp(ext, IF) == 0) {

    } else if(strcmp(ext, FUNCTION) == 0) {

    } else if(strcmp(ext, CALL) == 0) {

    }
}

Object * eval_statement(Statement statement) {
    char * type = statement.type, * ext;
    ExpressionStatement * est;

    if(strcmp(type, "EXPRESSION") == 0) {
        est = (ExpressionStatement *) statement.st;
        ext = est->expression_type;
        eval_expression(ext,  est);
    }
}

Object * eval_statements(Statement * statements, int sc) {
    int i;

    for(i = 0; i < sc; i++) {
        eval_statement(statements[i]);
    }
}
