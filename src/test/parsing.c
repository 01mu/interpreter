/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * test/parsing.c
 *
 */

void test_parse_all() {
    int i, c = 11;

    char * s[11] = {
        test_next_token(),
        test_let_statements(),
        test_return_statements(),
        test_identifier_expression(),
        test_integer_literal_expression(),
        test_parsing_prefix_expressions(),
        test_parsing_infix_expressions(),
        test_parsing_function_literal_expressions(),
        test_parsing_call_expressions(),
        test_parsing_grouped_expressions(),
        test_parsing_if_expressions()};

    printf("\n");

    for(int i = 0; i < c; i++) {
        if(i > 0) {
            free(s[i-1]);
        }

        printf("%s", s[i]);
    }

    free(s[c - 1]);
}

char * test_get_statement_type(char * type, void * expr) {
    if(strcmp(type, LET) == 0) {
        return ((LetStatement *) expr)->type;
    } else if(strcmp(type, RETURN) == 0) {
        return ((ReturnStatement *) expr)->type;
    } else {
        return ((ExpressionStatement *) expr)->expression_type;
    }
}

bool test_string_cmp(char * b, char * g, char * ex, int i, int * fail) {
    if(strcmp(g, ex) != 0) {
        * fail += 1;
        printf(b, i, g, ex);
        return false;
    }

    return true;
}

bool test_int_cmp(char * b, int g, int ex, int i, int * fail) {
    if(g != ex) {
        * fail += 1;
        printf(b, i, g, ex);
        return false;
    }

    return true;
}

char * print_test_result(char * b, int f, int tc) {
    char * z, * color = ANSI_COLOR_RED;

    z = malloc(sizeof(char) * (45 + strlen(b)));

    if(f == tc) {
        sprintf(z, ANSI_COLOR_GREEN "[PASSED] " ANSI_COLOR_RESET
            "%s: %i out of %i passed\n", b, f, tc);
    } else {
        sprintf(z, ANSI_COLOR_RED "[FAILED] " ANSI_COLOR_RESET
            "%s: %i out of %i passed\n", b, f, tc);
    }

    return z;
}

char * test_next_token() {
    int i, tc = 20, fail = 0;

    const char * input =
        "let x = 5; \
        let y = 10; \
        return asd; \
        let foobar = 838383;;";

    char * tests[20][2] = {
        {LET, "let"},
        {IDENT, "x"},
        {ASSIGN, "="},
        {INT, "5"},
        {SEMICOLON, ";"},
        {LET, "let"},
        {IDENT, "y"},
        {ASSIGN, "="},
        {INT, "10"},
        {SEMICOLON, ";"},
        {RETURN, "return"},
        {IDENT, "asd"},
        {SEMICOLON, ";"},
        {LET, "let"},
        {IDENT, "foobar"},
        {ASSIGN, "="},
        {INT, "838383"},
        {SEMICOLON, ";"},
        {SEMICOLON, ";"},
        {EOF_, ""}};

    Lexer * lexer = new_lexer(input);
    Token token = lexer_next_token(lexer);

    printf("Testing NEXT TOKEN\n");

    for(i = 0; i < tc; i++) {
        printf("[%i] %s %s\n", i, tests[i][0], tests[i][1]);

        test_string_cmp("[Error: %i] Expected token type %s got %s\n",
            tests[i][0], token.type, i, &fail);
        test_string_cmp("[Error: %i] Expected token literal %s got %s\n",
            tests[i][1], token.literal, i, &fail);

        token = lexer_next_token(lexer);
    }

    return print_test_result("NEXT TOKEN", tc - fail, tc);
}

char * test_let_statements() {
    int i, tc = 5, fail = 0;
    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    LetStatement * let;
    struct {
        char * input, * type;
    } t[5] = {
        {"let x = 2;", INT},
        {"let sd = !2;", PREFIX},
        {"let h = a;", IDENT},
        {"let z = if (x) { } else { };", IF},
        {"let a = asd(d);", CALL}};

    printf("Testing LET statements\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        let = (LetStatement *) stmt.st;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            LET, stmt.type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_let_statement(let));

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        test_string_cmp("[Error: %i] Expected assign type %s got %s\n",
            t[i].type, let->type, i, &fail);
    }

    return print_test_result("LET", tc - fail, tc);
}

char * test_return_statements() {
    int i, tc = 5, fail = 0;
    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ReturnStatement * ret;
    struct {
        char * input, * type;
    } t[5] = {
        {"return x;", IDENT},
        {"return 5 + 5;", INFIX},
        {"return 5;", INT},
        {"return if (x) { } else { };", IF},
        {"return !2;", PREFIX}};

    printf("Testing RETURN statements\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        ret = (ReturnStatement *) stmt.st;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            RETURN, stmt.type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_return_statement(ret));

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        test_string_cmp("[Error: %i] Expected return type %s got %s\n",
            t[i].type, ret->type, i, &fail);
    }

    return print_test_result("RETURN", tc - fail, tc);
}

