/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/funclit.c
 *
 */

Object * eval_function_literal(FunctionLiteral * fl, Env * env) {
    Object * obj = malloc(sizeof(Object));
    Function * function = malloc(sizeof(Function));

    obj->type = FUNCTION;
    obj->value = function;

    function->parameters = fl->parameters;
    function->body = fl->body;
    function->env = env;
    function->pc = fl->pc;

    return obj;
}
