#include"main.h"
#include"stack.h"

//SqStack* step3stack;

//信号量
//sem_t num_buffer_product;
//sem_t num_buffer_consume;
sem_t mutex;
sem_t mutexWhole;
pthread_t phandle1,phandle2;
ull bufCount=0;

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
					/*
					//测试是否压栈成功
					 *stacktop=GetTop(stack);
					 printf("instack type %d, len %d, name ",
					 (stacktop->bt==StagorEmptytag_start)?1:0,stacktop->len);
					 for(k=0;k<(*stacktop).len;k++)
					 printf("%c",*(stacktop->startpos++));
					 printf("\n");
					 */
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
						/*
						 *stacktop=GetTop(stack);
						 printf("instack type %d, len %d, name ",
						 (stacktop->bt==StagorEmptytag_start)?1:0,stacktop->len);
						 for(k=0;k<(*stacktop).len;k++)
						 printf("%c",*(stacktop->startpos++));
						 printf("\n");
						 */
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
								/*
								 *stacktop=GetTop(stack);
								 printf("instack type %d, len %d, name ",
								 ((*stacktop).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
								 for(k=0;k<(*stacktop).len;k++)
								 printf("%c",*(stacktop->startpos++));
								 printf("\n");
								 */
								//break;

							}
							else
							{
								//匹配成功
								if(strncmp(stacktop->startpos,stackbcs->startpos,stackbcs->len)==0)
								{
									/*
									   printf("outstack type %d, len %d, name ",
									   ((*stacktop).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
									   for(k=0;k<(*stacktop).len;k++)
									   printf("%c",*(stacktop->startpos++));
									   printf("\n");
									   */
									Pop(stack,stacktop);
								}
								//匹配不成功
								else
								{
									Push(stack,*stackbcs);
									/*
									 *stacktop=GetTop(stack);
									 printf("instack type %d, len %d, name ",
									 ((*stacktop).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
									 for(k=0;k<(*stacktop).len;k++)
									 printf("%c",*(stacktop->startpos++));
									 printf("\n");
									 */
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
void BufferMatch1(Buffer* b)
{
	//初始化栈step3stack
	if(b->bufnum==1ULL)
	{
		InitStack(step3stack);
	}
	b->stack=(SqStack*)malloc(sizeof(SqStack));
	StackType *stackbcs1=(StackType*)malloc(sizeof(StackType));
	StackType* stacktop1=(StackType*)malloc(sizeof(StackType));
	//	stackbcs1=(StackType*)malloc(sizeof(StackType));
	//	stacktop1=(StackType*)malloc(sizeof(StackType));

	InitStack(b->stack);
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
					stackbcs1->bt=StagorEmptytag_start;
					stackbcs1->len=j-ntemp;
					stackbcs1->startpos=b->buf+ntemp;
					Push(b->stack,*stackbcs1);
					/*
					//测试是否压栈成功
					 *stacktop1=GetTop(stack);
					 printf("instack type %d, len %d, name ",
					 (stacktop1->bt==StagorEmptytag_start)?1:0,stacktop->len);
					 for(k=0;k<(*stacktop1).len;k++)
					 printf("%c",*(stacktop1->startpos++));
					 printf("\n");
					 */
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
					stackbcs1->bt=Etag_start;
					stackbcs1->len=j-ntemp;
					stackbcs1->startpos=b->buf+ntemp;

					//Push(stack,*stackbcs1);
					//如果栈顶为空，压入栈
					if(b->stack->top<=b->stack->base)
					{
						Push(b->stack,*stackbcs1);
						/*
						 *stacktop1=GetTop(stack);
						 printf("instack type %d, len %d, name ",
						 (stacktop1->bt==StagorEmptytag_start)?1:0,stacktop->len);
						 for(k=0;k<(*stacktop1).len;k++)
						 printf("%c",*(stacktop1->startpos++));
						 printf("\n");
						 */
						//break;
					}
					//栈非空
					else
					{
						*stacktop1=GetTop(b->stack);
						if(stacktop1->bt==StagorEmptytag_start)
						{
							//判断名称是否相等
							if(stacktop1->len!=stackbcs1->len)
							{
								Push(b->stack,*stackbcs1);
								/*
								 *stacktop1=GetTop(stack);
								 printf("instack type %d, len %d, name ",
								 ((*stacktop1).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
								 for(k=0;k<(*stacktop1).len;k++)
								 printf("%c",*(stacktop1->startpos++));
								 printf("\n");
								 */
								//break;

							}
							else
							{
								//匹配成功
								if(strncmp(stacktop1->startpos,stackbcs1->startpos,stackbcs1->len)==0)
								{
									/*
									   printf("outstack type %d, len %d, name ",
									   ((*stacktop1).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
									   for(k=0;k<(*stacktop1).len;k++)
									   printf("%c",*(stacktop1->startpos++));
									   printf("\n");
									   */
									Pop(b->stack,stacktop1);
								}
								//匹配不成功
								else
								{
									Push(b->stack,*stackbcs1);
									/*
									 *stacktop1=GetTop(stack);
									 printf("instack type %d, len %d, name ",
									 ((*stacktop1).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
									 for(k=0;k<(*stacktop1).len;k++)
									 printf("%c",*(stacktop1->startpos++));
									 printf("\n");
									 */
								}
							}
						}
						//与栈顶类型不匹配,栈顶类型为结束标签
						else
						{
							Push(b->stack,*stackbcs1);
						}

					}
					//测试是否压栈成功
					break;
				}
			}
			//	break;
		}
	}
	/*
	//将栈中剩余元素添加到全局栈中
	while(stack->base!=stack->top)
	{
	Push(step3stack,*(stack->base++));
	}
	*/
	//	DestroyStack(stack);
	/*	//清空栈中剩余的元素
		printf("buffer %lld\n",b->bufnum);
		while(stack->base!=stack->top)
		{
	 *stacktop1=GetTop(stack);
	 printf("left stack type %d, len %d, name ",
	 ((*stacktop1).bt==StagorEmptytag_start)?1:0,(*stacktop).len);
	 for(k=0;k<(*stacktop1).len;k++)
	 printf("%c",*(stacktop1->startpos++));
	 printf("\n");
	 Pop(stack,stacktop1);
	 }
	 */
}

