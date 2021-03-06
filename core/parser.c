/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/parser.c
 *
 */

void * parse_hash_literal(Parser * par) {
    HashLiteral * hl = malloc(sizeof(HashLiteral));
    hl->pairs = hash_map_new(26);

    while(!peek_token_is(par, RBRACE)) {
        parser_next_token(par);

        ExpressionStatement * key = malloc(sizeof(ExpressionStatement));
        key->expression = parse_expression(par, PRE_LOWEST, key, PE_EXPRESSION);

        if(!expect_peek(par, COLON)) {
            return NULL;
        }

        parser_next_token(par);

        ExpressionStatement * exp = malloc(sizeof(ExpressionStatement));
        exp->expression = parse_expression(par, PRE_LOWEST, exp, PE_EXPRESSION);

        if(strcmp(key->expression_type, STRING) != 0) {
            return NULL;
        }

        StringLiteral * sl = key->expression;
        String * s = sl->value;

        char * a = malloc(strlen(s->string) + 1);
        strcpy(a, s->string);

        free_string_literal(sl);
        free(key->expression_type);
        free(key);

        hash_map_insert(hl->pairs, a, exp);

        if(!peek_token_is(par, RBRACE) && !expect_peek(par, COMMA)) {
            return NULL;
        }
    }

    if(!expect_peek(par, RBRACE)) {
        return NULL;
    }

    return hl;
}

void function_literal_store_add(FunctionLiteral * fl) {
    fls->store = realloc(fls->store, sizeof(FunctionLiteral *) *
        (fls->count + 1));
    fls->store[fls->count++] = fl;
}

void * parse_index_expression(Parser * par, void * left, char * left_type) {
    IndexExpression * ie = malloc(sizeof(IndexExpression));
    ExpressionStatement * e = NULL;

    ie->token = par->current_token;
    ie->left_expression_type = left_type;
    ie->left = left;

    parser_next_token(par);

    e = malloc(sizeof(ExpressionStatement));
    e->expression = parse_expression(par, PRE_LOWEST, ie, PE_ARRAYIDX);

    ie->index = e;

    if(!expect_peek(par, RBRACKET)) {
        return NULL;
    }

    return ie;
}

Array * parse_expression_list(Parser * par, char * end) {
    Array * array = array_new();
    ExpressionStatement * e = NULL;

    if(peek_token_is(par, end)) {
        return array;
    }

    parser_next_token(par);

    e = malloc(sizeof(ExpressionStatement));
    e->expression = parse_expression(par, PRE_LOWEST, e, PE_EXPRESSION);
    array_insert(array, e);

    while(peek_token_is(par, COMMA)) {
        parser_next_token(par);
        parser_next_token(par);

        e = malloc(sizeof(ExpressionStatement));
        e->expression = parse_expression(par, PRE_LOWEST, e, PE_EXPRESSION);
        array_insert(array, e);
    }

    if(!expect_peek(par, end)) {
        return NULL;
    }

    return array;
}

void * parse_array_literal(Parser * parser) {
    ArrayLiteral * al = malloc(sizeof(ArrayLiteral));

    al->token = parser->current_token;
    al->elements = parse_expression_list(parser, RBRACKET);

    return al;
}

ExpressionStatement ** parse_call_arguments(Parser * par, int * ac) {
    int i, c = 0;
    ExpressionStatement ** args = malloc(sizeof(ExpressionStatement *));
    ExpressionStatement * e = NULL;

    if(peek_token_is(par, RPAREN)) {
        free(args);
        * ac = 0;
        parser_next_token(par);
        return args;
    }

    parser_next_token(par);

    args = realloc(args, sizeof(ExpressionStatement *) * (c + 1));
    args[c] = malloc(sizeof(ExpressionStatement));
    e = (ExpressionStatement *) args[c++];
    e->expression = parse_expression(par, PRE_LOWEST, e, PE_EXPRESSION);

    while(peek_token_is(par, COMMA)) {
        parser_next_token(par);
        parser_next_token(par);

        args = realloc(args, sizeof(ExpressionStatement *) * (c + 1));
        args[c] = malloc(sizeof(ExpressionStatement));
        e = (ExpressionStatement *) args[c++];
        e->expression = parse_expression(par, PRE_LOWEST, e, PE_EXPRESSION);
    }

    if(!expect_peek(par, RPAREN)) {
        for(i = 0; i < * ac; i++) {
            free(args[i]->token.literal);
            free(args[i]);
        }

        free(args);
        * ac = 0;
        return NULL;
    }

    * ac = c;

    return args;
}

