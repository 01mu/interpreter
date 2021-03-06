/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * test/parsing.c
 *
 */

char * test_parsing_array_literal();
char * test_parsing_array_index();
char * test_parsing_hash_map_literal();

void test_parse_all(char * option) {
    int i, c;
    char * result = NULL;

    fls = malloc(sizeof(FunctionLiteralStore));
    fls->count = 0;
    fls->store = malloc(sizeof(FunctionLiteral *));

    if(option == NULL) {
        free_function_literal_store();
        printf("Usage: test-parse <option> (all, if, prefix, boolean, etc.)\n");
        return;
    } else if(strcmp(option, "prefix") == 0) {
        result = test_parsing_prefix_expressions();
    } else if(strcmp(option, "boolean") == 0) {
        result = test_parsing_boolean_expression();
    } else if(strcmp(option, "fliteral") == 0) {
        result = test_parsing_function_literal_expressions();
    } else if(strcmp(option, "call") == 0) {
        result = test_parsing_call_expressions();
    } else if(strcmp(option, "grouped") == 0) {
        result = test_parsing_grouped_expressions();
    } else if(strcmp(option, "if") == 0) {
        result = test_parsing_if_expressions();
    } else if(strcmp(option, "infix") == 0) {
        result = test_parsing_infix_expressions();
    } else if(strcmp(option, "integer") == 0) {
        result = test_integer_literal_expression();
    } else if(strcmp(option, "identifier") == 0) {
        result = test_identifier_expression();
    } else if(strcmp(option, "return") == 0) {
        result = test_return_statements();
    } else if(strcmp(option, "next") == 0) {
        result = test_next_token();
    } else if(strcmp(option, "let") == 0) {
        result = test_let_statements();
    } else if(strcmp(option, "array") == 0) {
        result = test_parsing_array_literal();
    } else if(strcmp(option, "arrayidx") == 0) {
        result = test_parsing_array_index();
    } else if(strcmp(option, "hashmap") == 0) {
        result = test_parsing_hash_map_literal();
    } else {
        c = 14;

        char * s[14] = {
            test_next_token(),
            test_let_statements(),
            test_return_statements(),
            test_identifier_expression(),
            test_parsing_boolean_expression(),
            test_integer_literal_expression(),
            test_parsing_prefix_expressions(),
            test_parsing_infix_expressions(),
            test_parsing_function_literal_expressions(),
            test_parsing_call_expressions(),
            test_parsing_grouped_expressions(),
            test_parsing_array_literal(),
            test_parsing_array_index(),
            test_parsing_if_expressions()};

        printf("\n");

        for(i = 0; i < c; i++) {
            printf("%s", s[i]);
        }

        return;
    }

    free_function_literal_store();
    printf("%s", result);
}

bool get_parse_test_expression(ExpressionStatement ** es, char * input) {
    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Statement stmt = program->statements[0];

    * es = (ExpressionStatement *) stmt.st;

    if(check_parser_errors(parser)) {
        return false;
    }

    return true;
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
    int i, tc = 22, fail = 0;

    const char * input =
        "let x = 5+2; \
        let y = 10; \
        return asd; \
        let foobar = 838383;;";

    char * tests[22][2] = {
        {LET, "let"},
        {IDENT, "x"},
        {ASSIGN, "="},
        {INT, "5"},
        {PLUS, "+"},
        {INT, "2"},
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
    ExpressionStatement * es;
    Identifier * id;
    struct {
        char * input, * expected;
    } t[5] = {
        {"zz_zz;", "zz_zz"},
        {"a ;", "a"},
        {"bbb ;", "bbb"},
        {"meme;", "meme"},
        {"foobar;", "foobar"}};

    printf("Testing IDENTIFIER expressions\n");

    for(i = 0; i < tc; i++) {
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

        id = (Identifier *) es->expression;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            IDENT, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_identifier_value(id));

        test_string_cmp("[Error: %i] Expected identifier value %s got %s\n",
            t[i].expected, id->value, i, &fail);
    }

    return print_test_result("IDENTIFIER", tc - fail, tc);
}


