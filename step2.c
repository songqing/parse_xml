#include"main.h"
#include"stack.h"

//SqStack* step3stack;

SqStack *stack;
StackType *stackbcs;
StackType *stacktop;
void BufferMatch(Buffer* b)
{
	//初始化栈step3stack
	if(b->bufnum==1ULL)
	{
		InitStack(step3stack);
	}
	stack=(SqStack*)malloc(sizeof(SqStack));
	stackbcs=(StackType*)malloc(sizeof(StackType));
	stacktop=(StackType*)malloc(sizeof(StackType));

	InitStack(stack);
	ull i=1ULL,j=0ULL,ntemp;
//	int k;
	for(i=1ULL;i<=b->bcsay.bcsmax;i++)
	{
		//处理开始标签，直接压栈
		if(b->bcsay.bcs[i].bt==StagorEmptytag_start)
		{
			ntemp=b->bcsay.bcs[i].bufpos;
			for(j=ntemp;j<BUFLEN+1024;j++)
			{
				if(b->buf[j]==' ' || b->buf[j]=='>')
				{
					stackbcs->bt=StagorEmptytag_start;
					stackbcs->len=j-ntemp;
					stackbcs->startpos=b->buf+ntemp;
					Push(stack,*stackbcs);
					//测试是否压栈成功
//					*stacktop=GetTop(stack);
//					printf("instack type %d, len %d, name ",
//								(stacktop->bt==StagorEmptytag_start)?1:0,stacktop->len);
//					for(k=0;k<(*stacktop).len;k++)
//					  printf("%c",*(stacktop->startpos++));
//					printf("\n");
					break;
				}
			}
		}
		//处理结束标签
		else if(b->bcsay.bcs[i].bt==Etag_start)
		{
			ntemp=b->bcsay.bcs[i].bufpos+1; //从 </后开始
			for(j=ntemp;j<=BUFLEN+1024;j++)
			{
				if(b->buf[j]==' ' || b->buf[j]=='>')
				{
					stackbcs->bt=Etag_start;
					stackbcs->len=j-ntemp;
					stackbcs->startpos=b->buf+ntemp;
					
					//Push(stack,*stackbcs);
					//如果栈顶为空，压入栈
					if(stack->top<=stack->base)
					{
						Push(stack,*stackbcs);
//						*stacktop=GetTop(stack);
//						printf("instack type %d, len %d, name ",
//									(stacktop->bt==StagorEmptytag_start)?1:0,stacktop->len);
//						for(k=0;k<(*stacktop).len;k++)
//						printf("%c",*(stacktop->startpos++));
//						printf("\n");
						//break;
					}
					//栈非空
					else
					{
						*stacktop=GetTop(stack);
						if(stacktop->bt==StagorEmptytag_start)
						{
							//判断名称是否相等
							if(stacktop->len!=stackbcs->len)
							{
								Push(stack,*stackbcs);
//								*stacktop=GetTop(stack);
//								printf("instack type %d, len %d, name ",
//											((*stacktop).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
//								for(k=0;k<(*stacktop).len;k++)
//								printf("%c",*(stacktop->startpos++));
//								printf("\n");
								//break;
								
							}
							else
							{
								//匹配成功
								if(strncmp(stacktop->startpos,stackbcs->startpos,stackbcs->len)==0)
								{
//									printf("outstack type %d, len %d, name ",
//												((*stacktop).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
//									for(k=0;k<(*stacktop).len;k++)
//									printf("%c",*(stacktop->startpos++));
//									printf("\n");
									Pop(stack,stacktop);
								}
								//匹配不成功
								else
								{
									Push(stack,*stackbcs);
//									*stacktop=GetTop(stack);
//									printf("instack type %d, len %d, name ",
//												((*stacktop).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
//									for(k=0;k<(*stacktop).len;k++)
//									printf("%c",*(stacktop->startpos++));
//									printf("\n");
								}
							}
						}
						//与栈顶类型不匹配,栈顶类型为结束标签
						else
						{
							Push(stack,*stackbcs);
						}
						
					}
					//测试是否压栈成功
					break;
				}
			}
		//	break;
		}
	}

	//将栈中剩余元素添加到全局栈中
	while(stack->base!=stack->top)
	{
		Push(step3stack,*(stack->base++));
	}
//	DestroyStack(stack);
/*	//清空栈中剩余的元素
	printf("buffer %lld\n",b->bufnum);
	while(stack->base!=stack->top)
	{
		*stacktop=GetTop(stack);
		printf("left stack type %d, len %d, name ",
					((*stacktop).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
		for(k=0;k<(*stacktop).len;k++)
		  printf("%c",*(stacktop->startpos++));
		printf("\n");
		Pop(stack,stacktop);
	}
*/
}
