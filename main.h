//添加多线程实现
//1228更新 添加锁机制，读文件单独一个线程，边读边写
//解析test.xml只需70ｍｓ
//将fputc()改为fread(),代码运行时间从400ms降到70ｍｓ
//读数据快的函数只运行5ｍｓ，故不单独设一个线程
//1231更新 读数据块单独一个线程，解析小于号单独一个线程，边读边解析
//1111字符串拷贝  memcpy()函数 
#ifndef _MAIN_H_
#define _MAIN_H_
#include<sched.h>
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include<pthread.h>
#include<semaphore.h>
#define BUFLEN 102400
#define BCSLEN 4000
typedef unsigned long long ull;
extern ull bufnummber;

//信号量
//sem_t num_buffer_product;
//sem_t num_buffer_consume;
//sem_t mutex;

//BCS类型
typedef enum Bcstype{
	StagorEmptytag_start,
	Etag_start,
	PI_start,
	Content,
	CDSECT_start,
	COMMENT_start,
	StartAndEnd
} Bcstype;

//BCS内容
typedef struct bcs_t{
	ull fileoffset;
	ull bufnum;
	ull bufpos;
	Bcstype bt;
}Bcs;

//每个Buffer中含有的BCS数组
typedef struct Bcsarray_t{
	Bcs bcs[BCSLEN];
	ull bcsmax;
}Bcsarray;


//栈中元素类型
typedef struct StackType
{
	Bcstype bt;
	char* startpos;  //起始地址
	int len;
}StackType;

//顺序栈，用来匹配每个buffer中的<标签
typedef struct SqStack
{
	StackType *base;
	StackType *top;
	int stacksize;
}SqStack;

//Buffer内容
typedef struct Buffer_t{
	//pthread_mutex_t lock;
	char buf[BUFLEN+1024];
	Bcsarray  bcsay;
	ull bufnum;
	int FINISH_STAGE0;
	int FINISH_STAGE1;
	int FINISH_STAGE2;
	int FINISH_STAGE3;
	int START_STAGE2;
	int START_STAGE3;
	SqStack* stack;
	struct	Buffer_t  *next;
}Buffer;

extern SqStack * step3stack;
extern Buffer *start,*pbuffer,*pprev;

//int read(char *name,Buffer *buffer);

Buffer* Bufferinit();		// 第0阶段，将XML文件分块读入


int identify(Buffer *b);  //第一阶段判别一个Buffer中的全部'<'位置

int bufferprint(Buffer * c);	//输出buffer中第一阶段'<'位置

//读入数据块线程
//void* handle0(void* args);

void* handle1(void* args);  //线程1，解析ｘｍｌ文件
//void* handle2(void* args);

//第二阶段匹配buffer中的标签
void BufferMatch(Buffer*);
//第二阶段，多线程处理
void BufferMatchMultiThread();
//第三阶段处理全局栈
//int HandleStack(Sq)
//step3,将ｓｔｅｐ2中ｂｕｆｆｅｒ中剩余的标签放入全局栈中
void BCSLeftHanlde(Buffer*);
#endif
