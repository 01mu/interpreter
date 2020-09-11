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

    while(1) {
        char str[120];

        printf(">>> ");
        fgets(str, 120, stdin);

        lexer = new_lexer(str);
        parser = new_parser(lexer);
        program = parse_program(parser);

        if(check_parser_errors(parser)) {
            continue;
        }

        if(program->sc > 0) {
            eval_statements(program->statements, program->sc, env);
        }
    }
}
