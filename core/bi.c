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
bool is_array(Object * obj);
Object * eval_new_array();
Object * eval_new_string();

Object * bi_int_obj(int i) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = i;
    obj->type = INT;
    obj->value = iobj;
    obj->ref = 0;

    return obj;
}

void bi_free_args(Object * obj, Object ** args, int argc) {
    int i;

    for(i = 0; i < argc; i++) {
        if(args[i]->ref == 0) {
            free_eval_expression(args[i]->type, args[i], NULL, true);
        } else {
            args[i]->type = ARRAY;
        }
    }

    free_eval_expression(obj->type, obj, NULL, true);
}

Object * bi_type(Object * obj, Object ** args, int argc) {
    char * m = NULL;
    Object * new = NULL, * rt = NULL;
    String * ns = NULL;
    StringObject * so = NULL;

    if(argc != 1) {
        m = malloc(sizeof(char) * 40);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 1, argc);
        rt = new_error(m);
    } else {
        new = eval_new_string();
        so = new->value;

        string_cat(so->value, args[0]->type, 0);

        rt = new;
    }

    bi_free_args(obj, args, argc);

    return rt;
}

Object * bi_rest(Object * obj, Object ** args, int argc) {
    int i;
    char * m = NULL, * l = NULL;
    Object * rt = NULL;
    ArrayObject * ao = NULL;
    Array * arr = NULL, * new_arr = NULL;

    if(argc != 1) {
        m = malloc(sizeof(char) * 40);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 1, argc);
        rt = new_error(m);
    } else if(!is_array(args[0])) {
        m = malloc(sizeof(char) * 30);
        sprintf(m, "First argument not an array");
        rt = new_error(m);
    } else {
        ao = args[0]->value;
        arr = ao->elements;
        rt = eval_new_array();
        new_arr = ((ArrayObject *) rt->value)->elements;

        for(i = 1; i < arr->size; i++) {
            array_insert(new_arr, copy_object(arr->array[i]));
        }

    }

    bi_free_args(obj, args, argc);

    return rt;
}

Object * bi_last(Object * obj, Object ** args, int argc) {
    char * m = NULL, * l = NULL;
    Object * rt = NULL;
    ArrayObject * ao = NULL;
    Array * arr = NULL;

    if(argc != 1) {
        m = malloc(sizeof(char) * 40);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 1, argc);
        rt = new_error(m);
    } else if(!is_array(args[0])) {
        m = malloc(sizeof(char) * 30);
        sprintf(m, "First argument not an array");
        rt = new_error(m);
    } else {
        ao = args[0]->value;
        arr = ao->elements;
        rt = copy_object(arr->array[arr->size - 1]);
    }

    bi_free_args(obj, args, argc);

    return rt;
}

Object * bi_first(Object * obj, Object ** args, int argc) {
    char * m = NULL, * l = NULL;
    Object * rt = NULL;
    ArrayObject * ao = NULL;
    Array * arr = NULL;

    if(argc != 1) {
        m = malloc(sizeof(char) * 40);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 1, argc);
        rt = new_error(m);
    } else if(!is_array(args[0])) {
        m = malloc(sizeof(char) * 30);
        sprintf(m, "First argument not an array");
        rt = new_error(m);
    } else {
        ao = args[0]->value;
        arr = ao->elements;
        rt = copy_object(arr->array[0]);
    }

    bi_free_args(obj, args, argc);

    return rt;
}

Object * bi_push(Object * obj, Object ** args, int argc) {
    char * m = NULL, * l = NULL;
    Object * rt = NULL;
    ArrayObject * ao = NULL;
    Array * arr = NULL;

    if(argc != 2) {
        m = malloc(sizeof(char) * 40);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 2, argc);
        rt = new_error(m);
    } else if(!is_array(args[0])) {
        m = malloc(sizeof(char) * 30);
        sprintf(m, "First argument not an array");
        rt = new_error(m);
    } else {
        ao = args[0]->value;
        arr = ao->elements;

        array_insert(arr, copy_object(args[1]));

        if(rt == NULL) {
            bi_free_args(obj, args, argc);
            return null_obj;
        }
    }

    bi_free_args(obj, args, argc);
    args[0]->type = ARRAY;

    return rt;
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
        new->ref = 0;
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

Object * get_built_in_fn(char * type, Object * obj, Object ** args, int argc) {
    if(strcmp(type, "len") == 0) {
        return bi_len(obj, args, argc);
    } else if(strcmp(type, "find") == 0) {
        return bi_find(obj, args, argc);
    } else if(strcmp(type, "str") == 0) {
        return bi_str(obj, args, argc);
    } else if(strcmp(type, "push") == 0) {
        return bi_push(obj, args, argc);
    } else if(strcmp(type, "first") == 0) {
        return bi_first(obj, args, argc);
    } else if(strcmp(type, "last") == 0) {
        return bi_last(obj, args, argc);
    } else if(strcmp(type, "rest") == 0) {
        return bi_rest(obj, args, argc);
    } else if(strcmp(type, "type") == 0) {
        return bi_type(obj, args, argc);
    }

    return null_obj;
}
