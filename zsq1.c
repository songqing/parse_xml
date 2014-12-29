#include<sched.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>
typedef unsigned int uint;

//元素类型
enum tagType{
	StartTag=1,
	EndTag,
	StartPI,
	EndPI,
	StartComment,
	EndComment,
	StartCDATA,
	EndCDATA
};

//BCS
typedef struct _BCS{
	uint			blockInAdr;		//快内地址
	enum tagType	type;			//BCS类型
	struct _BCS		*next;			//下一个BCS
}BCS;

//同一个数据块内BCS表
typedef struct _BCSList{
	BCS				*head;
	uint			count;
}BCSList;

//数据块
typedef struct _Block{
	char 			*l1;			//数据块地址
	uint			len;			//块大小
}Block;

//数据块及其中的事件，即事件划分结果
typedef struct _BlockBCS{
	uint				no;		//块号
	BCSList				bcsl;		//块内BCS表
	Block				block;		//数据块
	struct _BlockBCS	*next;
} BlockBCS;

//划分结果链表
typedef struct _BlockBCSList{
	BlockBCS			*first;		//首个数据块事件划分结果
	uint				count;		//数据块总数
}BlockBCSList;

const uint BLOCK_SIZE=10240;	//l2长度
const char *tag[]=
{
	"StartTag",
	"EndTag",
	"StartPI",
	"EndPI",
	"StartComment",
	"EndComment",
	"StartCDATA",
	"EndCDATA"
};

//新建BCS，并初始化
BCS *createBCS(enum tagType type,uint inAddr,BCS *next){
	BCS* q=(BCS*)malloc(sizeof(BCS));			
	q->type=type;
	q->blockInAdr=inAddr;
	q->next=next;
	return q;
}

//添加划分结果到链表
void addBCS(BCSList *pbcsl, BCS** p,BCS* q){
	if(pbcsl->head!=0){
		(*p)->next=q;
		*p=q;
	}
	else
		pbcsl->head=*p=q;
	++pbcsl->count;
}

//字符串比较
int strCmp(char *str,uint start,uint len,char str2[]){
	uint i=0;
	for(i=0;i<len;++i){
		if(str[start+i]!=str2[i])
			return 0;
	}
	return 1;
}

//分析数据块
BlockBCS *analizeBlock(char l2[],uint l2len,char lastBCS[],uint *lastlen,enum tagType *lasttype){
	BlockBCS *bbcs=(BlockBCS*)malloc(sizeof(BlockBCS));
	bbcs->next=0;

	uint llen=*lastlen;
	//初始化事件链
	BCSList *bcsl=&bbcs->bcsl;
	bcsl->head=0;
	bcsl->count=0;
	BCS *p=0,*q=0;
	
	//开始事件划分
	uint i=0;
	while(i<l2len){
		if(l2[i++]!='<')continue;
	
		//当前字符是'<',判断下一字符
		if(l2[i]!='/' && l2[i]!='?' && l2[i]!='!'){
			q=createBCS(StartTag,llen+i-1,0);
			addBCS(bcsl,&p,q);
			++i;
		}
		else if(l2[i]=='/'){
			q=createBCS(EndTag,llen+i-1,0);
			addBCS(bcsl,&p,q);
			++i;
		}
		else if (l2[i]=='?'){
			q=createBCS(StartPI,llen+i-1,0);

			addBCS(bcsl,&p,q);
			
			++i;
			while(i<l2len){
				if(l2[i]!='?') ++i;
				else if(strCmp(l2,i,2,"?>")==1){
					q=createBCS(EndPI,llen+i,0);

					addBCS(bcsl,&p,q);
					
					i+=2;
					break;
				}
				else
					++i;
			}
		}
		else if(strCmp(l2,i,3,"!--")==1){
			q=createBCS(StartComment,llen+i-1,0);

			addBCS(bcsl,&p,q);

			++i;
			while(i<l2len){
				if(l2[i]!='-') ++i;
				else if(strCmp(l2,i,3,"-->")==1){
					q=createBCS(EndComment,llen+i,0);

					addBCS(bcsl,&p,q);
					
					i+=3;
					break;
				}
				else
					++i;
			}
		}
		else if(strCmp(l2,i,8,"![CDATA[")==1){
			q=createBCS(StartCDATA,llen+i-1,0);

			addBCS(bcsl,&p,q);

			++i;
			while(i<l2len){
				if(l2[i]!=']') ++i;
				else if(strCmp(l2,i,3,"]]>")==1){
					q=createBCS(EndCDATA,llen+i,0); 
					addBCS(bcsl,&p,q);
					
					i+=3;
					break;
				}
				else
					++i;
			}
		}
	}
	--bcsl->count;

	//把上个数据块最后一个BCS添加到当前数据块
	if(llen>0){
		q=createBCS(*lasttype,0,0);
		q->next=bcsl->head;
		bcsl->head=q;
	}

	Block *blk=&bbcs->block;
	
	//为数据块分配存储区
	uint inAdr=p->blockInAdr-llen;		//最后一个事件的块内其实地址	
	uint size=l2len+llen;				//实际分配内存空间大小
	blk->len=p->blockInAdr+1+llen;		//实际字符串长度
	blk->l1=(char*)malloc(sizeof(char)*size);

	//拷贝到新存储区,连接上个数据块最后一个BCS及当前数据块	
	memcpy(blk->l1,lastBCS,llen);		//拷贝上一数据块的最后一个BCS
	memcpy(&blk->l1[llen],l2,l2len);	//拷贝当前数据块
	
	//记录最后一个BCS
	*lastlen=l2len-inAdr;				//最后一个BCS及其后字符串长度
	*lasttype=p->type;
	memcpy(lastBCS,&blk->l1[inAdr],*lastlen);	//拷贝最后一个BCS及其后字符
	
	return bbcs;
}