void Bufferhandle(void* args)
{
	Buffer* buffer1=NULL;
	//	int t=*((int*)args);
	//	printf("t=%d\n",t);
	int flag=0;
	ull i=1;
	for(i=1;i<=bufnummber;i++)
	{
		flag=0;
		if(bufCount<bufnummber)
		{
			//			printf("bufferhandle2 wait!\n");
			sem_wait(&mutex);
			if(pbuffer->FINISH_STAGE2==0)
			{
				bufCount++;
				pbuffer->FINISH_STAGE2=1;
				buffer1=pbuffer;
				//				printf("Bufferhandle%d start, bufnum is %lld,\n",t,pbuffer->bufnum);
			}
			else
			{
				flag=1;
			}
			pbuffer=pbuffer->next;

			sem_post(&mutex);
			if(flag==0 && bufCount<=bufnummber)
			{
				//				printf("Bufferhandle%d start, bufnum is %lld,\n",t,buffer1->bufnum);
				BufferMatch1(buffer1);

				//pbuffer=pbuffer->next;
			}
		}
		if(bufCount>=bufnummber)
		  break;
	}

}
//第二阶段，多线程处理
void BufferMatchMultiThread()
{

	int arg1=0,arg2=0;
	//初始化信号量
	sem_init(&mutex,0,1);
	sem_init(&mutexWhole,0,1);
	//	sem_init(&num_buffer_product,0,bufnummber);
	arg1=1;
	pthread_create(&phandle1,NULL,(void*)Bufferhandle,(void*)&arg1);
	arg2=2;
	pthread_create(&phandle2,NULL,(void*)Bufferhandle,(void*)&arg2);
	pthread_join(phandle1,NULL);
	pthread_join(phandle2,NULL);
	//return;
}
