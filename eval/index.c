/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/index.c
 *
 */

void eval_index_free_idx(char * iext, Object * idx) {
    if(!is_bool_or_ident(iext)) {
        free_eval_expression(idx->type, idx, NULL, 1);
    }
}

void eval_index_free_left(char * lext, Object * left, Object * idx) {
    if(lext == CALL || lext == HASHMAP || lext == ARRAY || lext == ARRAYIDX) {
        free_eval_expression(left->type, left, NULL, 1);
    }
}

Object * eval_idx_err(char * t, Object * l, Object * i, char * lx, char * ix) {
    char * m = NULL;

    if(strcmp(i->type, t) != 0) {
        m = malloc(strlen(l->type) + strlen(i->type) + 23);
        sprintf(m, "Invalid types: %s %s", l->type, i->type);
        eval_index_free_idx(ix, i);
        eval_index_free_left(lx, l, i);
        return new_error(m);
    }

    return NULL;
}

Object * eval_index_hash_exp(Object * left, Object * idx, char * iext) {
    String * s = ((StringObject *) idx->value)->value;

    return copy_object(eval_hash_get_value(left, s->string));
}

Object * eval_index_array_exp(Object * left, Object * idx, char * iext) {
    IntegerObject * io = idx->value;
    int v = io->value;

    return copy_object(eval_array_get_value(left, v));
}

Object * eval_index_expression(IndexExpression * ie, Env * env) {
    char * lext = ie->left_expression_type, * iext = ie->index_expression_type;
    ExpressionStatement * iex = ie->index;
    Object * left = eval_expression(ie->left_expression_type, ie->left, env);
    Object * idx = eval_expression(iext, iex->expression, env);
    Object * ret = NULL, * err = NULL;

    if(is_error(left)) {
        return left;
    }

    if(is_error(idx)) {
        eval_index_free_left(lext, left, idx);
        return idx;
    }

    if(strcmp(left->type, HASHMAP) == 0) {
        err = eval_idx_err(STRING, left, idx, lext, iext);

        if(err != NULL) {
            return err;
        }

        ret = eval_index_hash_exp(left, idx, iext);
    } else {
        err = eval_idx_err(INT, left, idx, lext, iext);

        if(err != NULL) {
            return err;
        }

        ret = eval_index_array_exp(left, idx, iext);
    }

    eval_index_free_idx(iext, idx);
    eval_index_free_left(lext, left, idx);

    return ret;
}
