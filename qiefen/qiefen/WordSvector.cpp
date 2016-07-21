
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"sharfuct.h"
#include "segment.h"
#include"EntityRecog.h"
#include"WordSvector.h"
////////////////////////////////////////////////////////////////
///function:  
//parameter: 
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////
int Insattriblist(siattlist * SL, const char * psenckey, const char * partk, int fqval)
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
		
		pn->score = 0.00001;
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
///function:  
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
///function:  
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
		fprintf(fw, "%s,%s,%d\t", pk->pword, pk->partk, pk->fqval);
		pk = pk->pnext;
	}
}
/////////////////////////////////////////////////////
///function:  
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
///function:  
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
			znum += Insattriblist(SL, (wsvhash+i)->pentityname, (wsvhash+i)->pidcode, (wsvhash+i)->fqval);
		}
	}
	return znum;
}

//////////////////////////////////////////////////////////////
///function:  按照场景各个intent句集合 进hash
//parameter: 
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////////////
int intent_aggVectorW(hashtable ewhash, const char * SIaggpath, const char * krespath, const char * scekey)
{
	hashtable wsvhash = NULL;
    FILE *fp, *fw;
	char * pbuf, *p, *pseg, *q, *s;
	char wordk[256];
	int len, ren;
	long lbitk = -1, lartk;
    int ires = -1, intenval;
    siattlist sxlist = NULL;


	if(SIaggpath == NULL || *SIaggpath == 0)
		return -1;
	if(krespath == NULL || *krespath == 0)
		return -1;
	fp = NULL;
	fp = fopen(SIaggpath, "rb");
	if(fp == NULL)
	    return -1;

    fw = NULL;
	fw = fopen(krespath, "wb");
	if(fw == NULL)
	{
		fclose(fw);
		return -1;
	}

    pbuf = (char *)malloc(1<<10);
    pseg = (char *)malloc(1<<12);
	*pbuf = 0;
	*pseg = 0;

    Init_mghash(&wsvhash);
	intenval = 1;
    while(fgets(pbuf, 1<<10, fp))
	{
		len = 0;
		len = GetRealString(pbuf);
        
		if(len <= 0)
			continue;
        p = NULL;
		p = strstr(pbuf, "####");
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
		ren = 1024;
		*pseg = 0;
		ires = 0;
        ires = repqiefen(pbuf, len, pseg, ren);
        q = pseg;
		while(*q != 0)
		{
			lbitk = -1;
			s = NULL;
			s = strchr(q, 0x20);
			if(s != NULL)
			{
				len = 0;
		        len = s - q;
                wordk[0] = 0;
                strncpy(wordk, q, len);
				wordk[len] = 0;
                
				lbitk = GetHashmg(wsvhash, wordk);
                if(lbitk >= 0)
				{
					(wsvhash+lbitk)->fqval += 1;
				}
				else
				{
					ires = 0;
					lartk = -1;
					lartk = GetHashmg(ewhash, wordk);
					if(lartk >= 0)
						ires = Ins3Hashmg(&wsvhash, wordk, (ewhash+lartk)->pidcode, 1);
					else
						ires = Ins3Hashmg(&wsvhash, wordk, "NFart", 1);
				}

				q = s +1;
			}
			else
			{
				lbitk = GetHashmg(wsvhash, q);
				if(lbitk >= 0)
				{
					(wsvhash+lbitk)->fqval += 1;
				}
				else
				{
					ires = 0;
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
        *pbuf = 0;
	}
	ires = 0;
    ires = wfvhash2list(wsvhash, &sxlist);
	fprintf(fw, "%s####%d\t", scekey, intenval);
	writlist2r(sxlist, fw);
	fprintf(fw, "\n");
	freeAtrList(&sxlist);
    if(wsvhash != NULL)
		free_mghash(&wsvhash);
    fclose(fw);
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
	siattlist sxlist = NULL;

	pseg = (char *)malloc(1024);

    if(puqkey == NULL || *puqkey == 0)
		return -1;
    Init_mghash(&wsvhash);
    len= strlen(puqkey);
	ren = 1024;
	*pseg = 0;
	ires = 0;
    ires = repqiefen((char *)puqkey, len, pseg, ren);

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
			lbitk = GetHashmg(wsvhash, wordk);
            if(lbitk >= 0)
			{
				(wsvhash+lbitk)->fqval += 1;
			}
			else
			{
				lartk = -1;
				lartk = GetHashmg(ewhash, wordk);
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
    ires = wfvhash2list(wsvhash, &sxlist);
    presvfeat = wlist2wkstr(sxlist, presvfeat);
    if(wsvhash != NULL)
		free_mghash(&wsvhash);
    return 0;
}
///////////////////////////////////////////////////////////////
///function:   两个向量特征相似计算
//parameter: 
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////////////
double VectorSimil(const char * pvecaggA, const char * pvecaggB)
{
	double simval = 0.0000001;
    char * pa, *qa, *p, *q;
	int len, wen, afval, bfval;
    
	char jhtstr[512], uxlstr[512], aVkey[256], bVkey[256], apart[32], bpart[32];


	if(pvecaggA != NULL || *pvecaggA == 0)
		return simval;
	if(pvecaggB != NULL || *pvecaggB == 0)
		return simval;
	pa = (char *)pvecaggA;
	while(*pa != 0)
	{
        qa = NULL;
	    qa = strchr(pa, 0x09);
		if(qa != NULL)
		{
			len = 0;
			len = qa - pa;
			jhtstr[0] = 0;
			strncpy(jhtstr, pa, len);
			jhtstr[len] = 0;
			//======

            p = (char *)pvecaggB;
			while(*p != 0)
			{
				q = NULL;
				q = strchr(p, 0x09); 
				if( q != NULL)
				{
					len = 0;
					len = q - p;
					uxlstr[0] = 0;
					strncpy(uxlstr, p, len);
					uxlstr[len] = 0;

					p = q + 1;
				}
				else
				{
					break;
				}
			}
			pa = qa + 1;
		}
		else
		{
			//pa == 
            p = (char *)pvecaggB;
			while(*p != 0)
			{
				q = NULL;
				q = strchr(p, 0x09); 
				if( q != NULL)
				{
					len = 0;
					len = q - p;
					uxlstr[0] = 0;
					strncpy(uxlstr, p, len);
					uxlstr[len] = 0;

					p = q + 1;
				}
				else
				{
					break;
				}
			}
			break;
		}
	}

    return simval;
}

///////////////////////////////////////////////////////////////
///function: 循环计算意图集合和语句向量符合度
//parameter: 
//author: wuxiaoqi
//time: 2016-5-9
///////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////
///function:   本次意图参选分析
//parameter: 
//author: wuxiaoqi
//time: 2016-5-9
//////////////////////////////////////////////////////////////








