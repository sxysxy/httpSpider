#ifndef AVLTREE_H
#define AVLTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

//来自我(sxysxy)的博客 sxysxy.org/blogs/13
#include "basedef.h"
typedef ansiString elemType;
typedef struct _AVLNode
{
    struct _AVLNode *left, *right;
    elemType data;     //当前节点数据
    int height;   //树高，用作平衡因子
    int size;     //以当前节点为根的子树的节点数
}AVLNode, *pNode, *AVLTree;
#define HEIGHT(x) ((x)?(x)->height:-1)
#define SIZE(x) ((x)?(x)->size:0)
void maintain(pNode k)
{       //维护单个节点的信息
    if(!k)return;
    k -> height = max(HEIGHT(k->left), HEIGHT(k->right))+1;
    k -> size = 1 + SIZE(k -> left) + SIZE(k -> right);
}
/*             LL 向左孩子的左边插入数据时执行 K1是左孩子，K2是新插入的
                K0
               /                  K1
              K1    --K0 LL->    /  \ 
             /                  K2  K0
            K2
*/
pNode singleRotateLL(pNode k)
{
    pNode k1 = k -> left;
    k -> left = k1 -> right;
    k1 -> right = k;
    maintain(k);
    maintain(k1);
    return k1;
}

/*
                    RR 向右孩子的右边插入数据
             K0                     K1
               \                   /  \
               K1     --K0 RR->   k0  k2
                \
                K2
*/
pNode singleRotateRR(pNode k)
{
    pNode k1 = k -> right;
    k -> right = k1 -> left;
    k1 -> left = k;
    maintain(k);
    maintain(k1);
    return k1;
}

/*
                  LR 向左孩子的右边插入数据
            K0                K0                K1
           /                 /                 /  \
          K1    --K1 RR->   K1    --K0 LL->   K2  K0
           \               /
           K2             K2

*/
pNode doubleRotateLR(pNode k)
{
    k -> left = singleRotateRR(k -> left);
    return singleRotateLL(k);
}

/*
                   RL 向右孩子的左边插入数据
           K0               K0                   K1
             \               \                  /  \ 
             K1  --K1 LL->   K1     --K0 RR->  K0  K2
            /                 \
           K2                 K2
*/
pNode doubleRotateRL(pNode k)
{
    k -> right = singleRotateLL(k -> right);
    return singleRotateRR(k);
}

//插入
pNode insert(AVLTree t, elemType x)
{ 
    if(!t)     //新建
    {
        t = (AVLNode *)malloc(sizeof(AVLNode));;
        t -> data = x;
        t -> size = 1;
        t -> left = t -> right = NULL;
    }else
    {
        if(x < t -> data)   //应插入左子树
        {
            t -> left = insert(t -> left, x);
            //插入后不平衡
            if(HEIGHT(t -> left) - HEIGHT(t -> right) == 2)
            {
                //插入了左边的左边
                if(x < t -> left -> data)
                    t = singleRotateLL(t); //左单旋
                else    
                    t = doubleRotateLR(t); //左双旋
            }
        }else //x >= t -> data 插入右子树
        {
            t -> right = insert(t -> right, x);
            if(HEIGHT(t -> right) - HEIGHT(t -> left) == 2)
            {
                //插入右边的右边
                if(x >= t -> right -> data)
                    t = singleRotateRR(t); //右单旋 
                else
                    t = doubleRotateRL(t);
            }
        }
    }
    maintain(t);
    return t;
}

//删除后重新平衡
pNode delBalance(AVLTree t)
{
    if(HEIGHT(t -> left) - HEIGHT(t -> right) == 2)
    {         //删除后左边高了
        if(t -> left)
        {
            //左孩子的左孩子比左孩子的右孩子高，左单旋
            if(HEIGHT(t -> left -> left) >= HEIGHT(t -> left -> right))
                t = singleRotateLL(t);
            else
                t = doubleRotateLR(t);
        }
    }
    if(HEIGHT(t -> right) - HEIGHT(t -> left) == 2)
    {
        if(t -> right)
        {   
            //同理
            if(HEIGHT(t -> right -> right) >= HEIGHT(t -> right -> left))
                t = singleRotateRR(t);
            else
                t = doubleRotateRL(t);
        }
    }
    maintain(t);
    return t;
}
//删除
pNode delNode(AVLTree t,elemType x)
{
    if(!t)
        return t;
    if(x == t -> data)   //找到要删除的节点
    {
        //没有右子树，直接让他的左孩子代替他的位置。
        if(t -> right == NULL)
        {
            pNode tmp = t;
            t = t -> left;
            free(tmp);
        }else
        {
            //存在右子树，找到右子树中最小的节点。
            pNode tmp = t -> right;
            while(tmp -> left)
                tmp = tmp -> left;
            //仅仅是替换掉t的值，不是交换tmp与t!!
            t -> data = tmp -> data;
            //在t的右子树种删掉tmp
            t -> right = delNode(t -> right, t -> data);
        }
        maintain(t);
        return t;
    }else if(x < t -> data)
    {
        t -> left = delNode(t -> left, x);    //向左边删除
    }else
    {
        t -> right = delNode(t -> right, x);
    }

    //维护平衡和维护信息。
    if(t -> left)
        t -> left = delBalance(t -> left);
    if(t -> right)
        t -> right = delBalance(t -> right);
    if(t)
        t = delBalance(t);
    return t;
}

void destroy(AVLTree *t)
{
    AVLTree root = *t;
    if(!root)return;
    if(root -> left)
        destroy(&root -> left);
    if(root -> right)
        destroy(&root -> right);
    free(root);
    *t = NULL;
}
#endif