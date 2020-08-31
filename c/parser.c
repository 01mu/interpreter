/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 */

void * parse_call_expression(Parser * par, char * type, void * function) {
    CallExpression * call = malloc(sizeof(CallExpression));

    call->token = par->current_token;
    call->function_type = type;
    call->function = function;
    call->arguments = parse_call_arguments(par, &call->ac);

    return call;
}

ExpressionStatement ** parse_call_arguments(Parser * par, int * ac) {
    ExpressionStatement ** args = malloc(sizeof(ExpressionStatement *));
    int sz = 0;

    ExpressionStatement * e;

    if(peek_token_is(par, RPAREN)) {
        * ac = sz;
        parser_next_token(par);
        return args;
    }

    parser_next_token(par);

    args = realloc(args, sizeof(ExpressionStatement *) * (sz + 1));
    args[sz] = malloc(sizeof(ExpressionStatement));
    e = (ExpressionStatement *) args[sz++];
    e->expression = parse_expression(par, PRE_LOWEST, e, PE_EXPRESSION);

    while(peek_token_is(par, COMMA)) {
        parser_next_token(par);
        parser_next_token(par);

        args = realloc(args, sizeof(ExpressionStatement *) * (sz + 1));
        args[sz] = malloc(sizeof(ExpressionStatement));
        e = (ExpressionStatement *) args[sz++];
        e->expression = parse_expression(par, PRE_LOWEST, e, PE_EXPRESSION);
    }

    if(!expect_peek(par, RPAREN)) {
        * ac = sz;
        return NULL;
    }

    * ac = sz;

    return args;
}

void * parse_expression(Parser * par, int precedence, void * ex, int et) {
    char * type = par->current_token.type;

    char * exp_type;
    void * expr;

    int type_len;

    if(type == IDENT) {
        exp_type = IDENT;
        expr = parse_identifier(par);
    } else if(type == INT) {
        exp_type = INT;
        expr = parse_integer_literal(par);
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
    } else {
        exp_type = "";
        expr = NULL;
    }

    while(!peek_token_is(par, SEMICOLON) &&
        precedence < parser_get_precedence(par, PREC_PEEK)) {

        type = par->peek_token.type;
        parser_next_token(par);

        if(type == PLUS || type == MINUS || type == SLASH ||
            type == ASTERISK || type == EQ || type == NOT_EQ ||
            type == LT || type == GT) {

            expr = parse_infix_expression(par, expr, exp_type);
            exp_type = INFIX;
        } else if(type == LPAREN) {
            expr = parse_call_expression(par, exp_type, expr);
            exp_type = CALL;
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
        }
    }

    return expr;
}

void parse_let_statement(Parser * par, Statement * smt) {
    LetStatement * let;
    ExpressionStatement * ex;

    smt->st = malloc(sizeof(LetStatement));
    let = (LetStatement *) smt->st;
    let->token = par->current_token;

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

    while(!cur_token_is(par, SEMICOLON)) {
        parser_next_token(par);
    }
}

void parse_return_statement(Parser * par, Statement * smt) {
    ReturnStatement * ret;
    ExpressionStatement  * ex;

    smt->st = malloc(sizeof(ReturnStatement));
    ret = (ReturnStatement *) smt->st;

    ret->token = par->current_token;

    parser_next_token(par);

    ret->value = malloc(sizeof(ExpressionStatement));
    ex = (ExpressionStatement *) ret->value;
    ex->expression = parse_expression(par, PRE_LOWEST, ex, PE_EXPRESSION);

    ret->type = ex->expression_type;

    while(!cur_token_is(par, SEMICOLON)) {
        parser_next_token(par);
    }
}

void * parse_identifier(Parser * par) {
    Identifier * identifier = malloc(sizeof(Identifier));

    identifier->token = par->current_token;
    identifier->value = par->current_token.literal;

    return identifier;
}

void * parse_integer_literal(Parser * par) {
    IntegerLiteral * lit = malloc(sizeof(IntegerLiteral));

    lit->token = par->current_token;
    lit->value = str_to_int(par->current_token.literal);

    return lit;
}

int parser_get_precedence(Parser * par, int type) {
    char * pt;

    if(type == PREC_PEEK)
        pt = par->peek_token.type;
    else if(type == PREC_CURRENT)
        pt = par->current_token.type;

    if(pt == EQ || pt == NOT_EQ) {
        return PRE_EQUALS;
    } else if(pt == LT || pt == GT) {
        return PRE_LESSGREATER;
    } else if(pt == PLUS || pt == MINUS) {
        return PRE_SUM;
    } else if(pt == SLASH || pt == ASTERISK) {
        return PRE_PRODUCT;
    } else if(pt == LPAREN) {
        return PRE_CALL;
    }

    return PRE_LOWEST;
}