char * test_parsing_boolean_expression() {
    int i, tc = 2, fail = 0;
    ExpressionStatement * es;
    Boolean * bo;
    struct {
        char * input, * type;
        bool value;
    } t[2] = {
        {"false;", FALSE, false},
        {"true;", TRUE, true}};

    printf("Testing BOOLEAN expressions\n");

    for(i = 0; i < tc; i++) {
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

        bo = (Boolean *) es->expression;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            t[i].type, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_boolean(bo));

        test_int_cmp("[Error: %i] Expected boolean value %i got %i\n",
            t[i].value, bo->value, i, &fail);
    }

    return print_test_result("BOOLEAN", tc - fail, tc);
}

char * test_integer_literal_expression() {
    int i, tc = 5, fail = 0;
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
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

        il = (IntegerLiteral *) es->expression;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            INT, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_integer_literal(il));

        test_int_cmp("[Error: %i] Expected integer value %i got %i\n",
            t[i].value, il->value, i, &fail);
    }

    return print_test_result("INTEGER", tc - fail, tc);
}

char * test_parsing_prefix_expressions() {
    int i, tc = 5, fail = 0;
    ExpressionStatement * es;
    PrefixExpression * pex;
    struct {
        char * input, * operator, * expression_type;
    } t[5] = {
        {"!5", "!", INT},
        {"!fn(x) { } ", "!", FUNCTION},
        {"-15", "-", INT},
        {"-var", "-", IDENT},
        {"!if(a) { } else { }", "!", IF}};

    printf("Testing PREFIX expressions\n");

    for(i = 0; i < tc; i++) {
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

        pex = (PrefixExpression *) es->expression;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            PREFIX, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_prefix_expression(pex));

        test_string_cmp("[Error: %i] Expected operator %s got %s\n",
            t[i].operator, pex->operator, i, &fail);
        test_string_cmp("[Error: %i] Expected right value %s got %s\n",
            t[i].expression_type, pex->expression_type, i, &fail);
    }

    return print_test_result("PREFIX", tc - fail, tc);
}

char * test_parsing_infix_expressions() {
    int i, tc = 5, fail = 0;
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
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

        iex = (InfixExpression *) es->expression;

        right_il = (IntegerLiteral *) iex->right;
        left_il = (IntegerLiteral *) iex->left;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            INFIX, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_infix_expression(iex));

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
    ExpressionStatement * es;
    FunctionLiteral * fl;
    BlockStatement * bs;
    struct {
        char * input;
        int param_count, statement_count;
    } t[5] = {
        {"fn(a) { let a = 4; };", 1, 1},
        {"fn(a, b) { x a };", 2, 2},
        {"fn(z) { !5 };", 1, 1},
        {"fn(a) { };", 1, 0},
        {"fn(a) { };", 1, 0}};

    printf("Testing FUNCTION LITERAL expressions\n");

    for(i = 0; i < tc; i++) {
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

        fl = (FunctionLiteral *) es->expression;
        bs = (BlockStatement *) fl->body;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            FUNCTION, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_function_literal(fl));

        test_int_cmp("[Error: %i] Expected parameter count %i got %i\n",
            t[i].param_count, fl->pc, i, &fail);
        test_int_cmp("[Error: %i] Expected statement count %i got %i\n",
            t[i].statement_count, bs->sc, i, &fail);
    }

    return print_test_result("FUNCTION LITERAL", tc - fail, tc);
}

char * test_parsing_call_expressions() {
    int i, tc = 5, fail = 0;
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
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

        cex = (CallExpression *) es->expression;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            CALL, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_call_expression(cex));

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
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

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
    }

    return print_test_result("GROUPED", tc - fail, tc);
}

