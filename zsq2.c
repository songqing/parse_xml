//添加多线程实现
//1228更新 添加锁机制，读文件单独一个线程，边读边写
//解析test.xml只需70ｍｓ
//将fputc()改为fread(),代码运行时间从400ms降到70ｍｓ
//读数据快的函数只运行5ｍｓ，故不单独设一个线程
#include<sched.h>
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include<pthread.h>
#include<semaphore.h>
#define BUFLEN 1024000
#define BCSLEN 50000
typedef unsigned long long ull;
char filename[]="test.xml";
ull bufnummber = 1ULL;
FILE *fp;


typedef enum Bcstype{
      StagorEmptytag_start,
      Etag_start,
      PI_start,
      Content,
      CDSECT_start,
      COMMENT_start
} Bcstype;

typedef struct bcs_t{
     ull fileoffset;
     ull bufnum;
     ull bufpos;
     Bcstype bt;
}Bcs;

typedef struct Bcsarray_t{
	Bcs bcs[BCSLEN];
	ull bcsmax;
}Bcsarray;



typedef struct Buffer_t{
	pthread_mutex_t lock;
	char buf[BUFLEN];
	Bcsarray  bcsay;
	ull bufnum;
	int FINISH_STAGE1;
	int FINISH_STAGE2;
	int FINISH_STAGE3;
	int START_STAGE2;
	int START_STAGE3;
	struct	Buffer_t  *next;
}Buffer;


//int read(char *name,Buffer *buffer);

Buffer *Bufferinit();		// 第0阶段，将XML文件分块读入


int identify(Buffer *b);  //第一阶段判别一个Buffer中的全部'<'位置

int bufferprint(Buffer * c);	//输出buffer中第一阶段'<'位置

void* handle1(void* args);  //线程1，解析ｘｍｌ文件
void* handle2(void* args);
//void* handle3(void* args);
pthread_t phandl1,phandl2;

Buffer *start,*p,*p1,*p2;

int main(){
//	printf("input the file name!\n");
//	gets(filename);
	p1=p2=NULL;	
	//p=NULL;
	start = Bufferinit();	// 第0阶段，将XML文件分块读入
	p1 = start;
	if(p1!=NULL && p1->next!=NULL)
		p2=p1->next;
	//if(p2!=NULL && p2->next!=NULL)
	//	p3=p2->next;
	// 下面开始第一阶段
	// 
/*
	for(i=1;i<=bufnummber;i++){
		identify(p);
		p=p->next;
	}
*/
	
	int arg;
	arg=1;
	pthread_create(&phandl1,NULL,(void*)handle1,(void*)&arg);
	arg=2;
	pthread_create(&phandl2,NULL,(void*)handle2,(void*)&arg);
//	arg=3;
	//pthread_create(&phandl3,NULL,(void*)handle3,(void*)&arg);
	pthread_join(phandl1,NULL);
	pthread_join(phandl2,NULL);
//	pthread_join(phandl3,NULL);
	//exit(1);
	
	//输出buffer中第一阶段'<'位置

	ull i;
	p = start;
	for(i=1;i<=bufnummber;i++){
		bufferprint(p);
		p=p->next;
	}

	exit(1);

}
  //handle1线程解析奇数号的数据块Buffer
void* handle1(void* args)
{
//	int t=*((int*)args);
	ull i;
	for(i=1;i<=bufnummber;i+=2)
	{
		identify(p1);
		if(i+2<=bufnummber)
			p1=p1->next->next;
	}

}
//handle2线程解析偶数号的Buffer
void* handle2(void* args)
{
	ull i;
	for(i=2;i<=bufnummber;i+=2)
	{
		identify(p2);
		if(i+2<bufnummber)
			p2=p2->next->next;
	}
}

/*
void* handle3(void* args)
{
	int i;
	for(i=3;i<=bufnummber;i+=3)
	{
		identify(p3);
		if(i+3<bufnummber)
			p3=p3->next->next->next;
	}
}
*/
/*
int read(char *name,Buffer *buffer){
	char c;
	int i=0;


	while((c=fgetc(fp)) != EOF && i<BUFLEN){
		buffer->buf[i++]=c;
	}
	if(c==EOF)
		return 0;
	else 
		return 1;
}
*/

Buffer *Bufferinit(){		// 第0阶段，将XML文件分块读入
	ull i;
	int readst;
	Buffer *p,*q,*pl;
	
	if((fp = fopen(filename,"r")) == NULL){
		printf("error:cannot open the file.\n");
		return NULL;
	}
	
	if((p = (Buffer *)malloc(sizeof(Buffer)))!=NULL)
	{
		//readst = read(filename,p);
		readst = fread(p->buf,BUFLEN,1,fp);  //将fputc()函数改为fread()函数，运行效率大大提高
		p->bufnum = bufnummber++;
		p->FINISH_STAGE1 = 0;	// 1阶段未完成
		p->FINISH_STAGE2 = 0;
		p->FINISH_STAGE3 = 0;
		p->START_STAGE2 = 0;
		p->START_STAGE3 = 0;
		p->next = NULL;
		if(pthread_mutex_init(&p->lock,NULL)!=0)
		{
			free(p);
			return NULL;
		}

		//printf("the first buffer read completed!\n");
		//	printf("bufnummber %d\n",bufnummber);
		pl = p;
		while( readst>0 ){
			q = (Buffer *)malloc(sizeof(Buffer));
			//	readst = read(filename,q);
			readst = fread(q->buf,BUFLEN,1,fp);
			q->bufnum = bufnummber++;
			q->FINISH_STAGE1 = 0;	// 表示1阶段未完成
			q->FINISH_STAGE2 = 0;
			q->FINISH_STAGE3 = 0;
			q->START_STAGE2 = 0;
			q->START_STAGE3 = 0;
			/*	
				for(i=1;i<bufnummber-2;i++){
				pl=pl->next;
				}
				*/
			pl->next = q;
			pl=q;
			q->next = NULL;
			if(pthread_mutex_init(&q->lock,NULL)!=0)
			{
				free(q);
				return NULL;
			}
			//	printf("bufnummber %d\n",bufnummber);
		}
		bufnummber--;
		//	printf("bufnummber=%d\n",bufnummber);

	}
	return p;
}


