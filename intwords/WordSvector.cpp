
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"sharfuct.h"
#include "segment.h"
#include"EntityRecog.h"
#include"WordSvector.h"
#include"math.h"
////////////////////////////////////////////////////////////////
///function:  �û������������û����ͼ����
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
///function:  �õ���intention����������������ͼ����
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
	pk = *SL;//��������
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
///function:�ͷ�����
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
///function://����ͼ��д��txt�ĵ�
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
///function:  �б�ת��
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
///function:  wfvhashת����list��������Ƶ����
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

			znum += Insattriblist0(SL, (wsvhash+i)->pentityname, (wsvhash+i)->pidcode, (wsvhash+i)->fqval);//SL����ʵ���������ԣ���Ƶ
		}
	}
	return znum;
}

//////////////////////////////////////////////////////////////
///function:  �зֺ�Ĵ��ﰴ�ճ�������intent�伯�� ��hash
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
	
	if (!fgets(scekey, 5, fp))          //��ȡ������ʶ
	{
		printf("ERROR:Can not read the scene ID!");
		return -1;
	}
	fgets(pbuf, 1 << 10, fp);         //�˵����з�
	*pbuf = 0;
    while(fgets(pbuf, 1<<10, fp))        //ѭ����ͼ���
	{
		//printf("pbuf=%s\n", pbuf);
		len = 0;
		len = GetRealString(pbuf);

		if(len <= 0)
			continue;
        p = NULL;
		p = strstr(pbuf, "####");            //strstr()�������������Ӵ����ַ������״γ��ֵ�λ��
		//��⵽��ͼ��ʶ
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
		//��⵽��ͼ���
		ren = 1024;
		*pseg = 0;
		ires = 0;
        ires = repqiefen(pbuf, len, pseg, ren);//�з�
		
        q = pseg;
		while(*q != 0)
		{
			lbitk = -1;
			s = NULL;
			s = strchr(q, 0x20);//�����״γ��ֿո��λ��

			if(s != NULL)
			{
				len = 0;
		        len = s - q;
                wordk[0] = 0;
                strncpy(wordk, q, len);
				wordk[len] = 0;
				///////////////////////////////
				//ȥ��ͣ�ô�
				///////////////////////////////
				pstop = NULL;
				pstop = wordk;
				removeStopWord(&pstop, stop, nStop);

				if (pstop != '\0')
				{
					lbitk = GetHashmg(wsvhash, wordk);//��hash���в���
					if (lbitk >= 0)
					{
						(wsvhash + lbitk)->fqval += 1;//�ҵ�����Ƶ��1
					}
					else
					{
						ires = 0;
						lartk = -1;
						lartk = GetHashmg(ewhash, wordk);//��hash�в���
						if (lartk >= 0)
							ires = Ins3Hashmg(&wsvhash, wordk, (ewhash + lartk)->pidcode, 1);//�����µ�Ԫ�ؽ���hash��
						else
							ires = Ins3Hashmg(&wsvhash, wordk, "NFart", 1);
					}
				}
				q = s + 1;
			  }
				
			else      //���һ����
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
	//ת�����б�����Ƶ����
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
///function:  �û�query ����������ʾ
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
	siattlist sxlist = NULL;//����浽list��

	pseg = (char *)malloc(1024);

    if(puqkey == NULL || *puqkey == 0)
		return -1;
    Init_mghash(&wsvhash);
    len= strlen(puqkey);
	ren = 1024;
	*pseg = 0;
	ires = 0;
    ires = repqiefen((char *)puqkey, len, pseg, ren);//�з�

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
			lbitk = GetHashmg(wsvhash, wordk);//�Ӵ���в���
            if(lbitk >= 0)
			{
				(wsvhash+lbitk)->fqval += 1;//��Ƶ+1
			}
			else
			{
				lartk = -1;
				lartk = GetHashmg(ewhash, wordk);//����װ��hash
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
    ires = wfvhash2list(wsvhash, &sxlist);//hashת����list
    presvfeat = wlist2wkstr(sxlist, presvfeat);
    if(wsvhash != NULL)
		free_mghash(&wsvhash);
    return 0;
}
///////////////////////////////////////////////////////////////
///function: ������intention���������Ͻ���һ��list
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

   pa = (char *) pbuf;//����
  
   while( *pa != 0 && strlen(pa) > 1)
	{
		    sourceqiefen(& pa,0x09,jhtstr);//'\t'
		   //printf("%s\n",jhtstr);
		    temp=NULL;
		    temp=jhtstr;
		   //�ʣ����ԣ���Ƶ��p����/��ͼ������������
		    sourceqiefen(&temp,0x2C,psenckey);//','
		    sourceqiefen(&temp, 0x2C, partk);
			   //printf("partk=%s\n",partk);
		    sourceqiefen(&temp, 0x2C, number1);
			fval = atoi(number1);
			sourceqiefen(&temp, 0x09, number2);
			score = atof(number2);

			Insattriblist(SL, psenckey, partk, fval, score);//��������
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
///function: ��ӡlist
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
///function:   ��������û��ڵ㣬���ؽڵ�λ��
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
///function:��Ȩ�ؼ��㣬Ȼ���ۼ�
//parameter:
//author: 
//time: 2016-6-5
//////////////////////////////////////////////////////////////
void ComputeWeight(const SIATTBNODE *Lnode, const char * pidcode,const int fval, double *degreeA, double *degreeAB)
{
	
	double weight=0.0000001,s = 0;

	//��һ�ּ��㷽ʽ��Ȩ�ط���
	//double p,q,r,userscore = 10;
	//p = 0.9;//��ʵ��
	//q = 0.05;//����
	//r = 0.05;//��Ƶ

	if(Lnode !=NULL)
    {
        printf(" %s ",Lnode->pword);//ƥ�䵽��ʵ��
        s = Lnode->score;
		//��һ�ּ��㷽ʽ
		//weight = log10(s) * log10(userscore) * ( p + q + r*(double(fval) / double(Lnode->fqval)));
        //�ڶ��ּ��㷽ʽ
		weight = (double(10 + atoi(Lnode->partk) + fval) + Lnode->score)*(double(10 + atoi(Lnode->partk) + fval) + Lnode->score);

        *degreeAB= *degreeAB + weight;
		*degreeA = *degreeA + weight;
    }
	else
		//��һ�ּ��㹫ʽ
	     //*degreeA= *degreeA + 1;
		//�ڶ��ּ��㹫ʽ
		*degreeA = *degreeA + (double(10 + atoi(pidcode) + fval) + 0) * (double(10 + atoi(pidcode) + fval) + 0);
    
}
///////////////////////////////////////////////////////////////
///function:�û���������ͼ�������ƶȼ���,�����û��ڵ�����ͼ����ıȽ�
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
		//��һ�ּ��㹫ʽ
		//degreeB = degreeB +log10(qa->score) * log10(qa->score);
		//�ڶ��ּ��㹫ʽ
		
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
///function:����ͣ�ôʱ�
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
		line++;                    //Line��¼ͣ�ô��ļ�������
		L = strlen(buf) - 1;        //ͣ�ôʳ���

		while (1)                  //ȥ����β��'\n'�Ϳո�
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
			n++;              //n��¼ʵ�ʶ����ͣ�ô���
		}
	}
	fclose(fstop);
	return n;
}
///////////////////////////////////////////////////////////////
///function:ȥ��ͣ�ô�
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

	while (*src != NULL && i < nStop)    //��������ͣ�ôʱȽ�
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






