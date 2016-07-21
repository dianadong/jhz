
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"sharfuct.h"
#include "segment.h"
#include"EntityRecog.h"
#include"WordSvector.h"
#include"math.h"
////////////////////////////////////////////////////////////////
///function:  用户输入插入链表，没有意图描述
//parameter:
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////
int Insattriblist0(siattlist * SL, const char * psenckey, const char * partk, int fqval)
{
	siattlist pn, pk, pr;
	int ln;

	if(psenckey == NULL ||*psenckey == 0)
		return 0;
	pr = NULL;
	pk = *SL;
	while(pk != NULL && fqval < pk->fqval)
	{
		pr = pk;
		pk = pk->pnext;
	}
	pn = (SIATTBNODE *)malloc(sizeof(SIATTBNODE));
	if(pn != NULL)
    {
		ln = 0;
		ln = strlen(psenckey);
		pn->pword = (char *)malloc(ln+1);
		strncpy(pn->pword, psenckey, ln);
		*(pn->pword +ln) = 0;

        pn->fqval = fqval;

		ln = 0;
		ln = strlen(partk);
		pn->partk = (char *)malloc(ln+1);
		strncpy(pn->partk, partk, ln);
		*(pn->partk +ln) = 0;

		pn->score = 1;
		pn->pnext = NULL;

		if(pr == NULL)
		{
			pn->pnext = *SL;
			*SL = pn;
            return 1;
		}
		else
		{
			pn->pnext = pk;
			pr->pnext = pn;
			return 1;
		}
	}
	return 0;
}