//释放某数据块所有划分结果
void freeBCSList(BCSList *bcsl){
	//释放事件划分结果 
	BCS *bp,*bq;
	uint count=bcsl->count;
	bp=bcsl->head;
	while(/*count>0 &&*/ bp!=0){
		//打印
		//printf("    事件类型：%-15s  块内地址:%8d\n",tag[bp->type-1],bp->blockInAdr);		


		bq=bp->next;
		free(bp);
		bp=bq;
		--count;
	}
}

//释放数据块及划分结果链表
void freeBlockBCSList(BlockBCSList *bbcsl){
	BlockBCS *p,*q;
	p=bbcsl->first;
	while(p!=0){
		//打印
		//printf("\n***********************************\n块号：%d \n数据块内容：\n\n",p->no);
		//printf("\n***********************************\n块号：%d \n数据块内容：\n%s\n",p->no,p->block.l1);

		q=p->next;
		//释放数据块
		free(p->block.l1);				

		//释放事件划分结果 
		freeBCSList(&p->bcsl);

		//释放当前BlockBCS
		free(p);
		p=q;
	}
}

//第二阶段 处理阶段 多线程实现
void* handle1(void* args);
void* handle2(void* args);
pthread_t phandl1,phandl2;

void main(){
	char path[]="a.xml";
	FILE *f;
	f=fopen(path,"r");
	
	//初始化划分结果链表
	BlockBCSList		bbcsl;
	bbcsl.first=0;
	bbcsl.count=0;

	BlockBCS 			*q,*p;
	p=q=0;
	
	//第一阶段
	char l2[BLOCK_SIZE];					//读入数据块l2
	char lastBCS[BLOCK_SIZE];				//上一数据块的最后一个BCS及气候字符
	uint lastlen=0;							//lastBCS字符数
	enum tagType lasttype=0;				//lastBCS类型
	uint readCount=0;						//读入l2时实际读取的字符数	
	uint blockNo=0;
	
	int flag=0;
	//循环读取数据块
	while((readCount=fread(l2,1,BLOCK_SIZE,f))>0){
		//对数据块进行事件划分，并合并上一数据块的最后一个BCS
		q=analizeBlock(l2,readCount,lastBCS,&lastlen,&lasttype);

		
		//链接划分结果
		if(bbcsl.first!=0){
			p->next=q;
			p=q;
		}
		else 
			bbcsl.first=p=q;

		
		++bbcsl.count;
		q->no=blockNo;
		++blockNo;
	}
	p->block.len+=lastlen;
	++p->bcsl.count;
	fclose(f);

	//第二阶段 多个线程同时进行处理

	int arg;
	arg=1;
	pthread_create(&phandl1,NULL,(void*)handle1,(void*)&arg);
	arg=2;
	pthread_create(&phandl2,NULL,(void*)handle2,(void*)&arg);
	pthread_join(phandl1,NULL);
	pthread_join(phandl2,NULL);
//	exit(1);
		
	//第三阶段


	//释放数据块及划分结果
	freeBlockBCSList(&bbcsl);
}

void* handle1(void* args)
{
	
}
void* handle2(void* args)
{

}
