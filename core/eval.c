/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/eval.c
 *
 */

#include "../eval/copy.c"
#include "../eval/call.c"
#include "../eval/if.c"
#include "../eval/infix.c"
#include "../eval/prefix.c"
#include "../eval/idxexp.c"
#include "../eval/let.c"
#include "../eval/return.c"
#include "../eval/arraylit.c"
#include "../eval/funclit.c"
#include "../eval/bool.c"
#include "../eval/ident.c"
#include "../eval/string.c"
#include "../eval/integer.c"

Object * new_error(char * msg) {
    Object * obj = malloc(sizeof(Object));
    ErrorObject * err = malloc(sizeof(ErrorObject));

    obj->type = ERROR;
    obj->value = err;
    err->message = malloc(strlen(msg) + 1);
    err->message[0] = '\0';

    strcat(err->message, msg);
    free(msg);

    return obj;
}

void init_bool(Object ** b, bool lit) {
    (*b) = malloc(sizeof(Object));
    (*b)->type = BOOLEAN;
    (*b)->value = malloc(sizeof(BooleanObject));
    ((BooleanObject *) (*b)->value)->value = lit;
}

void init_null(Object ** n) {
    (*n) = malloc(sizeof(Object));
    (*n)->type = "NULL";
    (*n)->value = malloc(sizeof(NullObject));
}

void free_stat(Object * b) {
    free(b->value);
    free(b);
}

String * print_object(Object * obj) {
    String * s = string_new();
    char * c = malloc(sizeof(char) + 40);

    c[0] = '\0';

    if(strcmp(obj->type, INT) == 0) {
        sprintf(c, "%i ", ((IntegerObject *) obj->value)->value);
    } else if(strcmp(obj->type, BOOLEAN) == 0) {
        sprintf(c, "%i ", ((BooleanObject *) obj->value)->value);
    } else if(strcmp(obj->type, NULL_) == 0) {
        sprintf(c, "NULL ");
    } else if(strcmp(obj->type, ERROR) == 0) {
        sprintf(c, "%s ", ((ErrorObject *) obj->value)->message);
    } else if(strcmp(obj->type, STRING) == 0) {
        sprintf(c, "%s ", ((StringObject *) obj->value)->value->string);
    } else if(strcmp(obj->type, ARRAY) == 0) {

    }

    string_cat(s, c, true);

    return s;
}

bool is_bool_or_ident(char * t) {
    bool eval_is_bool = strcmp(t, TRUE) == 0 || strcmp(t, FALSE) == 0 ||
        strcmp(t, BOOLEAN) == 0;

    if(strcmp(t, IDENT) == 0 || eval_is_bool || strcmp(t, "NULL") == 0) {
        return true;
    }

    return false;
}

bool is_error(Object * obj) {
    if(obj != NULL) {
        return strcmp(obj->type, ERROR) == 0;
    }
}

