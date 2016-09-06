#ifndef HTTPSPIDER_H
#define HTTPSPIDER_H

//#include "avltree.h"

#ifndef __cplusplus
typedef enum boolType
{
    false,
    true
}bool;
#endif

//36277是一个质数
#define HASHMAX 36277     
//一个工具函数
//可能有宽字符，不要用0作为结束的判断。
int ELFHash(char *str, int len)
{
    unsigned h = 0, x = 0;
    int i = 0;
    while(i < len)
    {
        h = (h << 4) + (str[i++]);
        if((x = h & 0xF0000000L) != 0)
        {
            h ^= (x >> 24);
            h &= ~x;
        }
    }
    return (((int)h % HASHMAX)+HASHMAX)%HASHMAX;
}

#define HOST_MAXLEN 40
typedef void (*analyzerType) (char *b, int sz);
typedef struct spiderType
{
    char host[HOST_MAXLEN];
    int port;
    analyzerType analyzer;
    //AVLTree hashSlot[HASHMAX];
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