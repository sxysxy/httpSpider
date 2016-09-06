#ifndef BASEDEF_H
#define BASEDEF_H
#include <string.h>
#ifndef __cplusplus
typedef enum boolType
{
    false,
    true
}bool;
#endif

typedef struct ansiStringType
{
    char *buffer;
    int length;
    bool freeable;
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
    str -> freeable = true;
}
void destroyAnsiString(ansiString *str)
{
    if(str -> freeable)
        free(str -> buffer);
}
void destroyAnsiStringPtr(ansiString **str)
{
    destroyAnsiString(*str);
    free(*str);
    *str = NULL;
}

#endif