#ifndef HTTPSPIDER_H
#define HTTPSPIDER_H
#include "basedef.h"
#include "trie.h"

#define HOST_MAXLEN 40
typedef void (*analyzerType) (char *b, int sz);
typedef struct spiderType
{
    char host[HOST_MAXLEN];   //事实上准确的说是用户给的host....
    int port;
    long ip;                 //ip
    analyzerType analyzer;
    trie slot;
}spider;
 
#define PLUG_MAXLEN 80
typedef struct spiderPlugType
{
    char plug[PLUG_MAXLEN];
    char func[PLUG_MAXLEN];
    bool attached;
    long nativePointer;
}spiderPlug;

#endif