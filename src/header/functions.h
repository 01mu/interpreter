/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * functions.h
 *
 */

Token new_token(char * type);
void read_char(Lexer * lexer);
int str_to_int(char * str);
Lexer * new_lexer(const char * input);
char * get_substr(int position, int final_pos, const char * input);
bool is_letter(char ch);
bool is_digit(char ch);
char * read_identifier(Lexer * lexer);
char * read_number(Lexer * lexer);
char * lookup_ident(char * literal);
void skip_whitespace(Lexer * lexer);
char peek_char(Lexer * lexer);
Token lexer_next_token(Lexer * lexer);
void parser_next_token(Parser * parser);
Parser * new_parser(Lexer * lexer);
bool cur_token_is(Parser * parser, char * type);
bool peek_token_is(Parser * parser, char * type);
void peek_error(Parser * parser, char * type);
bool expect_peek(Parser * parser, char * type);
bool check_parser_errors(Parser * parser);

void parse_statement(Parser * par, Statement * stmts, int sc, int sz);
void * parse_call_expression(Parser * par, char * type, void * function);
ExpressionStatement ** parse_call_arguments(Parser * par, int * ac);
void * parse_expression(Parser * par, int precedence, void * ex, int et);
void parse_let_statement(Parser * par, Statement * smt);
void parse_return_statement(Parser * par, Statement * smt);
void * parse_identifier(Parser * par);
void * parse_integer_literal(Parser * par);
int parser_get_precedence(Parser * par, int type);
void * parse_prefix_expression(Parser * par);
void * parse_infix_expression(Parser * par, void * left, char * left_type);
void * parse_boolean(Parser * par, bool value);
void * parse_grouped_expression(Parser * par);
void parse_expression_statement(Parser * par, Statement * smt);
void * parse_if_expression(Parser * par);
BlockStatement * parse_block_statement(Parser * parser);
Program * parse_program(Parser * parser);
Identifier ** parse_program_parameters(Parser * parser, int * c);
void * parse_function_literal(Parser * parser);

char * read_file(char * file);
char * print_call_expression(CallExpression * ce);
char * print_function_literal(FunctionLiteral * fl);
char * print_block_statement(BlockStatement * bst);
char * print_if_expression(IfExpression * iex);
char * get_print_expression(char * type, void * expr);
char * print_let_statement(LetStatement * smt);
char * print_return_statement(ReturnStatement * smt);
char * print_integer_literal(IntegerLiteral * il);
char * print_boolean(Boolean * b);
char * print_identifier_value(Identifier * i);
char * print_prefix_expression(PrefixExpression * pe);
char * print_infix_expression(InfixExpression * ie);
char * get_print_statement_type(Statement stmt);
char * print_program(Program * program);
void test_print_program();

void free_integer_literal(IntegerLiteral * il);
void free_identifier(Identifier * id);
void free_prefix_expression(PrefixExpression * pex);
void free_infix_expression(InfixExpression * iex);
void free_block_statement(BlockStatement * bst);
void free_if_expression(IfExpression * ifx);
void free_function_literal(FunctionLiteral * fl);
void free_call_expression(CallExpression * call);
void free_expression_statement(char * type, void * value);
void free_let_statement(LetStatement * let);
void free_return_statement(ReturnStatement * ret);
void free_statement(Statement stmt);
void free_program(Lexer * lexer, Parser * parser, Program * program);

void test_parse_all();
char * test_get_statement_type(char * type, void * expr);
bool test_string_cmp(char * b, char * g, char * ex, int i, int * fail);
bool test_int_cmp(char * b, int g, int ex, int i, int * fail);
char * print_parse_test_result(char * b, int fail, int tc);
char * test_next_token();
char * test_let_statements();
char * test_return_statements();
char * test_identifier_expression();
char * test_integer_literal_expression();
char * test_parsing_prefix_expressions();
char * test_parsing_infix_expressions();
char * test_parsing_function_literal_expressions();
char * test_parsing_call_expressions();
char * test_parsing_grouped_expressions();
char * test_parsing_if_expressions();
