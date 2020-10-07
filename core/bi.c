/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/builtin.c
 *
 */

char * print_obj_str(Object * obj, bool t);
Object * copy_object(Object * obj);

Object * bi_int_obj(int i) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = i;
    obj->type = INT;
    obj->value = iobj;

    return obj;
}

void bi_free_args(Object * obj, Object ** args, int argc) {
    int i;

    for(i = 0; i < argc; i++) {
        free_eval_expression(args[i]->type, args[i], NULL, true);
    }

    free_eval_expression(obj->type, obj, NULL, true);
}

Object * bi_push(Object * obj, Object ** args, int argc) {
    char * m = NULL, * l = NULL;
    Object * new = NULL, * rt = NULL;

    if(argc != 2) {
        m = malloc(sizeof(char) * 40);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 2, argc);
        rt = new_error(m);
    } else if(args[0]->type != ARRAY) {
        m = malloc(sizeof(char) * 30);
        sprintf(m, "First argument not an array");
        rt = new_error(m);
    } else {
        ArrayObject * ao = args[0]->value;
        Array * arr = ao->elements;

        printf("%i\n", arr->size);
        array_insert(arr, copy_object(args[1]));
    }

    //bi_free_args(obj, args, argc);

    return null_obj;
}

Object * bi_str(Object * obj, Object ** args, int argc) {
    char * m = NULL, * l = NULL;
    Object * new = NULL, * rt = NULL;
    StringObject * nv = NULL;
    String * new_string = NULL;

    if(argc != 1) {
        m = malloc(sizeof(char) * 40);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 1, argc);
        rt = new_error(m);
    } else if(args[0]->type == STRING) {
        m = malloc(sizeof(char) * 30);
        sprintf(m, "Argument already a string");
        rt = new_error(m);
    } else {
        new = malloc(sizeof(Object));
        nv = malloc(sizeof(StringObject));
        new_string = string_new();

        new->type = STRING;
        new->value = nv;
        nv->value = new_string;

        string_cat(new_string, print_obj_str(args[0], 0), 1);

        rt = new;
    }

    bi_free_args(obj, args, argc);

    return rt;
}

Object * bi_find(Object * obj, Object ** args, int argc) {
    char * m = NULL, * p = NULL;
    StringObject * hay, * needle;
    String * hh = NULL, * nn = NULL;
    Object * ret = NULL;
    int f = -1;

    if(argc != 2) {
        m = malloc(sizeof(char) * 40);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 2, argc);
        ret = new_error(m);
    } else if(args[0]->type != STRING || args[1]->type != STRING) {
        m = malloc(sizeof(char) * 30);
        sprintf(m, "Argument(s) not a string");
        ret = new_error(m);
    } else {
        hay = args[0]->value;
        needle = args[1]->value;
        hh = hay->value;
        nn = needle->value;
        p = strstr(hh->string, nn->string);

        if(p) {
            f = p - hh->string;
        }

        ret = bi_int_obj(f);
    }

    bi_free_args(obj, args, argc);

    return ret;
}

Object * bi_len(Object * obj, Object ** args, int argc) {
    char * m = NULL;
    Object * ret = NULL;
    int s;

    if(argc != 1) {
        m = malloc(sizeof(char) * 40);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 1, argc);
        ret = new_error(m);
    } else if(args[0]->type != STRING && args[0]->type != ARRAY) {
        m = malloc(sizeof(char) * 40);
        sprintf(m, "Argument not a string or an array");
        ret = new_error(m);
    } else {
        if(args[0]->type == STRING) {
            s = ((String *) ((StringObject *) args[0]->value)->value)->len;
        } else {
            s = ((Array *) ((ArrayObject *) args[0]->value)->elements)->size;
        }

        ret = bi_int_obj(s);
    }

    bi_free_args(obj, args, argc);

    return ret;
}
