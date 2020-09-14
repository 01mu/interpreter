/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/repl.c
 *
 */

void repl();
void repl_do_test(char * input);
void repl_test();

void repl_test() {
    int i, c = 4;

    char * t[4] = {
        "let a = 1; let b = a; let a = b; a; b;",
        "!false; !!true; false; true; !5; !!5; !!!500; -5; !5 == 5;",
        "let a = 3; if(a + 1) { a } else {  }",
        "let a = 3; let b = 5; let z = 3; 1 + 2;"};

    for(i = 0; i < c; i++) {
        printf("--------------------------------------\n");
        repl_do_test(t[i]);
    }
}

void repl_do_test(char * input) {
    Lexer * lexer = NULL;
    Parser * parser = NULL;
    Program * program = NULL;
    Env * env = new_env();

    lexer = new_lexer(input);
    parser = new_parser(lexer);
    program = parse_program(parser);

    if(check_parser_errors(parser)) {
        free_program(lexer, parser, program);
        return;
    }

    if(program->sc > 0) {
        eval_statements(program->statements, program->sc, env);
    }

    free_program(lexer, parser, program);

    env_free(env);
}

void repl() {
    Lexer * lexer = NULL;
    Parser * parser = NULL;
    Program * program = NULL;
    Env * env = new_env();

    char str[120];

    printf("Type '\\h' for help and '\\q' to quit.\n");

    while(1) {
        printf(">>> ");
        fgets(str, 120, stdin);

        if(strcmp(str, "\\q\n") == 0) {
            printf("Bye!\n");
            break;
        }

        lexer = new_lexer(str);
        parser = new_parser(lexer);
        program = parse_program(parser);

        if(check_parser_errors(parser)) {
            free_program(lexer, parser, program);
            continue;
        }

        if(program->sc > 0) {
            eval_statements(program->statements, program->sc, env);
        }

        free_program(lexer, parser, program);
    }

    env_free(env);
}
