/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * lib/hashmap.c
 *
 */

HashMap * hash_map_new(int size) {
    int i;
    HashMap * map = malloc(sizeof(HashMap));

    map->size = size;
    map->array = malloc(sizeof(SortedList *) * size);

    for(i = 0; i < map->size; i++) {
        map->array[i] = NULL;
    }

    return map;
}

void * hash_map_insert(HashMap * hm, char * key, void * data) {
    unsigned long idx;
    SortedList * find = NULL, * insert = NULL;

    if(hm == NULL || key == NULL) {
        return NULL;
    }

    idx = hash_map_hash(hm, key);

    if(hm->array[idx] == NULL) {
        hm->array[idx] = sorted_list_new(data, key);
    }

    find = hash_map_find(hm, key);

    if(find == NULL) {
        insert = hm->array[idx];
        sorted_list_ins(&insert, data, key);
    } else {
        find->data = data;
    }

    return data;
}

void hash_map_remove(HashMap * hm, char * key) {
    unsigned long idx = hash_map_hash(hm, key);
    SortedList * sl = hm->array[idx];

    if(sl != NULL) {
        sorted_list_remove(&hm->array[idx], key);
    }
}

SortedList * hash_map_find(HashMap * hm, char * key) {
    unsigned long idx = hash_map_hash(hm, key);
    SortedList * sl = hm->array[idx];

    if(sl == NULL) {
        return NULL;
    }

    return sorted_list_find(sl, key);
}


unsigned long hash_map_hash(HashMap * hm, char * key) {
    unsigned long i;
    int hash_val = 5381;

    while(i = *key++) {
        hash_val = ((hash_val << 5) + hash_val) + i;
    }

    return hash_val % hm->size;
}

String * hash_map_print(HashMap * hm) {
    int i;
    String * str = string_new();
    char * a = NULL;

    for(i = 0; i < hm->size; i++) {
        if(hm->array[i] != NULL) {
            a = malloc(sizeof(char) * 10);
            sprintf(a, "[%i] ", i);
            string_cat(str, a, true);
            string_append(str, sorted_list_print(hm->array[i]));
            string_cat(str, "\n", false);
        }
    }

    return str;
}

void hash_map_free(HashMap * hm) {
    int i;

    for(i = 0; i < hm->size; i++) {
        if(hm->array[i] != NULL) {
            sorted_list_free(hm->array[i]);
        }
    }

    free(hm->array);
    free(hm);
}

void hash_map_test() {

}
