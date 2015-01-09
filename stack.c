#include"stack.h"

//static SqStack S;
//static StackType elem;
int InitStack(SqStack *S)
{
/*
	S=NULL;
	S=(SqStack*)malloc(sizeof(SqStack));
	if(!S)
	{
		printf("init stack failed!\n");
		return 0;
	}
*/
	S->base=(StackType*)malloc(STACKLEN*sizeof(StackType));
	if(!S->base)
	{
		printf("init stack base failed!\n");
		return 0;
	}
	S->top=S->base;
	S->stacksize=STACKLEN;
	return 1;
}
int DestroyStack(SqStack *S)
{
	if(S!=NULL)
	  free(S->base);
//	S->base=S->top=NULL;
//	S->stacksize=0;
//	free(S);
	//S=NULL;

	return 1;
}


StackType GetTop(SqStack *S)
{
//	StackType *e=(StackType*)malloc(sizeof(StackType));
	//if(S->top==S->base) return NULL;
	return *(S->top-1);
//	return 1;
}

int Push(SqStack *S,StackType e)
{
	if(S->top-S->base>=S->stacksize)
	{
		S->base=(StackType*)realloc(S->base,(S->stacksize+STACKINCREMENT)*sizeof(StackType));
		if(!S->base) return 0;
		S->top=S->base+S->stacksize;
		S->stacksize+=STACKINCREMENT;
	}
	*S->top++=e;
	return 1;
}

int Pop(SqStack *S,StackType *e)
{
	if(S->top==S->base) return 0;
	e=--S->top;
	return 1;
}
