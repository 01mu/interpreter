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
    while(1) {
        char str[120];

        printf(">>> ");
        fgets(str, 120, stdin);

        Lexer * lexer = new_lexer(str);
        Parser * parser = new_parser(lexer);
        Program * program = parse_program(parser);
        Object * object;

        if(strlen(str) > 1) {
            object = eval_statement(program->statements[0]);
            print_object(object);
        }
    }
}
