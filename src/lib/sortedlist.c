/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * sortedlist.c
 *
 */

struct SortedList {
    struct SortedList * next;
    char * data_type;
    void * data;
    char * key;
};

typedef struct SortedList SortedList;

SortedList * sorted_list_new(char * data_type, void * data, char * key);
void sorted_list_insert(SortedList ** r, char * dt, void * d, char * key);
bool sorted_list_remove(SortedList ** r, char * key);
SortedList * sorted_list_find(SortedList * r, char * key);
char * sorted_list_print(SortedList * sl);
void sorted_list_free(SortedList * sl);

SortedList * sorted_list_new(char * data_type, void * data, char * key) {
    SortedList * sl = malloc(sizeof(SortedList));

    sl->data_type = data_type;
    sl->data = data;
    sl->key = key;
    sl->next = NULL;

    return sl;
}

void sorted_list_insert(SortedList ** r, char * dt, void * d, char * key) {
    SortedList * new_sl = sorted_list_new(dt, d, key);
    SortedList * prev = NULL, * current = * r;

    while(current != NULL && strcmp(new_sl->key, current->key) > 0) {
        prev = current;
        current = current->next;
    }

    if(prev == NULL) {
        if((* r)->key == NULL) {
            free(* r);
        }

        * r = new_sl;
    } else {
        prev->next = new_sl;
    }

    new_sl->next = current;
}

bool sorted_list_remove(SortedList ** r, char * key) {
    SortedList * prev = NULL, * current = * r;

    if(sorted_list_find(* r, key) == NULL) {
        return false;
    }

    while(current != NULL && (strcmp(key, current->key) != 0)) {
        prev = current;
        current = current->next;
    }

    if(prev == NULL) {
        * r = (* r)->next;

        free(current->data);
        free(current);

        if(* r == NULL) {
            * r = sorted_list_new(NULL, NULL, NULL);
        }
    } else {
        prev->next = current->next;

        free(current->data);
        free(current);
    }

    return true;
}

SortedList * sorted_list_find(SortedList * r, char * key) {
    SortedList * current = r;

    while(current != NULL && (strcmp(current->key, key) <= 0)) {
        if(strcmp(current->key, key) == 0) {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

char * sorted_list_print(SortedList * sl) {
    SortedList * current = sl, * temp;
    char * key = current->key, * final = malloc(sizeof(char));
    size_t sl_sz = 0;

    final[0] = '\0';

    while(key != NULL && current != NULL) {
        key = current->key;
        sl_sz += strlen(key) + 2;
        final = realloc(final, sizeof(char) * sl_sz);
        strcat(final, key);
        strcat(final, " ");
        current = current->next;
    }

    return final;
}

void sorted_list_free(SortedList * sl) {
    SortedList * current = sl, * temp;

    while(current != NULL) {
        temp = current->next;
        free(current->data);
        free(current);
        current = temp;
    }
}
