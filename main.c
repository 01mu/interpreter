/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * main.c
 *
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <stdbool.h>

#include "header/tokens.h"
#include "header/types.h"
#include "header/functions.h"

#include "lib/string.c"
#include "lib/sortedlist.c"
#include "lib/hashmap.c"

#include "core/env.c"
#include "core/free.c"
#include "core/lexer.c"
#include "core/parser.c"
#include "core/eval.c"
#include "core/print.c"
#include "core/repl.c"

#include "test/parsing.c"
#include "test/eval.c"

int main(int argc, char * argv[])
{
    init_bool(&true_bool, true);
    init_bool(&false_bool, false);
    init_null(&null_obj);

    if(strcmp(argv[1], "test-hash") == 0) {
        hash_map_test();
    } else if(strcmp(argv[1], "test-print") == 0) {
        test_print_program();
    } else if(strcmp(argv[1], "test-eval") == 0) {
        test_eval_all(argv[2]);
    } else if(strcmp(argv[1], "test-parse") == 0) {
        test_parse_all(argv[2]);
    } else if(strcmp(argv[1], "test-repl") == 0) {
        repl_test();
    } else if(strcmp(argv[1], "repl") == 0) {
        repl(0, 0, NULL, 0);
    } else {
        printf("Invalid command\n");
    }

    free_stat(true_bool);
    free_stat(false_bool);
    free_stat(null_obj);

    return 0;
}
