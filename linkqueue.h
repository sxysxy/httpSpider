#ifndef LINKQUEUE_H
#define LINKQUEUE_H
#include "basedef.h"
typedef struct linkType
{
    struct linkType *next;
    elemType data;
}link;

typedef struct linkQueueType
{
    link *head, *tail;
    int size;
}linkQueue;

void initQueue(linkQueue *q)
{
    q -> size = 0;
    if(!q -> head)q -> head = (link *)malloc(sizeof(link));
    q -> tail = q -> head;
}
void pushQueue(linkQueue *q, elemType s)
{
    if(!q -> size)
        initQueue(q);
    link *ln = (link *)malloc(sizeof(link));
    ln -> next = NULL;
    q -> tail -> next = ln;
    q -> tail = ln;
    ln -> data = s;
    q -> size++;
}
void destroyQueue(linkQueue *q)
{
    link *h = q -> head;
    if(!h)
        return;
    while(h -> next)
    {
        link *n = h -> next;
        free(h);
        h = n;
    }
    q -> head = q -> tail = NULL;
    q -> size = 0;
}
elemType popQueue(linkQueue *q)
{
    if(q -> size > 0)
    {
        link *h = q -> head -> next;
        elemType r = h -> data;
        q -> head -> next = h -> next;
        q -> size--;
        free(h);
        return r;
    }
}
 
#endif