void free_eval_expression(char * ext, Object * obj, Env * env, bool free_obj) {
    int i;
    ArrayObject * ao = NULL;
    Array * el = NULL;
    Object * t = NULL;

    if(strcmp(ext, INT) == 0) {
        free(obj->value);
    } else if(strcmp(ext, PREFIX) == 0 && strcmp(obj->type, INT) == 0) {
        free(obj->value);
    } else if(strcmp(ext, INFIX) == 0) {
        free(obj->value);
    } else if(strcmp(ext, CALL) == 0) {
        if(strcmp(obj->type, ARRAY) == 0) {
            free_eval_expression(ARRAY, obj, NULL, 0);
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
            free_eval_expression(t->type, t, NULL, 0);
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

Object * eval_expression(char * ext, void * est, Env * env) {
    Object * res = NULL;

    if(strcmp(ext, INT) == 0) {
        res = eval_integer((IntegerLiteral *) est, env);
    } else if(strcmp(ext, STRING) == 0) {
        res = eval_string((StringLiteral *) est, env);
    } else if(strcmp(ext, IDENT) == 0) {
        res = eval_identifier((Identifier *) est, env);
    } else if(strcmp(ext, PREFIX) == 0) {
        res = eval_prefix_expression((PrefixExpression *) est, env);
    } else if(strcmp(ext, INFIX) == 0) {
        res = eval_infix_expression((InfixExpression *) est, env);
    } else if(strcmp(ext, TRUE) == 0 || strcmp(ext, FALSE) == 0) {
        res = eval_bool(((Boolean *) est)->value, env);
    } else if(strcmp(ext, IF) == 0) {
        res = eval_if_expression((IfExpression *) est, env);
    } else if(strcmp(ext, FUNCTION) == 0 || strcmp(ext, BUILTIN) == 0) {
        res = eval_function_literal((FunctionLiteral *) est, env);
    } else if(strcmp(ext, ARRAY) == 0) {
        res = eval_array_literal((ArrayLiteral *) est, env);
    } else if(strcmp(ext, ARRAYIDX) == 0) {
        res = eval_index_expression((IndexExpression *) est, env);
    } else if(strcmp(ext, CALL) == 0) {
        res = eval_call_expression((CallExpression *) est, env);
    }

    return res;
}

Object * eval_statement(Statement statement, Env * env) {
    Object * obj = NULL;
    LetStatement * let = NULL;
    ReturnStatement * ret = NULL;
    ExpressionStatement * est = NULL;

    if(strcmp(statement.type, LET) == 0) {
        let = (LetStatement *) statement.st;
        est = (ExpressionStatement *) let->value;
        obj = eval_let_statement(est, env, let->name.value);
    } else if(strcmp(statement.type, RETURN) == 0) {
        ret = (ReturnStatement *) statement.st;
        est = (ExpressionStatement *) ret->value;
        obj = eval_return_statement(est, env);
    } else if(strcmp(statement.type, EXPRESSION) == 0) {
        est = (ExpressionStatement *) statement.st;
        obj = eval_expression(est->expression_type, est->expression, env);
    }

    return obj;
}

bool eval_free_error(Object * obj, Env * env) {
    ErrorObject * err = NULL;
    bool is_error = strcmp(obj->type, ERROR) == 0;

    if(is_error) {
        err = (ErrorObject *) obj->value;
        free(err->message);
        free(err);
        free(obj);
        return true;
    }

    return false;
}

bool eval_free_return(Object * obj, Env * env, char * ety) {
    ReturnValue * rv = NULL;
    bool is_return = strcmp(obj->type, RETURN) == 0;

    if(env == env_store->store[0] && is_return) {
        rv = (ReturnValue *) obj->value;
        free_eval_expression(ety, rv->value, env, true);
        free(obj->value);
        free(obj);
        return true;
    }

    return false;
}

Object * eval_statements(Statement * statements, int sc, Env * env) {
    int i;
    Object * obj = NULL;
    ExpressionStatement * est = NULL;
    char * stt = NULL, * ety = NULL;
    Env * henv = env_store->store[0];
    String * s = NULL;

    for(i = 0; i < sc; i++) {
        obj = eval_statement(statements[i], env);
        stt = statements[i].type;
        est = (ExpressionStatement *) statements[i].st;
        ety = est->expression_type;

        if(strcmp(ety, IF) != 0 && obj != null_obj) {
            s = print_object(obj);

            if(is_repl_test_string) {
                //printf("%s", s->string);
                string_append(repl_test_string, s);
            } else {
                printf("%s", s->string);

                if(strcmp(s->string, "") != 0) {
                   printf("\n");
                }

                string_free(s);
            }
        } else {
            continue;
        }

        //printf("[%p: %i] %s %s %s\n", env, i, stt, ety, obj->type);

        if(eval_free_error(obj, env) || eval_free_return(obj, env, ety)) {
            //return null_obj;
        } else if(env != henv && strcmp(obj->type, RETURN) == 0) {
            return obj;
        } else if(env != henv && !is_bool_or_ident(ety) && sc == 1) {
            free_eval_expression(obj->type, obj, env, true);
            return null_obj;
        } else if((strcmp(ety, INFIX) == 0 ||
            strcmp(ety, CALL) == 0) && strcmp(obj->type, STRING) == 0) {

            free_eval_expression(obj->type, obj, env, true);
        } else if(env != henv && !is_bool_or_ident(ety) && i != sc - 1) {
            free_eval_expression(obj->type, obj, env, true);
        } else if(env == henv && !is_bool_or_ident(obj->type)) {
            free_eval_expression(ety, obj, env, true);
        }
    }

    return obj;
}
