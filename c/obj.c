/* A small object-oriented experiment */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
        char *data;
        size_t len;
        size_t capa;

        size_t (*strlen)(void *self);
        char *(*c_str)(void *self);
        void (*append)(void *self, const char *str);
        void (*destroy)(void *self);
        char (*get)(void *self, int idx);
} string;

static size_t string_len(void *self)
{
        string *str = (string *)self;
        return str->len;
}

static char *string_c_str(void *self)
{
        string *str = (string *)self;
        return str->data;
}

static void string_append(void *self, const char *s)
{
        string *str = (string *)self;
        size_t new_len = str->len + strlen(s);

        if (new_len + 1 > str->capa) {
                str->capa = (new_len + 1) * 2;
                str->data = realloc(str->data, str->capa);
        }

        strcat(str->data, s);
        str->len = new_len;
}

static char string_get(void *self, int idx)
{
        string *str = (string *)self;
        if (idx < 0)
                idx = str->len + idx;
        if (idx < 0 || idx >= (int)str->len)
                return '\0';

        return str->data[idx];
}


static void string_destroy(void *self)
{
        string *str = (string *)self;
        free(str->data);
        free(str);
}

string *buildstr(const char *init)
{
        string *s = malloc(sizeof(string));
        
        if (!s)
                return NULL;
        
        size_t init_len = init ? strlen(init) : 0;
        s->capa = (init_len + 1) * 2;
        s->data = malloc(s->capa);

        if (!s->data) {
                free(s);
                return NULL;
        }

        if (init) {
                strcpy(s->data, init);
                s->len = init_len;
        } else {
                s->data[0] = '\0';
                s->len = 0;
        }

        s->strlen  = string_len;
        s->c_str   = string_c_str;
        s->append  = string_append;
        s->get     = string_get;
        s->destroy = string_destroy;

        return s;
}

int main(void)
{
        string *s = buildstr("hello");

        printf("len: %zu\n", s->strlen(s));
        printf("c str: %s\n", s->c_str(s));
 
        s->append(s, " wrold!");
        printf("len: %zu\n", s->strlen(s));
        printf("c str: %s\n", s->c_str(s));

        s->destroy(s);

        return 0;
}
