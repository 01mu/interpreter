/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/get.c
 *
 */

Object * eval_array_get_value(Object * obj, int k) {
    ArrayObject * ao = obj->value;
    Array * arr = ao->elements;

    return arr->array[k];
}

Object * eval_hash_get_value(Object * obj, char * k) {
    HashObject * ho = obj->value;
    HashMap * hm = ho->pairs;
    SortedList * sl = hash_map_find(hm, k);
    HashPair * hp = sl->data;

    return hp->value;
}