void * parse_call_expression(Parser * par, char * type, void * function) {
    CallExpression * call = malloc(sizeof(CallExpression));

    call->token = par->current_token;
    call->function_type = type;
    call->function = function;
    call->arguments = parse_call_arguments(par, &call->ac);

    return call;
}

BlockStatement * parse_block_statement(Parser * parser) {
    BlockStatement * bs = malloc(sizeof(BlockStatement));
    size_t stmt_sz = 0;

    bs->statements = malloc(sizeof(Statement));
    bs->sc = 0;

    parser_next_token(parser);

    while(!cur_token_is(parser, RBRACE) && !cur_token_is(parser, EOF_)) {
        stmt_sz = sizeof(Statement) * (bs->sc + 1);
        bs->statements = realloc(bs->statements, stmt_sz);

        parse_statement(parser, bs->statements, bs->sc++, stmt_sz);
        parser_next_token(parser);
    }

    return bs;
}

void * parse_if_expression(Parser * par) {
    IfExpression * iex = malloc(sizeof(IfExpression));

    iex->token = par->current_token;
    iex->else_token.literal = NULL;

    if(!expect_peek(par, LPAREN)) {
        free(iex);
        free(iex->token.literal);
        return NULL;
    }

    parser_next_token(par);

    iex->condition = parse_expression(par, PRE_LOWEST, iex, PE_CONDITION);

    if(!expect_peek(par, RPAREN)) {
        free(iex);
        free(iex->token.literal);
        return NULL;
    }

    if(!expect_peek(par, LBRACE)) {
        free(iex);
        free(iex->token.literal);
        return NULL;
    }

    iex->consequence = parse_block_statement(par);

    if(peek_token_is(par, ELSE)) {
        parser_next_token(par);
        iex->else_token = par->current_token;

        if(!expect_peek(par, LBRACE)) {
            return NULL;
        }

        iex->alternative = parse_block_statement(par);
    }

    return iex;
}

Identifier ** parse_program_parameters(Parser * parser, int * c) {
    int i, pc = 0;
    Identifier ** parameters = NULL, * parameter = NULL;

    if(peek_token_is(parser, RPAREN)) {
        * c = 0;
        parser_next_token(parser);
        return parameters;
    }

    parser_next_token(parser);

    parameters = malloc(sizeof(Identifier *));

    parameter = malloc(sizeof(Identifier));
    parameter->token = parser->current_token;
    parameter->value = parser->current_token.literal;
    parameters[pc++] = parameter;
    parameters = realloc(parameters, sizeof(Identifier *) * (pc + 1));

    while(peek_token_is(parser, COMMA)) {
        parser_next_token(parser);
        parser_next_token(parser);

        parameter = malloc(sizeof(Identifier));
        parameter->token = parser->current_token;
        parameter->value = parser->current_token.literal;
        parameters[pc++] = parameter;
        parameters = realloc(parameters, sizeof(Identifier *) * (pc + 1));
    }

    if(!expect_peek(parser, RPAREN)) {
        for(i = 0; i < pc; i++) {
            free(parameters[i]->token.literal);
            free(parameters[i]);
        }

        * c = 0;
        free(parameters);
        return NULL;
    }

    * c = pc;

    return parameters;
}

void * parse_function_literal(Parser * parser) {
    int i;
    FunctionLiteral * fl = malloc(sizeof(FunctionLiteral));

    fl->token = parser->current_token;

    if(!expect_peek(parser, LPAREN)) {
        free(fl);
        free(fl->token.literal);
        return NULL;
    }

    fl->parameters = parse_program_parameters(parser, &fl->pc);

    if(!expect_peek(parser, LBRACE)) {
        for(i = 0; i < fl->pc; i++) {
            free(fl->parameters[i]->token.literal);
            free(fl->parameters[i]);
        }

        free(fl->token.literal);
        free(fl);
        return NULL;
    }

    fl->body = parse_block_statement(parser);

    return fl;
}

void * parse_grouped_expression(Parser * par) {
    void * s = NULL;

    parser_next_token(par);

    s = parse_expression(par, PRE_LOWEST, NULL, 0);

    if(!expect_peek(par, RPAREN)) {
        free(s);
        return NULL;
    }

    return s;
}

