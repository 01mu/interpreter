/*
 *
 * array
 * github.com/01mu/interpreter
 *
 * lib/array.c
 *
 */

Array * array_new() {
    Array * array = malloc(sizeof(Array));
    void ** a = malloc(sizeof(void *));

    array->size = 0;
    array->array = a;

    return array;
}

void array_insert(Array * array, void * data) {
    array->array = realloc(array->array, sizeof(void *) * (array->size + 1));
    array->array[array->size++] = data;
}

void array_free(Array * array) {
    int i;

    for(i = 0; i < array->size; i++) {
        free(array->array[i]);
    }

    free(array->array);
    free(array);
}
