/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/repl.c
 *
 */

void repl();

void repl() {
    Lexer * lexer = NULL;
    Parser * parser = NULL;
    Program * program = NULL;
    Env * env = new_env();

    char str[120];

    printf("Type '\\h' for help and '\\q' to quit.\n");

    //char * test = "let a = 4;";

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