char * test_identifier_expression() {
    int i, tc = 5, fail = 0;
    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    Identifier * id;
    struct {
        char * input, * expected;
    } t[5] = {
        {"zz_zz;", "zz_zz"},
        {"a ;", "a"},
        {"bbb ;", "bbb"},
        {"meme;;", "meme"},
        {"foobar;", "foobar"}};

    printf("Testing IDENTIFIER expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        id = (Identifier *) es->expression;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            IDENT, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_identifier_value(id));

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        test_string_cmp("[Error: %i] Expected identifier value %s got %s\n",
            t[i].expected, id->value, i, &fail);
    }

    return print_test_result("IDENTIFIER", tc - fail, tc);
}

char * test_integer_literal_expression() {
    int i, tc = 5, fail = 0;
    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    IntegerLiteral * il;
    struct {
        char * input;
        int value;
    } t[5] = {
        {"5;", 5},
        {"11111;", 11111},
        {"2;", 2},
        {"0;", 0},
        {"100000;", 100000}};

    printf("Testing INTEGER expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        il = (IntegerLiteral *) es->expression;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            INT, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_integer_literal(il));

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        test_int_cmp("[Error: %i] Expected integer value %i got %i\n",
            t[i].value, il->value, i, &fail);
    }

    return print_test_result("INTEGER", tc - fail, tc);
}

char * test_parsing_prefix_expressions() {
    int i, tc = 5, fail = 0;
    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    PrefixExpression * pex;
    struct {
        char * input, * operator, * expression_type;
    } tests[5] = {
        {"!5", "!", INT},
        {"-15", "-", INT},
        {"!fn(x) { } ", "!", FUNCTION},
        {"-var", "-", IDENT},
        {"!if(a) { } else { }", "!", IF}};

    printf("Testing PREFIX expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(tests[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        pex = (PrefixExpression *) es->expression;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            PREFIX, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_prefix_expression(pex));

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        test_string_cmp("[Error: %i] Expected operator %s got %s\n",
            tests[i].operator, pex->operator, i, &fail);
        test_string_cmp("[Error: %i] Expected right value %s got %s\n",
            tests[i].expression_type, pex->expression_type, i, &fail);
    }

    return print_test_result("PREFIX", tc - fail, tc);
}

char * test_parsing_infix_expressions() {
    int i, tc = 5, fail = 0;
    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    InfixExpression * iex;
    IntegerLiteral * right_il;
    IntegerLiteral * left_il;
    struct {
        char * input, * operator;
        int left_val, right_val;
    } t[5] = {
        {"100 + 1;", "+", 100, 1},
        {"0 - 1;", "-", 0, 1},
        {"2 * 23;", "*", 2, 23},
        {"7 / 51;", "/", 7, 51},
        {"2 != 3;", "!=", 2, 3}};

    printf("Testing INFIX expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        iex = (InfixExpression *) es->expression;
        right_il = (IntegerLiteral *) iex->right;
        left_il = (IntegerLiteral *) iex->left;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            INFIX, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_infix_expression(iex));

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        test_string_cmp("[Error: %i] Expected operator %s got %s\n",
            t[i].operator, iex->operator, i, &fail);
        test_int_cmp("[Error: %i] Expected left value %i got %i\n",
            t[i].left_val, left_il->value, i, &fail);
        test_int_cmp("[Error: %i] Expected right value %i got %i\n",
           t[i].right_val, right_il->value, i, &fail);
    }

    return print_test_result("INFIX", tc - fail, tc);
}

char * test_parsing_function_literal_expressions() {
    int i, tc = 5, fail = 0;
    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    FunctionLiteral * fl;
    BlockStatement * bs;
    struct {
        char * input;
        int param_count, statement_count;
    } t[5] = {
        {"fn(a) { let a = 4; }", 1, 1},
        {"fn(a, b) { x a }", 2, 2},
        {"fn(z) { !5 }", 1, 1},
        {"fn(a) { }", 1, 0},
        {"fn() { }", 0, 0}};

    printf("Testing FUNCTION LITERAL expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        fl = (FunctionLiteral *) es->expression;
        bs = (BlockStatement *) fl->body;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            FUNCTION, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_function_literal(fl));

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        test_int_cmp("[Error: %i] Expected parameter count %i got %i\n",
            t[i].param_count, fl->pc, i, &fail);
        test_int_cmp("[Error: %i] Expected statement count %i got %i\n",
            t[i].statement_count, bs->sc, i, &fail);
    }

    return print_test_result("FUNCTION LITERAL", tc - fail, tc);
}