////////////////////////////////////////////////////////////////
///function:  得到的intention向量插入链表，有意图描述
//parameter:
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////
int Insattriblist(siattlist * SL, const char * psenckey, const char * partk, int fqval,double score)
{
	siattlist pn, pk, pr;
	int ln;

	if(psenckey == NULL ||*psenckey == 0)
		return 0;
	pr = NULL;
	pk = *SL;//已有链表
	while(pk != NULL && fqval < pk->fqval)
	{
		pr = pk;
		pk = pk->pnext;
	}
	pn = (SIATTBNODE *)malloc(sizeof(SIATTBNODE));
	if(pn != NULL)
    {
		ln = 0;
		ln = strlen(psenckey);
		pn->pword = (char *)malloc(ln+1);
		strncpy(pn->pword, psenckey, ln);
		*(pn->pword +ln) = 0;
		//printf("pn->pword=%s ",pn->pword);

        pn->fqval = fqval;
		//printf("pn->fqval=%d ",pn->fqval);
		ln = 0;
		ln = strlen(partk);
		pn->partk = (char *)malloc(ln+1);
		strncpy(pn->partk, partk, ln);
		*(pn->partk +ln) = 0;
		//printf("pn->partk=%s\n",pn->partk);
		pn->score =score;
		pn->pnext = NULL;

		if(pr == NULL)
		{
			pn->pnext = *SL;
			*SL = pn;
            return 1;
		}
		else
		{
			pn->pnext = pk;
			pr->pnext = pn;
			return 1;
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////
///function:释放链表
//parameter:
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////
void freeAtrList(siattlist * SL)
{
	siattlist pa, pk;
	if(*SL == NULL)
		return ;
    pa = NULL;
	pk = *SL;
	while(pk != NULL)
	{
		pa = pk;
		pk = pk->pnext;

		if(pa->pword != NULL)
		{
			free(pa->pword);
			pa->pword = NULL;
		}

		if(pa->partk != NULL)
		{
			free(pa->partk);
			pa->partk = NULL;
		}
		pa->fqval = 0;
		pa->score = 0.001;

		free(pa);
		pa = NULL;
	}
	*SL =NULL;
}
/////////////////////////////////////////////////////
///function://将意图词写入txt文档
//parameter:
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////
void writlist2r(siattlist slist, FILE * fw)
{
	siattlist pk;

	if(slist == NULL)
	{
		return ;
	}
	pk = slist;
	while(pk != NULL)
	{
		fprintf(fw, "%s,%s,%d,%0.1f\t", pk->pword, pk->partk, pk->fqval,pk->score);
		pk = pk->pnext;
	}
}
/////////////////////////////////////////////////////
///function:  列表转换
//parameter:
//author: wuxiaoqi
//time: 2016-5-9
/////////////////////////////////////////////////////
char * wlist2wkstr(siattlist slist, char * pwktres)
{
	siattlist pk;
	char xlwordem[256];

	if(slist == NULL)
		return pwktres;
	pk = slist;
	while(pk != NULL)
	{
		xlwordem[0] = 0;

		sprintf(xlwordem, "%s,%s,%d\t", pk->pword, pk->partk, pk->fqval);

		strcat(pwktres, xlwordem);
		pk = pk->pnext;
	}
	return pwktres;
}
/////////////////////////////////////////////////////
///function:  wfvhash转换成list，并按词频排序
//parameter:
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////
int wfvhash2list(hashtable wsvhash, siattlist * SL)
{
	long i;
	int znum = 0;
	if(wsvhash == NULL)
		return znum;

	for(i = 0; i<DCHASH_SIZE; i++)
	{
		if(	(wsvhash+i)->bExists != 0)
		{

			znum += Insattriblist0(SL, (wsvhash+i)->pentityname, (wsvhash+i)->pidcode, (wsvhash+i)->fqval);//SL链表：实体名，词性，词频
		}
	}
	return znum;
}

//////////////////////////////////////////////////////////////
///function:  切分后的词语按照场景各个intent句集合 进hash
//parameter:
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////////////
int intent_aggVectorW(hashtable ewhash, const char * SIaggpath,char **stop,int nStop,FILE *fw)
{
	hashtable wsvhash = NULL;
    FILE *fp;
	char * pbuf, *p, *pseg, *q, *s,* scekey,* pstop;
	char wordk[256];
	int len, ren;
	long lbitk = -1, lartk;
    int ires = -1, intenval;
    siattlist sxlist = NULL,sortlist = NULL;
	//SIATTBNODE * temp = NULL;

	if(SIaggpath == NULL || *SIaggpath == 0)
		return -1;
	/*if(krespath == NULL || *krespath == 0)
		return -1;*/

	fp = NULL;
	fp = fopen(SIaggpath, "rb");
	if(fp == NULL)
	    return -1;

    /*fw = NULL;
	fw = fopen(krespath, "wb");
	if(fw == NULL)
	{
		fclose(fw);
		return -1;
	}
	*/
    pbuf = (char *)malloc(1<<10);
    pseg = (char *)malloc(1<<12);
	scekey = (char *)malloc(1 << 10);
	*pbuf = 0;
	*pseg = 0;
	*scekey = 0;

	if (stop == NULL)
		return -1;


    Init_mghash(&wsvhash);
	
	intenval = 1;
	
	if (!fgets(scekey, 5, fp))          //读取场景标识
	{
		printf("ERROR:Can not read the scene ID!");
		return -1;
	}
	fgets(pbuf, 1 << 10, fp);         //滤掉换行符
	*pbuf = 0;
    while(fgets(pbuf, 1<<10, fp))        //循环意图语句
	{
		//printf("pbuf=%s\n", pbuf);
		len = 0;
		len = GetRealString(pbuf);

		if(len <= 0)
			continue;
        p = NULL;
		p = strstr(pbuf, "####");            //strstr()函数用来检索子串在字符串中首次出现的位置
		//检测到意图标识
		if(p != NULL)       
		{
			ires = 0;
            ires = wfvhash2list(wsvhash, &sxlist);
			fprintf(fw, "%s####%d\t", scekey, intenval);
            writlist2r(sxlist, fw);
			fprintf(fw, "\n");
            freeAtrList(&sxlist);
			if(wsvhash != NULL)
				free_mghash(&wsvhash);
            Init_mghash(&wsvhash);
			++intenval;
			continue;
		}
		//检测到意图语句
		ren = 1024;
		*pseg = 0;
		ires = 0;
        ires = repqiefen(pbuf, len, pseg, ren);//切分
		
        q = pseg;
		while(*q != 0)
		{
			lbitk = -1;
			s = NULL;
			s = strchr(q, 0x20);//查找首次出现空格的位置

			if(s != NULL)
			{
				len = 0;
		        len = s - q;
                wordk[0] = 0;
                strncpy(wordk, q, len);
				wordk[len] = 0;
				///////////////////////////////
				//去除停用词
				///////////////////////////////
				pstop = NULL;
				pstop = wordk;
				removeStopWord(&pstop, stop, nStop);

				if (pstop != '\0')
				{
					lbitk = GetHashmg(wsvhash, wordk);//本hash表中查找
					if (lbitk >= 0)
					{
						(wsvhash + lbitk)->fqval += 1;//找到，词频加1
					}
					else
					{
						ires = 0;
						lartk = -1;
						lartk = GetHashmg(ewhash, wordk);//大hash中查找
						if (lartk >= 0)
							ires = Ins3Hashmg(&wsvhash, wordk, (ewhash + lartk)->pidcode, 1);//插入新的元素进入hash表
						else
							ires = Ins3Hashmg(&wsvhash, wordk, "NFart", 1);
					}
				}
				q = s + 1;
			  }
				
			else      //最后一个词
			{
				removeStopWord(&q,stop, nStop);

				if (q != NULL)
				{
					lbitk = GetHashmg(wsvhash, q);
					if (lbitk >= 0)
					{
						(wsvhash + lbitk)->fqval += 1;
					}
					else
					{
						ires = 0;
						lartk = -1;
						lartk = GetHashmg(ewhash, q);
						if (lartk >= 0)
							ires = Ins3Hashmg(&wsvhash, q, (ewhash + lartk)->pidcode, 1);
						else
							ires = Ins3Hashmg(&wsvhash, q, "NFart", 1);
					}

				}
				
				break;
			}
		}
        *pbuf = 0;
	}
	ires = 0;
	
    ires = wfvhash2list(wsvhash, &sxlist);  
	/*
	//转换成列表，按词频排序
	temp = (SIATTBNODE *)malloc(sizeof(SIATTBNODE));
	temp = sxlist;
	while (temp != NULL)
	{
		
		SortInsert(&sortlist, temp);
		temp = temp->pnext;
	}
	
	free(temp);
	*/
	fprintf(fw, "%s####%d\t", scekey, intenval);
	writlist2r(sxlist, fw);
	//writlist2r(sortlist, fw);
	fprintf(fw, "\n");

	freeAtrList(&sxlist);
	//freeAtrList(&sortlist);

    if(wsvhash != NULL)
		free_mghash(&wsvhash);
    //fclose(fw);
	fclose(fp);
    free(pseg);
	pseg = NULL;
    free(pbuf);
	pbuf = NULL;

	return 0;
}

/////////////////////////////////////////////////////////////
///function:  用户query 向量特征表示
//parameter:
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////////////
int uqueryVecfeat(hashtable ewhash, const char * puqkey, char * presvfeat)
{
	hashtable wsvhash = NULL;
	char * pseg, *q, *s;
	char wordk[256];
	int len, ren, ires;
	long lbitk = -1, lartk;
	siattlist sxlist = NULL;//结果存到list中

	pseg = (char *)malloc(1024);

    if(puqkey == NULL || *puqkey == 0)
		return -1;
    Init_mghash(&wsvhash);
    len= strlen(puqkey);
	ren = 1024;
	*pseg = 0;
	ires = 0;
    ires = repqiefen((char *)puqkey, len, pseg, ren);//切分

	q = pseg;
	while(*q != 0)
	{
		s = NULL;
		s = strchr(q, 0x20);
		if(s != NULL)
		{
			len = 0;
		    len = s - q;
            wordk[0] = 0;
            strncpy(wordk, q, len);
			wordk[len] = 0;
			lbitk = -1;
			lbitk = GetHashmg(wsvhash, wordk);//从大表中查找
            if(lbitk >= 0)
			{
				(wsvhash+lbitk)->fqval += 1;//词频+1
			}
			else
			{
				lartk = -1;
				lartk = GetHashmg(ewhash, wordk);//重新装入hash
				if(lartk >= 0)
					ires = Ins3Hashmg(&wsvhash, wordk, (ewhash+lartk)->pidcode, 1);
				else
					ires = Ins3Hashmg(&wsvhash, wordk, "NFart", 1);
			}
			q = s +1;
		}
		else
		{
			lbitk = -1;
            lbitk = GetHashmg(wsvhash, q);
            if(lbitk >= 0)
			{
				(wsvhash+lbitk)->fqval += 1;
			}
			else
			{
				lartk = -1;
				lartk = GetHashmg(ewhash, q);
				if(lartk >= 0)
					ires = Ins3Hashmg(&wsvhash, q, (ewhash+lartk)->pidcode, 1);
				else
					ires = Ins3Hashmg(&wsvhash, q, "NFart", 1);
            }
			break;
		}
	}
    ires = 0;
    ires = wfvhash2list(wsvhash, &sxlist);//hash转换成list
    presvfeat = wlist2wkstr(sxlist, presvfeat);
    if(wsvhash != NULL)
		free_mghash(&wsvhash);
    return 0;
}
///////////////////////////////////////////////////////////////
///function: 给单独intention词向量集合建立一个list
//parameter:
//author: 
//time: 2016-6-2
///////////////////////////////////////////////////////////////

siattlist * createlist(const char *pbuf, siattlist *SL)
{
   char * pa,* s,* temp,* psenckey,* partk,* number1,* number2,* jhtstr;
   int len,fval,i;
   double score;

   jhtstr = (char *)malloc(1<<14);
   *jhtstr = 0;

   psenckey = (char *)malloc(1<<14);
   *psenckey = 0;
   partk = (char *)malloc(1<<14);
   *partk = 0;
   number1 = (char *)malloc(1<<14);
   *number1 = 0;
   number2 = (char *)malloc(1<<14);
   *number2 = 0;


   if(pbuf == NULL || *pbuf == 0)
	   return SL;

   pa = (char *) pbuf;//整串
  
   while( *pa != 0 && strlen(pa) > 1)
	{
		    sourceqiefen(& pa,0x09,jhtstr);//'\t'
		   //printf("%s\n",jhtstr);
		    temp=NULL;
		    temp=jhtstr;
		   //词，词性，词频，p（词/意图）放入链表中
		    sourceqiefen(&temp,0x2C,psenckey);//','
		    sourceqiefen(&temp, 0x2C, partk);
			   //printf("partk=%s\n",partk);
		    sourceqiefen(&temp, 0x2C, number1);
			fval = atoi(number1);
			sourceqiefen(&temp, 0x09, number2);
			score = atof(number2);

			Insattriblist(SL, psenckey, partk, fval, score);//插入链表
		}

     free(psenckey);
	 free(partk);
	 free(number1);
	 free(number2);
	 free(jhtstr);

     psenckey  = NULL;
     partk = NULL;
	 number1 = NULL;
	 number2 = NULL;
	 jhtstr=NULL;

	 return SL;

}
///////////////////////////////////////////////////////////////
///function: 打印list
//parameter:
//author: 
//time: 2016-6-2
///////////////////////////////////////////////////////////////
void print(siattlist *SL)
{
	if(SL == NULL)
	{
		printf("The list is empty!\n");

	}

    siattlist p = NULL;
	p = * SL;
    printf("The list of intention is:\n");


    do
      {
         printf("%s %s %d %f\n",p->pword,p->partk,p->fqval,p->score);

         p = p->pnext;
      }while(p != NULL);
}

///////////////////////////////////////////////////////////////
///function:   链表查找用户节点，返回节点位置
//parameter:
//author: 
//time: 2016-6-5
//////////////////////////////////////////////////////////////
SIATTBNODE * LocateNode(const char *key ,const siattlist intentL)
{
    siattlist srclist;

    if(key == NULL)
    {
	    printf("This word of userL is empty!\n");
		return NULL;
	}
	if(intentL == NULL)
    {
	    printf("This intention list is empty!\n");
		return NULL;
	}
	srclist = intentL;

    //print(&srclist);
	while(srclist != NULL )
    {
       if(strcmp(key,srclist->pword)==0)
          break;
        srclist = srclist->pnext;
    }
    return srclist;
 }

///////////////////////////////////////////////////////////////
///function:词权重计算，然后累加
//parameter:
//author: 
//time: 2016-6-5
//////////////////////////////////////////////////////////////
void ComputeWeight(const SIATTBNODE *Lnode, const char * pidcode,const int fval, double *degreeA, double *degreeAB)
{
	
	double weight=0.0000001,s = 0;

	//第一种计算方式：权重分配
	//double p,q,r,userscore = 10;
	//p = 0.9;//词实体
	//q = 0.05;//词性
	//r = 0.05;//词频

	if(Lnode !=NULL)
    {
        printf(" %s ",Lnode->pword);//匹配到词实体
        s = Lnode->score;
		//第一种计算方式
		//weight = log10(s) * log10(userscore) * ( p + q + r*(double(fval) / double(Lnode->fqval)));
        //第二种计算方式
		weight = (double(10 + atoi(Lnode->partk) + fval) + Lnode->score)*(double(10 + atoi(Lnode->partk) + fval) + Lnode->score);

        *degreeAB= *degreeAB + weight;
		*degreeA = *degreeA + weight;
    }
	else
		//第一种计算公式
	     //*degreeA= *degreeA + 1;
		//第二种计算公式
		*degreeA = *degreeA + (double(10 + atoi(pidcode) + fval) + 0) * (double(10 + atoi(pidcode) + fval) + 0);
    
}
///////////////////////////////////////////////////////////////
///function:用户向量与意图向量相似度计算,采用用户节点与意图链表的比较
//parameter:
//author:
//time: 2016-6-3
//////////////////////////////////////////////////////////////

double VectorSimil(const entlist userL, const siattlist intentL)

{
	double simvalA = 0.0000001,simvalB = 0.0000001,degreeB = 0.0000001;
	double * degreeA,* degreeAB ;
	ENTNODE *pa;
	SIATTBNODE *keynode;
	siattlist qa;
    degreeA = &simvalA;
    degreeAB = &simvalB;

	if(userL == NULL||intentL == NULL)
		return simvalA;
	
	pa = userL;
	qa = intentL;
	int count = 0;

	while(qa!=NULL)//|B|
	{
		//第一种计算公式
		//degreeB = degreeB +log10(qa->score) * log10(qa->score);
		//第二种计算公式
		
		degreeB = degreeB + (double(10 + atoi(qa->partk) + qa->fqval) + qa->score)*(double(10 + atoi(qa->partk) + qa->fqval) + qa->score);
		
		qa = qa->pnext;
		 
	}
	printf("match successfully:");
	while(pa != NULL)
	{
         keynode = LocateNode(pa->pentityk,intentL);

         ComputeWeight(keynode,pa->pidcode,pa->startsit,degreeA,degreeAB);
         pa = pa->pnext;
	}
	printf("\n");
	return 2*(*degreeAB)/(sqrt(*degreeA) * sqrt(degreeB));
}
///////////////////////////////////////////////////////////////
///function:建立停用词表
//parameter:
//author:
//time: 2016-6-16
//////////////////////////////////////////////////////////////
int readStopLib(char *f, char **stop)
{
	FILE *fstop;
	char buf[MAXCHARS];           
	int line = 0,L,n = 0;
	
	if (NULL == (fstop = fopen(f, "r")))
	{
		fprintf(stderr, "Can not open file:%s\n", f);
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		if (NULL == fgets(buf, 256, fstop)) break;
		line++;                    //Line记录停用词文件的行数
		L = strlen(buf) - 1;        //停用词长度

		while (1)                  //去掉行尾的'\n'和空格
		{
			if ('\n' == buf[L]) {
				buf[L] = 0;
				L--;
				if (L < 0) break;
			}
			else break;
		}
		if (L >= 0)
		{
			stop[n] = (char*)malloc(sizeof(char)*(L + 2));
			strcpy(stop[n], buf);
			n++;              //n记录实际读入的停用词数
		}
	}
	fclose(fstop);
	return n;
}
///////////////////////////////////////////////////////////////
///function:去除停用词
//parameter:
//author: 
//time: 2016-6-16
///////////////////////////////////////////////////////////////
void removeStopWord(char **src,char **stop,const int nStop)
{
	int i = 0;
	 
	if (*src == NULL)
	{
		printf("Error:The word of src is empty!\n");
		//return *src;
	}

	while (*src != NULL && i < nStop)    //单个词与停用词比较
	{
		//printf("stop0=%s\n",stop[i]);
		if (strcmp(*src, stop[i]) == 0)
		{
			*src = '\0';
			break;
		}
		else
			i++;	
	}
	//return *src;
}






