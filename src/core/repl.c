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
    int i;

    /*char * t[] = {
        "let a = true; let a = false; let b = a; a; !b;",
        "let var = 1 ---3; let z = -1; z * -2;",
        "let a = !3; if(!!!!false == true) { a } else { 55 }",
        "!false; !!true; false; true; !5; !!5; !!!500; -5; !5 == 5;",
        "let a = 3; if(a + 1) { a } else {  }",
        "let a = 3; let b = 5; let z = 3; 1 + 2;",
        "let a = 2; let b = a; let a = b + 2; 3;",
    };*/

    char * t[] = {
        "let mult = fn(x) { x; }; let a = 1; mult(a);",
        //"let mult = fn(x, a) { x; 1; }; let z = 2; mult(z, 10);",
        //"let mult = fn(x, a) { x * a; }; let z = 2; mult(z, 10);",
    };

    for(i = 0; i < sizeof(t) / sizeof(t[1]); i++) {
        repl_do_test(t[i]);
    }
}

void repl_do_test(char * input) {
    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Env * env = new_env();
    Object * r = NULL;

    if(check_parser_errors(parser)) {
        free_program(lexer, parser, program);
        return;
    }

    if(program->sc > 0) {
        r = eval_statements(program->statements, program->sc, env);
    }

    env_free(env);

    if(out != NULL) {
        env_free(out);
        out = NULL;
    }

    free_program(lexer, parser, program);
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
