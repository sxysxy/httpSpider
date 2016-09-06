#ifndef BASEDEF_H
#define BASEDEF_H
#include <string.h>

typedef struct ansiStringType
{
    char *buffer;
    int length;
}ansiString;
typedef ansiString elemType;

void initAnsiString(ansiString *str, char *w)
{
    int l = strlen(w);
    str -> buffer = (char *)malloc(sizeof(char)* l + 10);
    str -> length = l;
    for(int i = 0; i < l; i++)
        str -> buffer[i] = w[i];
    str -> buffer[l] = 0;
}
void destroyAnsiString(ansiString *str)
{
    free(str -> buffer);
}
void destroyAnsiStringPtr(ansiString **str)
{
    free((*str) -> buffer);
    free(*str);
    *str = NULL;
}

#endif