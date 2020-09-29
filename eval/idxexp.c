/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/idxexp.c
 *
 */

Object * eval_array_index_expression(Object * left, int v, bool del) {
    ArrayObject * ao = left->value;
    Array * la = ao->elements;
    char * type = ((Object *) la->array[v])->type;
    Object * a = copy_object(la->array[v]);

    if(del) {
        free_eval_expression(ARRAY, left, NULL, 1);
    }

    return a;
}

Object * eval_index_expression(IndexExpression * ie, Env * env) {
    Object * left = eval_expression(ie->left_expression_type, ie->left, env);
    Object * index = NULL;
    char * m = NULL;

    ExpressionStatement * iex = ie->index;

    bool del = 0;

    if(strcmp(ie->left_expression_type, ARRAY) == 0 ||
        strcmp(ie->left_expression_type, ARRAYIDX) == 0) {

        del = 1;
    }

    if(is_error(left)) {
        return left;
    }

    index = eval_expression(ie->index_expression_type, iex->expression, env);

    if(is_error(index)) {
        return index;
    }

    if(strcmp(index->type, INT) != 0) {
        m = malloc(strlen(left->type) + strlen(index->type) + 23);
        sprintf(m, "Invalid types: %s %s", left->type, index->type);
        return new_error(m);
    }

    IntegerObject * io = index->value;
    int v = io->value;

    if(!is_bool_or_ident(ie->index_expression_type)) {
        free_eval_expression(INT, index, env, 1);
    }

    return eval_array_index_expression(left, v, del);
}
