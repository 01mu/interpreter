/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * lib/sortedlist.c
 *
 */

SortedList * sorted_list_new(void * data, char * key) {
    SortedList * sl = malloc(sizeof(SortedList));

    sl->data = data;
    sl->key = key;
    sl->next = NULL;

    return sl;
}

SortedList * sorted_list_ins(SortedList ** r, void * d, char * key) {
    SortedList * new_sl = sorted_list_new(d, key);
    SortedList * prev = NULL, * current = * r;

    if(r == NULL || key == NULL) {
        return NULL;
    }

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

    return new_sl;
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
            * r = sorted_list_new(NULL, NULL);
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

String * sorted_list_print(SortedList * sl) {
    SortedList * current = sl;
    String * str = string_new();
    char * a = NULL;

    while(current != NULL && current->key != NULL) {
        string_cat(str, current->key, false);
        string_cat(str, ": ", false);
        a = malloc(sizeof(char) * 20);
        sprintf(a, " %p", current->data);
        string_cat(str, a, true);
        string_cat(str, ", ", false);
        current = current->next;
    }

    return str;
}

void sorted_list_free(SortedList * sl) {
    SortedList * current = sl, * temp = NULL;
    char * addr = NULL;

    while(current != NULL) {
        temp = current->next;
        free(current->key);
        free(current->data);
        free(current);
        current = temp;
    }
}
