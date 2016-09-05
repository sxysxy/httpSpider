#ifndef HTTPSPIDER_H
#define HTTPSPIDER_H

#define HOST_MAXLEN 40
typedef struct spiderType
{
    char host[HOST_MAXLEN];
    int port;
    void (*analyzer) (char *b, int sz);
}spider;

#endif