char * test_parsing_call_expressions() {
    int i, tc = 5, fail = 0;
    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    CallExpression * cex;
    struct {
        char * input, * function_type, * arg_type;
        int ac;
    } t[5] = {
        {"t(if(a > u) { a } else { return u; }, 2, 5)", IDENT, IF, 3},
        {"t(!asd)", IDENT, PREFIX, 1},
        {"func(false, 2, false)", IDENT, FALSE, 3},
        {"fn(1, 2, 3) { } ( 3 )", FUNCTION, INT, 1},
        {"fn(z) { z } (var)", FUNCTION, IDENT, 1}};

    printf("Testing CALL expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        cex = (CallExpression *) es->expression;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            CALL, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_call_expression(cex));

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        test_string_cmp("[Error: %i] Expected function type %s got %s\n",
            t[i].function_type, cex->function_type, i, &fail);
        test_int_cmp("[Error: %i] Expected argument count %i got %i\n",
            t[i].ac, cex->ac, i, &fail);

        es = (ExpressionStatement *) cex->arguments[0];

        test_string_cmp("[Error: %i] Expected first argument type %s got %s\n",
            t[i].arg_type, es->expression_type, i, &fail);
    }

    return print_test_result("CALL", tc - fail, tc);
}

char * test_parsing_grouped_expressions() {
    int i, tc = 5, fail = 0;
    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    PrefixExpression * pex;
    InfixExpression * iex;
    struct {
        char * input, * group_type, * lt, * rt;
    } t[5] = {
        {"1 + (2 + 3) + var", INFIX, INFIX, IDENT},
        {"(5 + 5) * 2", INFIX, INFIX, INT},
        {"2 / (5 + 5)", INFIX, INT, INFIX},
        {"-(5 + 5)", PREFIX, MINUS, INFIX},
        {"!(true == false)", PREFIX, BANG, INFIX}};

    printf("Testing GROUPED expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;

        if(strcmp(es->expression_type, INFIX) == 0) {
            iex = (InfixExpression *) es->expression;

            printf("[%i] %s\n", i, print_infix_expression(iex));

            test_string_cmp("[Error: %i] Expected left type %s got %s\n",
                t[i].lt, iex->left_expression_type, i, &fail);
            test_string_cmp("[Error: %i] Expected right type %s got %s\n",
                t[i].rt, iex->right_expression_type, i, &fail);
        } else if(strcmp(es->expression_type, PREFIX) == 0){
            pex = (PrefixExpression *) es->expression;

            printf("[%i] %s\n", i, print_prefix_expression(pex));

            test_string_cmp("[Error: %i] Expected left type %s got %s\n",
                t[i].lt, pex->operator, i, &fail);
            test_string_cmp("[Error: %i] Expected right type %s got %s\n",
                t[i].rt, pex->expression_type, i, &fail);
        }

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }
    }

    return print_test_result("GROUPED", tc - fail, tc);
}

char * test_parsing_if_expressions() {
    int i, tc = 5, fail = 0;
    char * cons_type, * alt_type;
    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    IfExpression * ifx;
    BlockStatement * cons;
    BlockStatement * alt;
    struct {
        char * input, * condition_type, * cons_type, * alt_type;
    } t[5] = {
        {"if (x) { let a = 3; } else { return 1; }",
            IDENT, INT, INT},
        {"if (x == 1) { let a = b; } else { 5 }",
            INFIX, IDENT, INT},
        {"if (x) { let a = b; } else { 5 }",
            IDENT, IDENT, INT},
        {"if (x) { let a = b; } else { if (z) { } else { } }",
            IDENT, IDENT, IF},
        {"if (a > 3) { !2 } else { false }",
            INFIX, PREFIX, FALSE}};

    printf("Testing IF expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        ifx = (IfExpression *) es->expression;
        cons = (BlockStatement *) ifx->consequence;
        alt = (BlockStatement *) ifx->alternative;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            IF, es->expression_type, i, &fail)) {
            continue;
        }

        cons_type = test_get_statement_type(cons->statements[0].type,
            cons->statements[0].st);
        alt_type = test_get_statement_type(alt->statements[0].type,
            alt->statements[0].st);

        printf("[%i] %s\n", i, print_if_expression(ifx));

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        test_string_cmp("[Error: %i] Expected condition type %s got %s\n",
            t[i].condition_type, ifx->condition_type, i, &fail);
        test_string_cmp("[Error: %i] Expected consequence type %s got %s\n",
            t[i].cons_type, cons_type, i, &fail);
        test_string_cmp("[Error: %i] Expected altnerative type %s got %s\n",
            t[i].alt_type, alt_type, i, &fail);
    }

    return print_test_result("IF", tc - fail, tc);
}
