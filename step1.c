//添加多线程实现
//1228更新 添加锁机制，读文件单独一个线程，边读边写
//解析test.xml只需70ｍｓ
//将fputc()改为fread(),代码运行时间从400ms降到70ｍｓ
//读数据快的函数只运行5ｍｓ，故不单独设一个线程
//1231更新 读数据块单独一个线程，解析小于号单独一个线程，边读边解析
//1111字符串拷贝  memcpy()函数 
#include"main.h"
//typedef unsigned long long ull;
char filename[]="test.xml";
//ull bufnummber = 1ULL;
FILE *fp;

//pthread_t phandl0, phandl1,phandl2;

Buffer *step1buffer;
int read_thread=0;
int parse_thread=0;
/*
void* handle0(void* args)
{
	Bufferinit();	// 第0阶段，将XML文件分块读入

}
*/
  //handle1线程解析奇数号的数据块Buffer
void* handle1(void* args)
{
//	int t=*((int*)args);
	ull i;
	for(i=1;i<=bufnummber;i++)
	//while(1)
	{
//		sem_wait(&num_buffer_product);
//		sem_wait(&mutex);
//		printf("parse buffer %d\n",parse_thread++);
		identify(step1buffer);
		step1buffer=step1buffer->next;
//		sem_post(&mutex);
//		sem_post(&num_buffer_consume);
	}
	return ((void*)0);
}
/*
//handle2线程解析偶数号的Buffer
void* handle2(void* args)
{
	ull i;
	for(i=2;i<=bufnummber;i+=2)
	{
		identify(p2);
		if(i+2<=bufnummber)
			p2=p2->next->next;
	}
}
*/
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

Buffer* Bufferinit(){		// 第0阶段，将XML文件分块读入
//	ull i;
	int readst;
	Buffer *p,*q,*pl;
	
	if((fp = fopen(filename,"r")) == NULL){
		printf("error:cannot open the file.\n");
		return NULL;
	}
	
	if((p = (Buffer *)malloc(sizeof(Buffer)))!=NULL)
	{
		//申请一个资源
//		sem_wait(&num_buffer_consume);
//		//加锁
//		sem_wait(&mutex);
		//readst = read(filename,p);
		readst = fread(p->buf,BUFLEN,1,fp);  //将fputc()函数改为fread()函数，运行效率大大提高
		p->bufnum = bufnummber++;
		p->FINISH_STAGE0=1;
		p->FINISH_STAGE1 = 0;	// 1阶段未完成
		p->FINISH_STAGE2 = 0;
		p->FINISH_STAGE3 = 0;
		p->START_STAGE2 = 0;
		p->START_STAGE3 = 0;
		p->next = NULL;
//		if(pthread_mutex_init(&p->lock,NULL)!=0)
//		{
//			free(p);
//			return NULL;
//		}

		//全局变量start,p1赋值
	//	start=p1=p;
		//printf("the first buffer read completed!\n");
		//	printf("bufnummber %d\n",bufnummber);
		pl = p;
//		printf("read buffer %d\n",read_thread++);
		//解锁
//		sem_post(&mutex);
		//读入一个缓冲区
//		sem_post(&num_buffer_product);
	//	sleep(1);
		while( readst>0 ){
			
			//申请一个资源
//			sem_wait(&num_buffer_consume);
			//加锁
//			sem_wait(&mutex);
			q = (Buffer *)malloc(sizeof(Buffer));
			//	readst = read(filename,q);
			readst = fread(q->buf,BUFLEN,1,fp);
			q->bufnum = bufnummber++;
			p->FINISH_STAGE0=1;
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
//			if(pthread_mutex_init(&q->lock,NULL)!=0)
//			{
//				free(q);
//				return NULL;
//			}
			//	printf("bufnummber %d\n",bufnummber);
//			printf("read buffer %d\n",read_thread++);
			//解锁
//			sem_post(&mutex);
			//读入一个缓冲区
//			sem_post(&num_buffer_product);
		}
		bufnummber--;
		//	printf("bufnummber=%d\n",bufnummber);

	}
	return p;
}


