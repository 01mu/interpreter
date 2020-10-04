/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * types/lib.c
 *
 */

typedef struct {
    void ** array;
    int size;
} Array;

typedef struct {
    char * string;
    int len;
} String;

struct SortedList {
    struct SortedList * next;
    char * data_type;
    void * data;
    char * key;
};

typedef struct {
    int size;
    struct SortedList ** array;
} HashMap;

typedef struct SortedList SortedList;
