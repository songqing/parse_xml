#ifndef _STACK_H_
#define _STACK_H_
#include"main.h"
#define STACKLEN 3200
#define STACKINCREMENT 100
/*
//栈中元素类型
typedef struct StackType
{
Bcstype bt;
char* startpos;  //起始地址
int len;
}StackType;

//用来匹配每个buffer中的<标签
typedef struct SqStack
{
StackType *base;
StackType *top;
int stacksize;
}SqStack;

extern SqStack * step3stack;
*/
int InitStack(SqStack *S);

int DestroyStack(SqStack *S);

//bool StackEmpty(SqStack* S){return S->top<=S->base;}

//int StackLength(SqStack S){return S.stacksize;}
StackType GetTop(SqStack *S);

int Push(SqStack *S,StackType e);

int Pop(SqStack *S,StackType *e);
//第三阶段，处理全局栈
int HandleStack(SqStack*);
#endif