int identify(Buffer *b){  //第一阶段判别一个Buffer中的全部'<'位置
//	printf("buffer %lld\n",b->bufnum);
	ull i=0ULL,j,n=1ULL;
	int st ;
	//将Buffer中第一个“<”之前的数据拷贝到上一个缓冲区中
	if(b->bufnum>1)
	{
		for(;i<BUFLEN;i++)
		{
//			if(b->buf[i]==' ' || b->buf[i]=='\t' || b->buf[i]=='\n')
//			  continue;
			if(b->buf[i]=='<')
			{
				break;
			}
			if(b->buf[i]=='>')
			{
//				printf("i=%lld\n",i);
				memcpy(pprev->buf+BUFLEN,b->buf,i+1);
				i++;
				//根据这个>判断上一个buffer最后一个BCS的类型
				if(i==1)
				{
					if(*(pprev->buf+BUFLEN-1)=='/')
					{
						pprev->bcsay.bcs[pprev->bcsay.bcsmax].bt=StartAndEnd;
					}
				}
				else 
				{
					if(*(pprev->buf+BUFLEN-1)!='<')
					{
						if(b->buf[i-2]=='/')
						{
							pprev->bcsay.bcs[pprev->bcsay.bcsmax].bt=StartAndEnd;
						}
					}
				//	if(*(pprev->buf+BUFLEN-1)=='<')
					else
					{
						if(b->buf[0]=='/')
						{
							pprev->bcsay.bcs[++(pprev->bcsay.bcsmax)].fileoffset = (b->bufnum -1)*BUFLEN;
							pprev->bcsay.bcs[pprev->bcsay.bcsmax].bufpos = BUFLEN;
							pprev->bcsay.bcs[pprev->bcsay.bcsmax].bufnum = pprev->bufnum;
							pprev->bcsay.bcs[pprev->bcsay.bcsmax].bt = Etag_start;
						}
						else if(b->buf[0]!='!' && b->buf[0]!='?')
						{
							pprev->bcsay.bcs[++(pprev->bcsay.bcsmax)].fileoffset = (b->bufnum -1)*BUFLEN;
							pprev->bcsay.bcs[pprev->bcsay.bcsmax].bufpos = BUFLEN;
							pprev->bcsay.bcs[pprev->bcsay.bcsmax].bufnum = pprev->bufnum;
							pprev->bcsay.bcs[pprev->bcsay.bcsmax].bt = StagorEmptytag_start;

						}
					}
				}
				break;
			}
		}
	}
	//解析小于号
	for(;i<BUFLEN;i++){
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
			else if(i!=BUFLEN-1){    //不是最后一个小于好
				//考虑 <head name="ahdkj"/>,是开始和结束合二为一的标签
				j=i;
				st=0;
				for(i=i+1;i<=BUFLEN;i++)
				{
					if(b->buf[i]=='>')
					{
						if(b->buf[i-1]=='/')
							st=1;
						break;
					}
				}
				if(st==1)
				{
					b->bcsay.bcs[n].fileoffset = (b->bufnum -1)*BUFLEN+j+1;
					b->bcsay.bcs[n].bufpos = j+1;
					b->bcsay.bcs[n].bufnum = b->bufnum;
					b->bcsay.bcs[n].bt = StartAndEnd;
					n++;
				}
				else if(n<=BCSLEN){
					b->bcsay.bcs[n].fileoffset = (b->bufnum -1)*BUFLEN+j+1;
					b->bcsay.bcs[n].bufpos = j+1;
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
			case 6:	printf("the type of this < is StartAndEnd.\n");break;
		}
		printf("the position is %lld.\n",c->bcsay.bcs[i].bufpos);
		printf("the offset is %lld.\n",c->bcsay.bcs[i].fileoffset);
		printf("\n");
	}
	return 1;
}

