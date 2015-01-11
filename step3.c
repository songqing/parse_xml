#include"stack.h"
#include"main.h"

//将第二阶段ｂｕｆｆｅｒ中剩余的标签放入全局栈中
void BCSLeftHandle(Buffer* b)
{
	ull i;
	for(i=1ULL;i<=bufnummber;i++)
	{
		//将栈中剩余元素添加到全局栈中
		while(b->stack->base!=b->stack->top)
		{
			Push(step3stack,*(b->stack->base++));

		}
		//		printf("bufnum is %lld.\n",b->bufnum);
		b=b->next;
	}
}
//处理全局栈
int HandleStack(SqStack* stack)
{
	//		int k=0;
	int count1=0,count2=0;

	SqStack* stackTemp=(SqStack*)malloc(sizeof(SqStack));
	StackType * stackbcs3=(StackType*)malloc(sizeof(StackType));

	InitStack(stackTemp);

	while(stack->base<stack->top)
	{
		//		*stackbcs3=stack->base;
		if(stackTemp->top<=stackTemp->base)
		{
			if(stack->base->bt==Etag_start)
			  return 0;

			Push(stackTemp,*(stack->base++));

			count1++;
			/*			 *stackbcs3=GetTop(stackTemp);
						 printf("instack type %d, len %d, name ",
						 (stackbcs3->bt==StagorEmptytag_start)?1:0,stackbcs3->len);
						 for(k=0;k<(*stackbcs3).len;k++)
						 printf("%c",*(stackbcs3->startpos++));
						 printf("\n");
						 */			 
		}
		else
		{
			//栈顶元素是开始标签，直接入栈
			if(stack->base->bt==StagorEmptytag_start)
			{

				if((stackTemp->top-1)->bt==Etag_start)
				  return 0;

				Push(stackTemp,*(stack->base++));

				count1++;
				/*				 *stackbcs3=GetTop(stackTemp);
								 printf("instack type %d, len %d, name ",
								 (stackbcs3->bt==StagorEmptytag_start)?1:0,stackbcs3->len);
								 for(k=0;k<(*stackbcs3).len;k++)
								 printf("%c",*(stackbcs3->startpos++));
								 printf("\n");
								 */				 
			}
			else  //全局栈顶为结束标签
			{
				if((stackTemp->top-1)->bt==Etag_start)
				  return 0;
				else if((stackTemp->top-1)->len!=stack->base->len)
				  return 0;
				else if(strncmp((stackTemp->top-1)->startpos,stack->base->startpos,stack->base->len)!=0)
				  return 0;
				else
				{  
					stack->base++;

					count2++;
					/*					 *stackbcs3=GetTop(stackTemp);
										 printf("outstack type %d, len %d, name ",
										 (stackbcs3->bt==StagorEmptytag_start)?1:0,stackbcs3->len);
										 for(k=0;k<(*stackbcs3).len;k++)
										 printf("%c",*(stackbcs3->startpos++));
										 printf("\n");
										 */					 
					Pop(stackTemp,stackbcs3);	

				}

			}
		}
	}

	printf("instack num=%d, outstack num=%d.\n",count1,count2);

	if(stack->top==stack->base)
	  return 1;
	else return 0;
}