char * test_parsing_if_expressions() {
    int i, tc = 5, fail = 0;
    char * cons_type, * alt_type;
    ExpressionStatement * es;
    IfExpression * ifx;
    BlockStatement * cons;
    BlockStatement * alt;
    struct {
        char * input, * condition_type, * cons_type, * alt_type;
    } t[5] = {
        {"if (x) { let a = 3; } else { return 1; }", IDENT, INT, INT},
        {"if (x == 1) { let a = b; } else { 5 }", INFIX, IDENT, INT},
        {"if (x) { let a = b; } else { 5 }", IDENT, IDENT, INT},
        {"if (x) { let a = b; } else { if (z) {} else {} }", IDENT, IDENT, IF},
        {"if (a > 3) { !2 } else { false }", INFIX, PREFIX, FALSE}};

    printf("Testing IF expressions\n");

    for(i = 0; i < tc; i++) {
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

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

        test_string_cmp("[Error: %i] Expected condition type %s got %s\n",
            t[i].condition_type, ifx->condition_type, i, &fail);
        test_string_cmp("[Error: %i] Expected consequence type %s got %s\n",
            t[i].cons_type, cons_type, i, &fail);
        test_string_cmp("[Error: %i] Expected altnerative type %s got %s\n",
            t[i].alt_type, alt_type, i, &fail);
    }

    return print_test_result("IF", tc - fail, tc);
}

char * test_parsing_array_literal() {
    int i, tc = 2, fail = 0;
    char * cons_type, * alt_type;
    ExpressionStatement * es;
    ArrayLiteral * al;
    Array * ar;
    ExpressionStatement * first, * last;
    struct {
        char * input;
        int c;
        char * first, * last;
    } t[2] = {
        {"[1,2,3,4];", 4, INT, INT},
        {"[\"a\"];", 1, STRING, STRING}};

    printf("Testing ARRAY literals\n");

    for(i = 0; i < tc; i++) {
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

        al = (ArrayLiteral *) es->expression;
        ar = al->elements;
        first = (ExpressionStatement *) ar->array[0];
        last = (ExpressionStatement *) ar->array[ar->size-1];

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            ARRAY, es->expression_type, i, &fail)) {
            continue;
        }

        printf("[%i] %i\n", i, ar->size);

        test_int_cmp("[Error: %i] Expected array count %i got %i\n",
            t[i].c, ar->size, i, &fail);
        test_string_cmp("[Error: %i] Expected first type %s got %s\n",
            t[i].first, first->expression_type, i, &fail);
        test_string_cmp("[Error: %i] Expected last type %s got %s\n",
            t[i].last, last->expression_type, i, &fail);
    }

    return print_test_result("ARRAY", tc - fail, tc);
}

char * test_parsing_array_index() {
    int i, tc = 1, fail = 0;
    char * cons_type, * alt_type;
    ExpressionStatement * es;
    IndexExpression * ae;
    char * right;
    struct {
        char * input, * right;
    } t[1] = {
        {"a[2];", INT}};

    printf("Testing ARRAY INDEX expression\n");

    for(i = 0; i < tc; i++) {
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

        ae = (IndexExpression *) es->expression;
        right = ae->index_expression_type;

        if(!test_string_cmp("[Error: %i] Expected type %s got %s\n",
            ARRAYIDX, es->expression_type, i, &fail)) {
            continue;
        }

        test_string_cmp("[Error: %i] Expected index type %s got %s\n",
            t[i].right, right, i, &fail);
    }

    return print_test_result("ARRAY INDEX", tc - fail, tc);
}

char * test_parsing_hash_map_literal() {
    int i, tc = 1, fail = 0;
    char * cons_type, * alt_type;
    ExpressionStatement * es;
    HashLiteral * hl;
    HashMap * hm;
    char * left, * right;
    void * v;
    IntegerLiteral * r;
    SortedList * sl;
    struct {
        char * input, * left, * right;
    } t[1] = {
        {"{\"a\": 33, \"b\": \"z\"}", "a", INT}};

    printf("Testing HASH MAP LITERAL expression\n");

    for(i = 0; i < tc; i++) {
        if(!get_parse_test_expression(&es, t[i].input)) {
            fail++;
            continue;
        }

        hl = (HashLiteral *) es->expression;
        hm = hl->pairs;

        sl = hash_map_find(hm, t[i].left);
        es = sl->data;
        r = es->expression;

        test_string_cmp("[Error: %i] Expected map value type %s got %s\n",
            t[i].right, es->expression_type, i, &fail);
    }

    return print_test_result("HASH MAP LITERAL", tc - fail, tc);
}