void * parse_prefix_expression(Parser * par) {
    PrefixExpression * pe = malloc(sizeof(PrefixExpression));

    pe->token = par->current_token;
    pe->operator = par->current_token.literal;

    parser_next_token(par);

    pe->right = parse_expression(par, PRE_PREFIX, pe, PE_PREFIX);

    return pe;
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

void * parse_boolean(Parser * par, bool value) {
    Boolean * bool_ = malloc(sizeof(Boolean));

    bool_->token = par->current_token;
    bool_->value = value;

    return bool_;
}

void * parse_grouped_expression(Parser * par) {
    void * s;

    parser_next_token(par);

    s = parse_expression(par, PRE_LOWEST, NULL, 0);

    if(!expect_peek(par, RPAREN)) {
        free(s);
        return NULL;
    }

    return s;
}

void parse_expression_statement(Parser * par, Statement * smt) {
    ExpressionStatement * est;

    smt->st = malloc(sizeof(ExpressionStatement));
    est = (ExpressionStatement *) smt->st;

    est->token = par->current_token;
    est->expression = parse_expression(par, PRE_LOWEST, est, PE_EXPRESSION);

    if(peek_token_is(par, SEMICOLON)) {
        parser_next_token(par);
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

void * parse_if_expression(Parser * par) {
    IfExpression * iex = malloc(sizeof(IfExpression));
    iex->token = par->current_token;
    iex->else_token.literal = NULL;

    if(!expect_peek(par, LPAREN)) {
        return NULL;
    }

    parser_next_token(par);

    iex->condition = parse_expression(par, PRE_LOWEST, iex, PE_CONDITION);

    if(!expect_peek(par, RPAREN)) {
        return NULL;
    }

    if(!expect_peek(par, LBRACE)) {
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

BlockStatement * parse_block_statement(Parser * parser) {
    BlockStatement * bs = malloc(sizeof(BlockStatement));
    int sz = 0;

    bs->statements = malloc(sizeof(Statement));
    bs->sc = 0;

    parser_next_token(parser);

    while(!cur_token_is(parser, RBRACE) && !cur_token_is(parser, EOF_)) {
        sz = sizeof(Statement) * (bs->sc + 1);
        bs->statements = realloc(bs->statements, sz);

        parse_statement(parser, bs->statements, bs->sc++, sz);
        parser_next_token(parser);
    }

    return bs;
}

Program * parse_program(Parser * parser) {
    Program * prg = malloc(sizeof(Program));
    int sz = 0;

    prg->statements = malloc(sizeof(Statement));
    prg->sc = 0;

    while(!cur_token_is(parser, EOF_)) {
        sz = sizeof(Statement) * (prg->sc + 1);
        prg->statements = realloc(prg->statements, sz);

        parse_statement(parser, prg->statements, prg->sc++, sz);
        parser_next_token(parser);
    }

    return prg;
}

Identifier ** parse_program_parameters(Parser * parser, int * c) {
    Identifier ** parameters = malloc(sizeof(Identifier *));
    int pc = 0;

    if(peek_token_is(parser, RPAREN)) {
        * c = pc;
        parser_next_token(parser);
        return parameters;
    }

    parser_next_token(parser);

    Identifier * parameter = malloc(sizeof(Identifier));

    parameter->token = parser->current_token;
    parameter->value = parser->current_token.literal;

    parameters[pc++] = parameter;
    parameters = realloc(parameters, sizeof(Identifier *) * (pc + 1));

    while(peek_token_is(parser, COMMA)) {
        parser_next_token(parser);
        parser_next_token(parser);

        Identifier * parameter = malloc(sizeof(Identifier));

        parameter->token = parser->current_token;
        parameter->value = parser->current_token.literal;

        parameters[pc++] = parameter;
        parameters = realloc(parameters, sizeof(Identifier *) * (pc + 1));
    }

    if(!expect_peek(parser, RPAREN)) {
        * c = pc;
        return NULL;
    }

    * c = pc;

    return parameters;
}

void * parse_function_literal(Parser * parser) {
    FunctionLiteral * fl = malloc(sizeof(FunctionLiteral));

    fl->token = parser->current_token;

    if(!expect_peek(parser, LPAREN)) {
        return NULL;
    }

    fl->parameters = parse_program_parameters(parser, &fl->pc);

    if(!expect_peek(parser, LBRACE)) {
        return NULL;
    }

    fl->body = parse_block_statement(parser);

    return fl;
}
