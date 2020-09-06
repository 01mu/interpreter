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
#include "core/free.c"
#include "core/lexer.c"
#include "core/parser.c"
#include "core/eval.c"
#include "core/print.c"
#include "test/parsing.c"
#include "test/eval.c"

int main(int argc, char * argv[])
{
    if(!false_bool) {
        false_bool = malloc(sizeof(Object));
        false_bool->type = FALSE;
        false_bool->value = malloc(sizeof(BooleanObject));
        ((BooleanObject *) false_bool->value)->value = false;
    }

    if(!true_bool) {
        true_bool = malloc(sizeof(Object));
        true_bool->type = TRUE;
        true_bool->value = malloc(sizeof(BooleanObject));
        ((BooleanObject *) true_bool->value)->value = true;
    }

    if(strcmp(argv[1], "prefix") == 0) {
        test_parsing_prefix_expressions();
    } else if(strcmp(argv[1], "hash") == 0) {
        hash_map_test();
    } else if(strcmp(argv[1], "fliteral") == 0) {
        test_parsing_function_literal_expressions();
    } else if(strcmp(argv[1], "call") == 0) {
        test_parsing_call_expressions();
    } else if(strcmp(argv[1], "grouped") == 0) {
        test_parsing_grouped_expressions();
    } else if(strcmp(argv[1], "if") == 0) {
        test_parsing_if_expressions();
    } else if(strcmp(argv[1], "infix") == 0) {
        test_parsing_infix_expressions();
    } else if(strcmp(argv[1], "integer") == 0) {
        test_integer_literal_expression();
    } else if(strcmp(argv[1], "identifier") == 0) {
        test_identifier_expression();
    } else if(strcmp(argv[1], "return") == 0) {
        test_return_statements();
    } else if(strcmp(argv[1], "next") == 0) {
        test_next_token();
    } else if(strcmp(argv[1], "let") == 0) {
        test_let_statements();
    } else if(strcmp(argv[1], "print") == 0) {
        test_print_program();
    } else if(strcmp(argv[1], "test-eval") == 0) {
        test_eval_all();
    } else if(strcmp(argv[1], "test-parse") == 0) {
        test_parse_all();
    } else if(strcmp(argv[1], "repl") == 0) {
        while(1) {
            char str[120];

            printf(">> ");
            fgets(str, 120, stdin);

            Lexer * lexer = new_lexer(str);
            Parser * parser = new_parser(lexer);
            Program * program = parse_program(parser);

            printf("%s", print_program(program));
        }
    }

    return 0;
}
