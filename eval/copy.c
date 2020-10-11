/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/copy.c
 *
 */

Object * copy_object(Object * obj);

Object * copy_integer_object(Object * fr) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = ((IntegerObject *) fr->value)->value;
    obj->type = INT;
    obj->value = iobj;
    obj->ref = 0;

    return obj;
}

Object * copy_string_object(Object * obj) {
    Object * new = malloc(sizeof(Object));
    StringObject * nv = malloc(sizeof(StringObject));
    String * old_string = ((StringObject *) obj->value)->value;
    String * new_string = string_new();

    string_cat(new_string, old_string->string, 0);

    new->type = STRING;
    new->value = nv;
    new->ref = 0;
    nv->value = new_string;

    return new;
}

Object * copy_array_object(Object * obj) {
    int i;
    Object * new = malloc(sizeof(Object));
    ArrayObject * new_ao = malloc(sizeof(ArrayObject)), * old_ao = obj->value;
    Array * new_arr = array_new(), * old_arr = old_ao->elements;

    new->type = ARRAY;
    new->value = new_ao;
    new->ref = 0;
    new_ao->elements = new_arr;

    for(i = 0; i < old_arr->size; i++) {
        array_insert(new_arr, copy_object(old_arr->array[i]));
    }

    return new;
}

Object * copy_function_object(Object * obj) {
    Object * new = malloc(sizeof(Object));
    Function * old = obj->value;
    Function * new_func = malloc(sizeof(Function));

    new_func->pc = old->pc;
    new_func->env = old->env;
    new_func->body = old->body;
    new_func->parameters = old->parameters;

    new->type = FUNCTION;
    new->value = new_func;
    new->ref = 0;

    return new;
}

HashPair * copy_hash_pair(HashPair * hp, char * key_cpy) {
    HashPair * new = malloc(sizeof(HashPair));

    new->key = key_cpy;
    new->value = copy_object(hp->value);

    return new;
}

Object * copy_hash_literal(Object * obj) {
    int i;
    SortedList * current = NULL;
    char * key_cpy = NULL;

    Object * new = eval_new_hashmap();
    HashObject * new_ho = new->value;
    HashMap * new_hm = new_ho->pairs;

    HashObject * ho = obj->value;
    HashMap * hm = ho->pairs;
    HashPair * hp = NULL;

    for(i = 0; i < hm->size; i++) {
        current = hm->array[i];

        while(current != NULL) {
            hp = current->data;

            key_cpy = malloc(strlen(current->key) + 1);
            strcpy(key_cpy, current->key);

            hash_map_insert(new_hm, key_cpy, copy_hash_pair(hp, key_cpy));

            current = current->next;
        }
    }

    return new;
}

Object * copy_object(Object * obj) {
    Object * ret = NULL;

    if(strcmp(obj->type, STRING) == 0) {
        ret = copy_string_object(obj);
    } else if(strcmp(obj->type, INT) == 0) {
        ret = copy_integer_object(obj);
    } else if(strcmp(obj->type, ARRAY) == 0) {
        ret = copy_array_object(obj);
    } else if(strcmp(obj->type, FUNCTION) == 0) {
        ret = copy_function_object(obj);
    } else if(strcmp(obj->type, HASHMAP) == 0) {
        ret = copy_hash_literal(obj);
        return ret;
    } else {
        return obj;
    }

    return ret;
}
