/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/free.c
 *
 */

void free_eval_hash_map(Object * obj) {
    int i;
    HashObject * ho = obj->value;
    HashMap * hm = ho->pairs;
    SortedList * current = NULL;
    HashPair * hp = NULL;
    Object * obj_free = NULL;

    for(i = 0; i < hm->size; i++) {
        current = hm->array[i];

        while(current != NULL) {
            hp = current->data;
            obj_free = hp->value;
            free_eval_expression(obj_free->type, obj_free, NULL, 1);
            current = current->next;
        }
    }

    hash_map_free(hm);
    free(ho);
}

void free_eval_array(Object * obj) {
    int i;
    ArrayObject * ao = obj->value;
    Array * arr = ao->elements;
    Object * obj_free = NULL;

    for(i = 0; i < arr->size; i++) {
        obj_free = arr->array[i];

        if(is_bool_or_ident(obj_free->type)) {
            arr->array[i] = malloc(1);
        } else {
            free_eval_expression(obj_free->type, obj_free, NULL, 0);
        }
    }

    array_free(arr);
    free(obj->value);
}

void free_eval_expression(char * ext, Object * obj, Env * env, bool free_obj) {
    if(strcmp(ext, INT) == 0) {
        free(obj->value);
    } else if(strcmp(ext, PREFIX) == 0 && strcmp(obj->type, INT) == 0) {
        free(obj->value);
    } else if(strcmp(ext, INFIX) == 0) {
        free_eval_expression(obj->type, obj, NULL, 0);
    } else if(strcmp(ext, CALL) == 0) {
        free_eval_expression(obj->type, obj, NULL, 0);
    } else if(strcmp(ext, HASHMAP) == 0) {
        free_eval_hash_map(obj);
    } else if(strcmp(ext, ARRAY) == 0) {
        free_eval_array(obj);
    } else if(strcmp(ext, FUNCTION) == 0) {
        free(obj->value);
    } else if(strcmp(ext, ARRAYIDX) == 0) {
        free_eval_expression(obj->type, obj, NULL, 0);
    } else if(strcmp(ext, BUILTIN) == 0) {
        free(obj->value);
    } else if(strcmp(ext, STRING) == 0) {
        string_free(((StringObject *) obj->value)->value);
        free(obj->value);
    } else {
        return;
    }

    if(free_obj) {
        free(obj);
    }
}
