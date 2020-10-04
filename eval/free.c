/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/free.c
 *
 */

void free_eval_expression(char * ext, Object * obj, Env * env, bool free_obj) {
    int i;
    ArrayObject * ao = NULL;
    Array * el = NULL;
    Object * t = NULL;

    if(strcmp(ext, INT) == 0) {
        free(obj->value);
    } else if(strcmp(ext, PREFIX) == 0 && strcmp(obj->type, INT) == 0) {
        free(obj->value);
    } else if(strcmp(ext, NULL_) == 0) {
        return;
    } else if(strcmp(ext, INFIX) == 0) {
        if(strcmp(obj->type, ARRAY) == 0 || strcmp(obj->type, STRING) == 0) {
            free_eval_expression(obj->type, obj, NULL, 0);
        } else {
            free(obj->value);
        }
    } else if(strcmp(ext, CALL) == 0) {
        if(strcmp(obj->type, ARRAY) == 0 || strcmp(obj->type, STRING) == 0) {
            free_eval_expression(obj->type, obj, NULL, 0);
        } else {
            free(obj->value);
        }
    } else if(strcmp(ext, CALL) == 0 && strcmp(obj->type, INT) == 0) {
        free(obj->value);
    } else if(strcmp(ext, FUNCTION) == 0) {
        free(obj->value);
    } else if(strcmp(ext, ARRAYIDX) == 0) {
        if(strcmp(obj->type, ARRAY) == 0) {
            free_eval_expression(ARRAY, obj, NULL, 0);
        } else {
            free_eval_expression(obj->type, obj, NULL, 0);
        }
    } else if(strcmp(ext, BUILTIN) == 0) {
        free(obj->value);
    } else if(strcmp(ext, ARRAY) == 0) {
        ao = obj->value;
        el = ao->elements;

        for(i = 0; i < el->size; i++) {
            t = el->array[i];

            if(is_bool_or_ident(t->type)) {
                el->array[i] = malloc(1);
            } else {
                free_eval_expression(t->type, t, NULL, 0);
            }
        }

        array_free(el);
        free(obj->value);
    } else if(strcmp(ext, STRING) == 0) {
        string_free(((StringObject *) obj->value)->value);
        free(obj->value);
    } else {
        return;
    }

    if(free_obj) {
        free(obj);
    }
}
