/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * lib/hashmap.c
 *
 */

HashMap * hash_map_new(int size);
void * hash_map_insert(HashMap * hm, char * key, char * dt, void * data);
void hash_map_remove(HashMap * hm, char * key);
SortedList * hash_map_find(HashMap * hm, char * key);
int hash_map_hash(HashMap * hm, char * key);
String * hash_map_print(HashMap * hm);
void hash_map_free(HashMap * hm);
void hash_map_test();

HashMap * hash_map_new(int size) {
    int i;
    HashMap * map = malloc(sizeof(HashMap));

    map->size = size;
    map->array = malloc(sizeof(SortedList *));

    for(i = 0; i < map->size; i++) {
        map->array = realloc(map->array, sizeof(SortedList *) * (i + 1));
        map->array[i] = NULL;
    }

    return map;
}

void * hash_map_insert(HashMap * hm, char * key, char * dt, void * data) {
    int idx;

    if(hm == NULL || key == NULL) {
        return NULL;
    }

    idx = hash_map_hash(hm, key);

    if(hm->array[idx] == NULL) {
        hm->array[idx] = sorted_list_new(dt, data, key);
    }

    SortedList * find = hash_map_find(hm, key), * insert;

    if(find == NULL) {
        insert = hm->array[idx];
        sorted_list_ins(&insert, dt, data, key);
    } else {
        find->data_type = dt;
        find->data = data;
    }

    return data;
}

void hash_map_remove(HashMap * hm, char * key) {
    int idx = hash_map_hash(hm, key);
    SortedList * sl = hm->array[idx];

    if(sl != NULL) {
        sorted_list_remove(&hm->array[idx], key);
    }
}

SortedList * hash_map_find(HashMap * hm, char * key) {
    int idx = hash_map_hash(hm, key);
    SortedList * sl = hm->array[idx];

    if(sl == NULL) {
        return NULL;
    }

    return sorted_list_find(sl, key);
}

int hash_map_hash(HashMap * hm, char * key) {
    int i, hash_val = 0;

    for(i = 0; i < strlen(key); i++) {
        hash_val = (hash_val * 27 + key[i]) % hm->size;
    }

    return hash_val;
}

String * hash_map_print(HashMap * hm) {
    int i;
    String * str = string_new(1);

    for(i = 0; i < hm->size; i++) {
        if(hm->array[i] != NULL) {
            string_cat(str, "[ ", false);
            string_append(str, sorted_list_print(hm->array[i]));
            string_cat(str, " ]\n", false);
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
    HashMap * hm = hash_map_new(50);
    String * hmp;
    void * a = malloc(1);

    hash_map_insert(hm, "f", NULL, NULL);
    hash_map_insert(hm, "f", NULL, NULL);
    hash_map_insert(hm, "a", NULL, NULL);
    hash_map_insert(hm, "h", NULL, NULL);
    hash_map_insert(hm, "z", "test", a);

    hmp = hash_map_print(hm);
    printf("%s", hmp->string);
    hash_map_free(hm);
    string_free(hmp);
}
