/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * hashmap.c
 *
 */

typedef struct {
    int size;
    SortedList ** array;
} HashMap;

HashMap * hash_map_new(int size);
void hash_map_insert(HashMap * hm, char * key, char * data_type, void * data);
void hash_map_remove(HashMap * hm, char * key);
SortedList * hash_map_find(HashMap * hm, char * key);
int hash_map_hash(HashMap * hm, char * key);
char * hash_map_print(HashMap * hm);
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

void hash_map_insert(HashMap * hm, char * key, char * data_type, void * data) {
    int idx = hash_map_hash(hm, key);

    if(hm->array[idx] == NULL) {
        hm->array[idx] = sorted_list_new(data_type, data, key);
    }

    SortedList * find = hash_map_find(hm, key), * insert;

    if(find == NULL) {
        insert = hm->array[idx];
        sorted_list_insert(&insert, data_type, data, key);
    } else {
        find->data_type = data_type;
        find->data = data;
    }
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

char * hash_map_print(HashMap * hm) {
    int i;
    char * sl = NULL, * final = malloc(sizeof(char));
    size_t hm_sz = 0;

    final[0] = '\0';

    for(i = 0; i < hm->size; i++) {
        if(hm->array[i] != NULL) {
            sl = sorted_list_print(hm->array[i]);
            hm_sz += strlen(sl);
            final = realloc(final, sizeof(char) * hm_sz);
            strcat(final, sl);
            free(sl);
        }

        hm_sz += 2;
        final = realloc(final, sizeof(char) * hm_sz);
        strcat(final, "\n");
    }

    return final;
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
    char * hmp;

    hash_map_insert(hm, "f", NULL, NULL);
    hash_map_insert(hm, "f", NULL, NULL);
    hash_map_insert(hm, "a", NULL, NULL);
    hash_map_insert(hm, "h", NULL, NULL);
    hash_map_insert(hm, "z", NULL, NULL);

    hmp = hash_map_print(hm);

    printf("%s", hmp);
    hash_map_free(hm);
    free(hmp);
}
