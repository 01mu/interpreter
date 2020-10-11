/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/eval.c
 *
 */

bool is_array(Object * obj);
bool eval_is_hashmap(Object * obj);

#include "../eval/get.c"
#include "../eval/free.c"
#include "../eval/new.c"
#include "../eval/copy.c"
#include "../eval/hash.c"
#include "../eval/call.c"
#include "../eval/if.c"
#include "../eval/infix.c"
#include "../eval/prefix.c"
#include "../eval/index.c"
#include "../eval/let.c"
#include "../eval/return.c"
#include "../eval/array.c"
#include "../eval/function.c"
#include "../eval/bool.c"
#include "../eval/identifier.c"
#include "../eval/string.c"
#include "../eval/integer.c"
#include "../eval/print.c"

bool is_array(Object * obj) {
    return strcmp(obj->type, ARRAY) == 0;
}

bool eval_is_hashmap(Object * obj) {
    return strcmp(obj->type, HASHMAP) == 0;
}

void init_bool(Object ** b, bool lit) {
    (*b) = malloc(sizeof(Object));
    (*b)->type = BOOLEAN;
    (*b)->ref = 0;
    (*b)->value = malloc(sizeof(BooleanObject));
    ((BooleanObject *) (*b)->value)->value = lit;
}

void init_null(Object ** n) {
    (*n) = malloc(sizeof(Object));
    (*n)->type = "NULL";
    (*n)->value = malloc(sizeof(NullObject));
    (*n)->ref = 0;
}

void free_stat(Object * b) {
    free(b->value);
    free(b);
}

bool is_bool_or_ident(char * t) {
    bool eval_is_bool = strcmp(t, TRUE) == 0 || strcmp(t, FALSE) == 0 ||
        strcmp(t, BOOLEAN) == 0;

    if(strcmp(t, IDENT) == 0 || eval_is_bool || strcmp(t, NULL_) == 0) {
        return true;
    }

    return false;
}

bool is_error(Object * obj) {
    if(obj != NULL) {
        return strcmp(obj->type, ERROR) == 0;
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
    } else if(strcmp(ext, HASHMAP) == 0) {
        res = eval_hash_literal((HashLiteral *) est, env);
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

    if(is_error(obj)) {
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

    if(env == env_store->store[0] && strcmp(obj->type, RETURN) == 0) {
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

        if(strcmp(ety, IF) == 0) {
            continue;
        }

        s = print_object(obj);

        if(is_repl_test_string) {
            string_append(repl_test_string, s);
        } else {
            printf("%s", s->string);

            if(strcmp(s->string, "") != 0) {
               printf("\n");
            }

            string_free(s);
        }

        //printf("[%p: %i] %s %s %s\n", env, i, stt, ety, obj->type);

        if(eval_free_error(obj, env) || eval_free_return(obj, env, ety)) {
            return null_obj;
        }

        if(env != henv && strcmp(obj->type, RETURN) == 0) {
            return obj;
        } else if(env != henv && !is_bool_or_ident(ety) && sc == 1) {
            free_eval_expression(obj->type, obj, env, true);
            return null_obj;
        } else if(env != henv && !is_bool_or_ident(ety) && i != sc - 1) {
            free_eval_expression(obj->type, obj, env, true);
        } else if(env == henv && !is_bool_or_ident(obj->type)) {
            free_eval_expression(ety, obj, env, true);
        }
    }

    return obj;
}