void * parse_infix_expression(Parser * par, void * left, char * left_type) {
    InfixExpression * ie = malloc(sizeof(InfixExpression));
    int precedence = parser_get_precedence(par, PREC_CURRENT);

    ie->token = par->current_token;
    ie->operator = par->current_token.literal;
    ie->left_expression_type = left_type;
    ie->left = left;

    parser_next_token(par);

    ie->right = parse_expression(par, precedence, ie, PE_INFIX);

    return ie;
}

void * parse_prefix_expression(Parser * par) {
    PrefixExpression * pe = malloc(sizeof(PrefixExpression));

    pe->token = par->current_token;
    pe->operator = par->current_token.literal;

    parser_next_token(par);

    pe->right = parse_expression(par, PRE_PREFIX, pe, PE_PREFIX);

    return pe;
}

void * parse_identifier(Parser * par) {
    Identifier * identifier = malloc(sizeof(Identifier));

    identifier->token = par->current_token;
    identifier->value = par->current_token.literal;

    return identifier;
}

void * parse_boolean(Parser * par, bool value) {
    Boolean * b = malloc(sizeof(Boolean));

    b->token = par->current_token;
    b->value = value;

    return b;
}

void * parse_string_literal(Parser * par) {
    StringLiteral * str = malloc(sizeof(StringLiteral));
    String * st = string_new();

    string_cat(st, par->current_token.literal, 1);

    str->token = par->current_token;
    str->value = st;

    return str;
}

void * parse_integer_literal(Parser * par) {
    IntegerLiteral * lit = malloc(sizeof(IntegerLiteral));

    lit->token = par->current_token;
    lit->value = str_to_int(par->current_token.literal);

    return lit;
}

void * parse_expression(Parser * par, int precedence, void * ex, int et) {
    char * type = par->current_token.type, * exp_type = NULL;
    void * expr = NULL;
    size_t type_len;

    if(type == IDENT) {
        exp_type = IDENT;
        expr = parse_identifier(par);
    } else if(type == INT) {
        exp_type = INT;
        expr = parse_integer_literal(par);
    } else if(type == STRING) {
        exp_type = STRING;
        expr = parse_string_literal(par);
    } else if(type == TRUE) {
        exp_type = TRUE;
        expr = parse_boolean(par, true);
    } else if(type == FALSE) {
        exp_type = FALSE;
        expr = parse_boolean(par, false);
    } else if(type == BANG || type == MINUS) {
        exp_type = PREFIX;
        expr = parse_prefix_expression(par);
    } else if(type == LPAREN) {
        exp_type = INFIX;
        expr = parse_grouped_expression(par);
    } else if(type == IF) {
        exp_type = IF;
        expr = parse_if_expression(par);
    } else if(type == FUNCTION) {
        exp_type = FUNCTION;
        expr = parse_function_literal(par);
        function_literal_store_add(expr);
    } else if(type == LBRACKET) {
        exp_type = ARRAY;
        expr = parse_array_literal(par);
    } else if(type == LBRACE) {
        expr = parse_hash_literal(par);
        exp_type = HASHMAP;
    } else {
        exp_type = ILLEGAL;
        expr = NULL;
        error_invalid_syntax(par, par->peek_token);
    }

    while(!peek_token_is(par, SEMICOLON) &&
        precedence < parser_get_precedence(par, PREC_PEEK)) {

        type = par->peek_token.type;
        parser_next_token(par);

        if(type == PLUS || type == MINUS || type == SLASH || type == ASTERISK ||
            type == EQ || type == NOT_EQ || type == LT || type == GT ||
            type == CONDAND || type == CONDOR ||
            type == LTE || type == GTE || type == ASSIGN) {

            expr = parse_infix_expression(par, expr, exp_type);
            exp_type = INFIX;
        } else if(type == LPAREN) {
            expr = parse_call_expression(par, exp_type, expr);
            exp_type = CALL;
        } else if(type == LBRACKET) {
            expr = parse_index_expression(par, expr, exp_type);
            exp_type = ARRAYIDX;
        } else {
            exp_type = ILLEGAL;
            expr = NULL;
        }
    }

    if(ex != NULL) {
        type_len = strlen(exp_type) + 2;

        if(et == PE_EXPRESSION) {
            ((ExpressionStatement *) ex)->expression_type = malloc(type_len);
            strcpy(((ExpressionStatement *) ex)->expression_type, exp_type);
        } else if(et == PE_PREFIX){
            ((PrefixExpression *) ex)->expression_type = malloc(type_len);
            strcpy(((PrefixExpression *) ex)->expression_type, exp_type);
        } else if(et == PE_INFIX){
            ((InfixExpression *) ex)->right_expression_type = malloc(type_len);
            strcpy(((InfixExpression *) ex)->right_expression_type, exp_type);
        } else if(et == PE_CONDITION){
            ((IfExpression *) ex)->condition_type = malloc(type_len);
            strcpy(((IfExpression *) ex)->condition_type, exp_type);
        } else if(et == PE_ARRAYIDX) {
            ((IndexExpression *) ex)->index_expression_type = malloc(type_len);
            strcpy(((IndexExpression *) ex)->index_expression_type, exp_type);
        }
    }

    return expr;
}

