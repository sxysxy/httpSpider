#ifndef HTTPSPIDER_H
#define HTTPSPIDER_H

#ifndef __cplusplus
typedef enum boolType
{
    false,
    true
}bool;
#endif

#define HOST_MAXLEN 40
typedef void (*analyzerType) (char *b, int sz);
typedef struct spiderType
{
    char host[HOST_MAXLEN];
    int port;
    analyzerType analyzer;
}spider;

#define PLUG_MAXLEN 80
typedef struct spiderPlugType
{
    char plug[PLUG_MAXLEN];
    bool attached;
}spiderPlug;

#endif