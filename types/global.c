/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * types/global.c
 *
 */

Object * false_bool = NULL, * true_bool = NULL;
Object * null_obj = NULL;

FunctionLiteralStore * fls = NULL;
EnvStore * env_store = NULL;

String * repl_test_string = NULL;
bool is_repl_test_string = false;

HashMap * fn_map = NULL;
