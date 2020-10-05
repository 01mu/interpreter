/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/idxexp.c
 *
 */

Object * eval_hash_index_expression(Object * left, Object * index, char * lex) {
    HashObject * ho = left->value;
    HashMap * values = ho->pairs;
    String * s = ((StringObject *) index->value)->value;

    SortedList * sl = hash_map_find(values, s->string);
    HashPair * d = sl->data;
    Object * dd = d->value;

    Object * ob = copy_object(dd);

    //string_free(s);
    //free(index->value);
    free_eval_expression(index->type, index, NULL, 1);
    //free_eval_expression(dd->type, dd, NULL, 1);
    //free_eval_expression(ARRAY, left, NULL, 1);

    //if(strcmp(

    return ob;
}

Object * eval_array_index_expression(Object * left, int v, bool del) {
    ArrayObject * ao = left->value;
    Array * la = ao->elements;
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
    bool del = 0;
    ExpressionStatement * iex = ie->index;

    index = eval_expression(ie->index_expression_type, iex->expression, env);

    if(strcmp(ie->left_expression_type, CALL) == 0 ||
        strcmp(ie->left_expression_type, ARRAY) == 0 ||
        strcmp(ie->left_expression_type, ARRAYIDX) == 0) {

        del = 1;
    }

    if(strcmp(left->type, HASHMAP) == 0) {
        return eval_hash_index_expression(left, index, ie->left_expression_type);
    }

    if(is_error(left)) {
        return left;
    }

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