void parse_expression_statement(Parser * par, Statement * smt) {
    ExpressionStatement * est = NULL;

    smt->st = malloc(sizeof(ExpressionStatement));
    est = (ExpressionStatement *) smt->st;

    est->token = par->current_token;
    est->expression = parse_expression(par, PRE_LOWEST, est, PE_EXPRESSION);

    if(peek_token_is(par, SEMICOLON)) {
        parser_next_token(par);
    }
}

void parse_return_statement(Parser * par, Statement * smt) {
    ReturnStatement * ret = NULL;
    ExpressionStatement * ex = NULL;

    smt->st = malloc(sizeof(ReturnStatement));
    ret = (ReturnStatement *) smt->st;

    ret->token = par->current_token;

    parser_next_token(par);

    ret->value = malloc(sizeof(ExpressionStatement));
    ex = (ExpressionStatement *) ret->value;
    ex->expression = parse_expression(par, PRE_LOWEST, ex, PE_EXPRESSION);

    ret->type = ex->expression_type;

    if(!expect_peek(par, SEMICOLON)) {
        return;
    }
}

void parse_let_statement(Parser * par, Statement * smt) {
    LetStatement * let = NULL;
    ExpressionStatement * ex = NULL;

    smt->st = malloc(sizeof(LetStatement));
    let = (LetStatement *) smt->st;
    let->token = par->current_token;
    let->type = NULL;
    let->value = NULL;
    let->name.value = NULL;

    if(!expect_peek(par, IDENT)) {
        return;
    }

    let->name.token = par->current_token;
    let->name.value = par->current_token.literal;

    if(!expect_peek(par, ASSIGN)) {
        return;
    }

    parser_next_token(par);

    let->value = malloc(sizeof(ExpressionStatement));
    ex = (ExpressionStatement *) let->value;
    ex->expression = parse_expression(par, PRE_LOWEST, ex, PE_EXPRESSION);

    let->type = ex->expression_type;

    if(!expect_peek(par, SEMICOLON)) {
        return;
    }
}

void parse_statement(Parser * par, Statement * stmts, int sc, int sz) {
    char * token_type = par->current_token.type;

    if(strcmp(token_type, LET) == 0) {
        stmts[sc].type = LET;
        parse_let_statement(par, &stmts[sc]);
    } else if(strcmp(token_type, RETURN) == 0) {
        stmts[sc].type = RETURN;
        parse_return_statement(par, &stmts[sc]);
    } else {
        stmts[sc].type = EXPRESSION;
        parse_expression_statement(par, &stmts[sc]);
    }
}

Program * parse_program(Parser * parser) {
    Program * prg = malloc(sizeof(Program));
    size_t stmt_sz;

    prg->statements = malloc(sizeof(Statement));
    prg->sc = 0;

    while(!cur_token_is(parser, EOF_)) {
        stmt_sz = sizeof(Statement) * (prg->sc + 1);
        prg->statements = realloc(prg->statements, stmt_sz);
        parse_statement(parser, prg->statements, prg->sc++, stmt_sz);
        parser_next_token(parser);
    }

    return prg;
}

int parser_get_precedence(Parser * par, int type) {
    char * pt = NULL;

    if(type == PREC_PEEK) {
        pt = par->peek_token.type;
    } else if(type == PREC_CURRENT) {
        pt = par->current_token.type;
    }

    if(pt == EQ || pt == NOT_EQ || pt == ASSIGN || pt == CONDOR || pt == CONDAND) {
        return PRE_EQUALS;
    } else if(pt == LT || pt == GT || pt == LTE || pt == GTE) {
        return PRE_LESSGREATER;
    } else if(pt == PLUS || pt == MINUS) {
        return PRE_SUM;
    } else if(pt == SLASH || pt == ASTERISK) {
        return PRE_PRODUCT;
    } else if(pt == LPAREN) {
        return PRE_CALL;
    } else if(pt == LBRACKET) {
        return PRE_INDEX;
    } else {
        return PRE_LOWEST;
    }
}