int identify(Buffer *b){  //第一阶段判别一个Buffer中的全部'<'位置
	ull i,j,n=1ULL;
	int st ;
	for(i=0;i<BUFLEN;i++){
		if(b->buf[i] == '<'){ 
			
			//Etag_start
			if(b->buf[i+1] == '/'){
				if(n<=BCSLEN){
					b->bcsay.bcs[n].fileoffset = (b->bufnum -1)*BUFLEN+i+1;
					b->bcsay.bcs[n].bufpos = i+1;
					b->bcsay.bcs[n].bufnum = b->bufnum;
					b->bcsay.bcs[n].bt = Etag_start;
					n++;
				}
				else{
					printf("error: BCSLEN is too short!\n");
					return -1;
				}
			}
			
			//PI_start
			else if(b->buf[i+1] == '?'){
				j=i;
				st=0;
				for(i=i+3;i<=BUFLEN;i++){
					if(b->buf[i-1]=='?' && b->buf[i]=='>'){
						st = 1;
						break;
					}
				}
				if(st == 1 || i > BUFLEN){
					if(n<=BCSLEN){
						b->bcsay.bcs[n].fileoffset = (b->bufnum -1)*BUFLEN+j+1;
						b->bcsay.bcs[n].bufpos = j+1;
						b->bcsay.bcs[n].bufnum = b->bufnum;
						b->bcsay.bcs[n].bt = PI_start;
						n++;
					}
					else{
						printf("error: BCSLEN is too short!\n");
						return -1;
					}
				}
			}

			//CDSECT_start
			else if(b->buf[i+1] == '!' && b->buf[i+2] == '['&& b->buf[i+3] == 'C'&& b->buf[i+4] == 'D'&& b->buf[i+5] == 'A'&& b->buf[i+6] == 'T'&& b->buf[i+7] == 'A'&& b->buf[i+8] == '['){
				j=i;
				st=0;
				for(i=i+4;i<=BUFLEN;i++){
					if(b->buf[i-2]==']' && b->buf[i-1]==']'&& b->buf[i]=='>'){
						st = 1;
						break;
					}
				}
				if(st == 1 || i > BUFLEN){
					if(n<=BCSLEN){
						b->bcsay.bcs[n].fileoffset = (b->bufnum -1)*BUFLEN+j+1;
						b->bcsay.bcs[n].bufpos = j+1;
						b->bcsay.bcs[n].bufnum = b->bufnum;
						b->bcsay.bcs[n].bt = CDSECT_start;
						n++;
					}
					else{
						printf("error: BCSLEN is too short!\n");
						return -1;
					}
				}
			}

			//COMMENT_start
			else if(b->buf[i+1] == '!'&& b->buf[i+2] == '-'&& b->buf[i+3] == '-'){
				j=i;
				st=0;
				for(i=i+4;i<=BUFLEN;i++){
					if(b->buf[i-2]=='-' && b->buf[i-1]=='-'&& b->buf[i]=='>'){
						st = 1;
						break;
					}
				}
				if(st == 1 || i > BUFLEN){
					if(n<=BCSLEN){
						b->bcsay.bcs[n].fileoffset = (b->bufnum -1)*BUFLEN+j+1;
						b->bcsay.bcs[n].bufpos = j+1;
						b->bcsay.bcs[n].bufnum = b->bufnum;
						b->bcsay.bcs[n].bt = COMMENT_start;
						n++;
					}
					else{
						printf("error: BCSLEN is too short!\n");
						return -1;
					}
				}
			}

			//StagorEmptytag_start
			else{
				if(n<=BCSLEN){
					b->bcsay.bcs[n].fileoffset = (b->bufnum -1)*BUFLEN+i+1;
					b->bcsay.bcs[n].bufpos = i+1;
					b->bcsay.bcs[n].bufnum = b->bufnum;
					b->bcsay.bcs[n].bt = StagorEmptytag_start;
					n++;
				}
				else{
					printf("error: BCSLEN is too short!\n");
					return -1;
				}
			}
		}
	}
	b->bcsay.bcsmax = --n;
	b->FINISH_STAGE1 = 1;	//修改FINISH_STAGE1，表示第一阶段结束
	return 1;
}


int bufferprint(Buffer * c){	//输出buffer中第一阶段'<'位置
	ull i;
	printf("the NO.%lld buffer: number %lld\n",c->bufnum,c->bcsay.bcsmax);
	for(i=1ULL;i<=c->bcsay.bcsmax;i++){
		printf("the NO.%lld < information:\n",i);
		switch(c->bcsay.bcs[i].bt){
			case 0: printf("the type of this < is StagorEmptytag_start.\n");break;
			case 1:	printf("the type of this < is Etag_start.\n");break;
			case 2:	printf("the type of this < is PI_start.\n");break;
			case 3:	printf("the type of this < is Conten.\n");break;
			case 4:	printf("the type of this < is CDSECT_star.\n");break;
			case 5:	printf("the type of this < is COMMENT_start.\n");break;
		}
		printf("the position is %lld.\n",c->bcsay.bcs[i].bufpos);
		printf("the offset is %lld.\n",c->bcsay.bcs[i].fileoffset);
		printf("\n");
	}
	return 1;
}

