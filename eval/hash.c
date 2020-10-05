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
    HashMap * hm = hl->pairs;
    SortedList * current = NULL;

    Object * obj = malloc(sizeof(Object));
    HashObject * ho = malloc(sizeof(HashObject));

    obj->type = HASHMAP;
    obj->value = ho;
    ho->pairs = hash_map_new(26);

    for(i = 0; i < hm->size; i++) {
        current = hm->array[i];

        while(current != NULL) {
            ExpressionStatement * d = current->data;
            char * key = current->key;
            HashPair * hp = malloc(sizeof(HashPair));

            Object * g = eval_expression(d->expression_type, d->expression, env);

            char * keyc = malloc(strlen(key) + 1);
            strcpy(keyc, key);

            hp->key = keyc;
            hp->value = g;

            if(is_error(g)) {
                return g;
            }

            hash_map_insert(ho->pairs, keyc, hp);

            current = current->next;
        }
    }

    return obj;
}
