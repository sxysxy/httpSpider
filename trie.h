#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "basedef.h"

#define NEXT_SIZE 16
typedef struct trieNodeType
{
    bool isFinal;
    struct trieNodeType *(*next);
}trieNode, *pTrieNode;

typedef struct trieType
{
    trieNode *root;
}trie;

void initTrie(trie *t)
{
    t -> root = (trieNode *)malloc(sizeof(trieNode));
    memset(t -> root, 0, sizeof(trieNode));
}
trieNode *appendChar(trieNode *d, int t)
{
    if(!d -> next)
    {
        d -> next = (pTrieNode *)malloc(sizeof(pTrieNode) * NEXT_SIZE);
        memset(d -> next, 0, sizeof(pTrieNode) * NEXT_SIZE);
    }
    if(!d -> next[t])
    {
        d -> next[t] = (pTrieNode)malloc(sizeof(trieNode));
        memset(d -> next[t], 0, sizeof(trieNode));
    }
    return d -> next[t];
}
void insertWord(trie *t, char *w, int len)
{
    trieNode *fw = t -> root;
    for(int i = 0; i < len; i++)
    {
        unsigned char c = w[i];
        unsigned hw = (c & (unsigned char)0xf0)>>4;   //高四位
        unsigned lw = c & (unsigned char)0x0f;        //低四位
        fw = appendChar(fw, hw);
        fw = appendChar(fw, lw);
    }
    fw -> isFinal = true;
}
bool existWord(trie *t, char *w, int len)
{
    trieNode *fw = t -> root;
    for(int i = 0; i < len; i++)
    {
        if(!fw -> next)
            return false;
        unsigned char c = w[i];
        unsigned hw = (c & (unsigned char)0xf0)>>4;
        unsigned lw = c & (unsigned char)0x0f;
        if(fw -> next[hw])
            fw = fw -> next[hw];
        else return false;
        if(fw -> next[lw])
            fw = fw -> next[lw];
        else return false;
        if(!fw)return false;
    }
    return fw -> isFinal;
}
void destroyTrieNodes(pTrieNode d)
{
    if(!d)return;
    if(!d -> next)return;
    for(int i = 0; i < NEXT_SIZE; i++)
        if(d -> next[i])destroyTrieNodes(d -> next[i]);
    for(int i = 0; i < NEXT_SIZE; i++)
        if(d -> next[i])free(d -> next[i]);
    free(d -> next);
}
void destroyTrie(trie *t)
{
    destroyTrieNodes(t -> root);
}

#endif