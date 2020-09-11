/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * lib/string.c
 *
 */

typedef struct {
    char * string;
    int len;
} String;

String * string_new();
void string_cat(String * string, char * ap, bool h);
void string_append(String * string, String * ap);
void string_free(String * string);

String * string_new() {
    String * string = malloc(sizeof(String));

    string->len = 0;
    string->string = malloc(sizeof(char));
    string->string[0] = '\0';

    return string;
}

void string_cat(String * string, char * ap, bool h) {
    int len_ap = strlen(ap);

    string->string = realloc(string->string, string->len + len_ap + 1);
    strcat(string->string, ap);
    string->len += len_ap;

    if(h) {
        free(ap);
    }
}

void string_append(String * string, String * ap) {
    int len_ap = strlen(ap->string);

    string->string = realloc(string->string, string->len + len_ap + 1);
    strcat(string->string, ap->string);
    string->len += len_ap;
    string_free(ap);
}

void string_free(String * string) {
    free(string->string);
    free(string);
}
