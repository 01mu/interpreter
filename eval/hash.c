/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/hash.c
 *
 */

Object * eval_hash_literal(HashLiteral * hl, Env * env) {
    int i;
    char * key_cpy = NULL;
    HashMap * hm = hl->pairs;
    SortedList * current = NULL;

    Object * obj = eval_new_hashmap(), * set = NULL;
    HashObject * ho = obj->value;
    HashPair * hp = NULL;

    ExpressionStatement * es = NULL;

    for(i = 0; i < hm->size; i++) {
        current = hm->array[i];

        while(current != NULL) {
            es = current->data;
            hp = malloc(sizeof(HashPair));

            set = eval_expression(es->expression_type, es->expression, env);

            if(strcmp(es->expression_type, IDENT) == 0) {
                set = copy_object(set);
            }

            key_cpy = malloc(strlen(current->key) + 1);
            strcpy(key_cpy, current->key);

            hp->key = key_cpy;
            hp->value = set;

            if(is_error(set)) {
                free_eval_expression(obj->type, obj, env, 1);
                free(hp);
                free(key_cpy);
                return set;
            }

            hash_map_insert(ho->pairs, key_cpy, hp);
            current = current->next;
        }
    }

    return obj;
}
