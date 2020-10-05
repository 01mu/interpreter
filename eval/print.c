/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/print.c
 *
 */

char * print_obj_str(Object * obj, bool t);

void print_array(char ** c, Object * obj) {
    ArrayObject * ao = obj->value;
    Array * arr = ao->elements;
    String * s = string_new();

    string_cat(s, "[", 0);

    for(int i = 0; i < arr->size; i++) {
        string_cat(s, print_obj_str(arr->array[i], 0), true);

        if(i != arr->size - 1) {
            string_cat(s, ", ", 0);
        }
    }

    string_cat(s, "]", 0);

    sprintf(*c, "%s", s->string);
    string_free(s);
}

char * print_obj_str(Object * obj, bool t) {
    char * c = malloc(sizeof(char) + 40);

    c[0] = '\0';

    if(strcmp(obj->type, INT) == 0) {
        sprintf(c, "%i", ((IntegerObject *) obj->value)->value);
    } else if(strcmp(obj->type, BOOLEAN) == 0) {
        sprintf(c, "%i", ((BooleanObject *) obj->value)->value);
    } else if(strcmp(obj->type, NULL_) == 0) {
        sprintf(c, "NULL");
    } else if(strcmp(obj->type, FUNCTION) == 0) {
        sprintf(c, "(fn)");
    } else if(strcmp(obj->type, ERROR) == 0) {
        sprintf(c, "%s", ((ErrorObject *) obj->value)->message);
    } else if(strcmp(obj->type, STRING) == 0) {
        sprintf(c, "%s", ((StringObject *) obj->value)->value->string);
    } else if(strcmp(obj->type, ARRAY) == 0) {
        print_array(&c, obj);
    }

    if(t) {
        sprintf(c, "%s ", c);
    }

    return c;
}

String * print_object(Object * obj) {
    String * s = string_new();

    if(obj->type != RETURN && obj->type != FUNCTION && obj->type != HASHMAP) {
        string_cat(s, print_obj_str(obj, 1), true);
    }

    return s;
}
