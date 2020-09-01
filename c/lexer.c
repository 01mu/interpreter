/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * lexer.c
 *
 */

Token new_token(char * type) {
    Token token;

    token.type = type;
    token.literal = type;

    return token;
}

void read_char(Lexer * lexer) {
    if(lexer->read_pos >= strlen(lexer->input)) {
        lexer->ch = 0;
    } else {
        lexer->ch = lexer->input[lexer->read_pos];
    }

    lexer->pos = lexer->read_pos;
    lexer->read_pos += 1;
}

int str_to_int(char * s) {
    int r = 0;
    int p = 1;

    while(* s == '-' || * s == '+') {
        if(* s++ == '-') {
            p *= -1;
        }
    }

    while(* s >= '0' && * s <= '9') {
        r = (r * 10) + (* s++ - '0');
    }

    return r * p;
}

Lexer * new_lexer(const char * input) {
    Lexer * lexer = malloc(sizeof(Lexer));

    lexer->input = input;
    lexer->pos = 0;
    lexer->read_pos = 0;

    read_char(lexer);

    return lexer;
}

char * get_substr(int position, int final_pos, const char * input) {
    int i;
    int size = final_pos - position;

    char * buff = malloc(size + 1);

    for(i = 0; position < final_pos; position++, i++) {
        buff[i] = input[position];
    }

    buff[size] = '\0';

    return buff;
}

bool is_letter(char ch) {
    return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_';
}

bool is_digit(char ch) {
    return '0' <= ch && ch <= '9';
}

char * read_identifier(Lexer * lexer) {
    int position = lexer->pos;

    while(is_letter(lexer->ch)) {
        read_char(lexer);
    }

    char * sub = get_substr(position, lexer->pos, lexer->input);

    /*if(strcmp(sub, "if") == 0) {
        return "if";
    } else if(strcmp(sub, "else") == 0) {
        return "else";
    }*/

    return sub;
}

char * read_number(Lexer * lexer) {
    int position = lexer->pos;

    while(is_digit(lexer->ch)) {
        read_char(lexer);
    }

    return get_substr(position, lexer->pos, lexer->input);
}

char * lookup_ident(char * literal) {
    if(strcmp("fn", literal) == 0) {
        return FUNCTION;
    } else if(strcmp("let", literal) == 0) {
        return LET;
    } else if(strcmp("true", literal) == 0) {
        return TRUE;
    } else if(strcmp("false", literal) == 0) {
        return FALSE;
    } else if(strcmp("if", literal) == 0) {
        return IF;
    } else if(strcmp("else", literal) == 0) {
        return ELSE;
    } else if(strcmp("return", literal) == 0) {
        return RETURN;
    } else {
        return IDENT;
    }
}

void skip_whitespace(Lexer * lexer) {
    while(lexer->ch == ' ' || lexer-> ch == '\t' || lexer->ch == '\n' ||
        lexer->ch  == '\r') {

        read_char(lexer);
    }
}

char peek_char(Lexer * lexer) {
    if(lexer->read_pos >= strlen(lexer->input)) {
        return 0;
    } else {
        return lexer->input[lexer->read_pos];
    }
}

Token lexer_next_token(Lexer * lexer) {
    Token token;

    skip_whitespace(lexer);

    switch(lexer->ch) {
    case ';':
        token = new_token(SEMICOLON);
        break;
    case '(':
        token = new_token(LPAREN);
        break;
    case ')':
        token = new_token(RPAREN);
        break;
    case ',':
        token = new_token(COMMA);
        break;
    case '+':
        token = new_token(PLUS);
        break;
    case '{':
        token = new_token(LBRACE);
        break;
    case '}':
        token = new_token(RBRACE);
        break;
    case '-':
        token = new_token(MINUS);
        break;
    case '*':
        token = new_token(ASTERISK);
        break;
    case '/':
        token = new_token(SLASH);
        break;
    case '<':
        token = new_token(LT);
        break;
    case '>':
        token = new_token(GT);
        break;
    case '=':
        if(peek_char(lexer) == '=') {
            read_char(lexer);
            token.type = EQ;
            token.literal = "==";
        } else {
            token = new_token(ASSIGN);
        }
        break;
    case '!':
        if(peek_char(lexer) == '=') {
            read_char(lexer);
            token.type = NOT_EQ;
            token.literal = "!=";
        } else {
            token = new_token(BANG);
        }
        break;
    case 0:
        token.literal = "";
        token.type = EOF_;
        break;
    default:
        if(is_letter(lexer->ch)) {
            token.literal = read_identifier(lexer);
            token.type = lookup_ident(token.literal);
            return token;
        } else if(is_digit(lexer->ch)) {
            token.literal = read_number(lexer);
            token.type = INT;
            return token;
        } else {
            token = new_token(ILLEGAL);
        }
    };

    read_char(lexer);

    return token;
}

void parser_next_token(Parser * parser) {
    parser->current_token = parser->peek_token;
    parser->peek_token = lexer_next_token(parser->lexer);
}

Parser * new_parser(Lexer * lexer) {
    Parser * parser = malloc(sizeof(Parser));

    parser->lexer = lexer;
    parser->errors = malloc(sizeof(char *));
    parser->ec = 0;

    parser_next_token(parser);
    parser_next_token(parser);

    return parser;
}

bool cur_token_is(Parser * parser, char * type) {
    return strcmp(parser->current_token.type, type) == 0;
}

bool peek_token_is(Parser * parser, char * type) {
    return strcmp(parser->peek_token.type, type) == 0;
}

void peek_error(Parser * parser, char * type) {
    int ex_len = strlen(type);
    int gt_len = strlen(parser->peek_token.type);

    char * str = malloc(25 + ex_len + gt_len);

    sprintf(str, "ERROR: Expected %s got %s", type, parser->peek_token.type);

    parser->errors[parser->ec] = str;
    parser->errors[++parser->ec] = malloc(sizeof(char *));
}

bool expect_peek(Parser * parser, char * type) {
    if(peek_token_is(parser, type)) {
        parser_next_token(parser);
        return true;
    } else {
        peek_error(parser, type);
        return false;
    }
}

bool check_parser_errors(Parser * parser) {
    int i;

    if(parser->ec == 0) {
        return false;
    }

    if(parser->ec != 0) {
        for(i = 0; i < parser->ec; i++) {
            printf("%s\n", parser->errors[i]);
        }
    }

    return true;
}
