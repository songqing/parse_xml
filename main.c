//添加多线程实现
//1228更新 添加锁机制，读文件单独一个线程，边读边写
//解析test.xml只需70ｍｓ
//将fputc()改为fread(),代码运行时间从400ms降到70ｍｓ
//读数据快的函数只运行5ｍｓ，故不单独设一个线程
//1231更新 读数据块单独一个线程，解析小于号单独一个线程，边读边解析
//1111字符串拷贝  memcpy()函数 
#include"main.h"
#include"stack.h"
//typedef unsigned long long ull;
ull bufnummber=1ULL;
//全局栈，用于第三阶段匹配剩余的标签
//SqStack* wholeStack;

//信号量
//sem_t num_buffer_product;
//sem_t num_buffer_consume;
//sem_t mutex;

pthread_t phandl0, phandl1,phandl2;

Buffer *start=NULL,*pbuffer=NULL,*pprev=NULL;

//避免变量重复定义
extern Buffer* Bufferinit();
extern int identify(Buffer*);

extern int InitStack(SqStack*);
extern int DestroyStack(SqStack*);
extern int Push(SqStack*,StackType);
extern StackType GetTop(SqStack*);

//第二阶段ｂｕｆｆｅｒ中标签匹配
extern void BufferMatch(Buffer* b);

SqStack * step3stack;
//第三阶段，处理全局栈
extern int HandleStack(SqStack* stack);
int main(){
//	printf("input the file name!\n");
//	gets(filename);
//	p1=NULL;	
	//p=NULL;
	
	step3stack=(SqStack*)malloc(sizeof(SqStack));

	start = Bufferinit();	// 第0阶段，将XML文件分块读入 耗时33ms
	pprev=pbuffer=start;
	//if(p1!=NULL && p1->next!=NULL)
	//	p2=p1->next;
	//if(p2!=NULL && p2->next!=NULL)
	//	p3=p2->next;
	// 下面开始第一阶段
	// 

	//解析小与号，耗时88ms
	ull i;
	for(i=1;i<=bufnummber;i++){
		identify(pbuffer);
		pprev=pbuffer;
		pbuffer=pbuffer->next;
	}

	//初始化信号量
//	sem_init(&mutex,0,1);
//	sem_init(&num_buffer_product,0,0);
//	sem_init(&num_buffer_consume,0,50);

//	int arg;
//	arg=0;
	//读入数据块线程
//	pthread_create(&phandl0,NULL,(void*)handle0,(void*)&arg);
//	arg=1;
	//解析小于号线程
//	pthread_create(&phandl1,NULL,(void*)handle1,(void*)&arg);
//	arg=2;
//	pthread_create(&phandl2,NULL,(void*)handle2,(void*)&arg);
//	arg=3;
	//pthread_create(&phandl3,NULL,(void*)handle3,(void*)&arg);
//	pthread_join(phandl0,NULL);
//	pthread_join(phandl1,NULL);
//	pthread_join(phandl2,NULL);
//	pthread_join(phandl3,NULL);
	//exit(1);
	
	//输出buffer中第一阶段'<'位置

	//ull i;
/*	
	printf("buffer %lld :%szsq\n",start->bufnum,start->buf);
	printf("buffer %lld :%s\n",start->next->bufnum,start->next->buf);
	//bufferprint(start);

	pbuffer = start;
	for(i=1;i<=bufnummber;i++){
		bufferprint(pbuffer);
		pbuffer=pbuffer->next;
	}
*/
	//第二阶段，事件解析阶段，解析“<”名称以及是否匹配
	printf("the first stage has finished!\n");

	//第二阶段，耗时120ms
	pbuffer=start;
	for(i=1;i<=bufnummber;i++){
		BufferMatch(pbuffer);
		pbuffer=pbuffer->next;
	}

	printf("the second stage has finished!\n");

	//	BufferMatch(start);
	//查看全局栈中剩余的标签

/*
	int k,count1=0,count2=0;
	StackType *stackTemp=(StackType*)malloc(sizeof(StackType));
	while(step3stack->base!=step3stack->top)
	{
		stackTemp=step3stack->base++;
		if(stackTemp->bt==Etag_start)
		  count1++;
		else
		  count2++;
		printf("type %d, len %d, name ",
					((*stackTemp).bt==StagorEmptytag_start)?1:0,(*stackTemp).len);
		for(k=0;k<(*stackTemp).len;k++)
		  printf("%c",*(stackTemp->startpos++));
		printf("\n");

	}

	printf("count1= %d, count2=%d\n",count1,count2);
*/
	//第三阶段，处理全局栈
	if(HandleStack(step3stack)==1)
	  printf("the third stage has finished!,match success!");
	else
	  printf("the third stage has finished!,match failed!");
	
	exit(1);
}
