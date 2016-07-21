
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"sharfuct.h"
#include"EntityRecog.h"

///////////////////////////////////////////////////////////
//function:初始化hash表 
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
void Init_mghash(hashtable * HashMg)
{
	long i;
	prepareCryptTable();
	* HashMg = (HASHMANGNODE*)malloc(sizeof(HASHMANGNODE) * DCHASH_SIZE);
	if(* HashMg ==NULL)
		return ;
	
	for(i = 0; i<DCHASH_SIZE; i++)
	{
		((*HashMg)+i)->bExists = 0;
		((*HashMg)+i)->nHashA = -1;
		((*HashMg)+i)->nHashB = -1;
        
		((*HashMg)+i)->pentityname = NULL;
		((*HashMg) + i)->gnum = 0;
		((*HashMg)+i)->slist = NULL;
        
	}
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
	if (*SL == NULL)
		return;
	pa = NULL;
	pk = *SL;
	while (pk != NULL)
	{
		pa = pk;
		pk = pk->pnext;

		if (pa->scene != NULL)
		{
			free(pa->scene);
			pa->scene = NULL;
		}

		pa->intent = 0;
		pa->score = 0.001;

		free(pa);
		pa = NULL;
	}
	*SL = NULL;
}
///////////////////////////////////////////////////////////
//function:释放hash表 
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
void free_mghash(hashtable * HashMg)
{
	long i;
	if(* HashMg == NULL)
	{
		HashMg = NULL;
		return ;
	}
	for(i = 0; i<DCHASH_SIZE; i++)
	{
		if(	((*HashMg)+i)->bExists != 0)
		{
			((*HashMg)+i)->bExists = 0;
			((*HashMg)+i)->nHashA = -1;
			((*HashMg)+i)->nHashB = -1;

			if(((*HashMg)+i)->pentityname != NULL)
			{
				free(((*HashMg)+i)->pentityname);
				((*HashMg)+i)->pentityname = NULL;
			}
			if (((*HashMg) + i) ->slist != NULL )
			{
				freeAtrList(&((*HashMg) + i)->slist);
			}
			
		    
		}
	}
	free(*HashMg);
	*HashMg = NULL;
}
////////////////////////////打印list
void print(siattlist *SL, FILE * fw)
{
	if (SL == NULL)
	{
		printf("The list is empty!\n");

	}
	
	if (fw == NULL)
		return;
	siattlist p = NULL;
	

	p = *SL;
	//printf("The list of intention is:\n");


	do
	{
		fprintf(fw, "<%s,%d,%.1f > ", p->scene, p->intent, p->score);
		p = p->pnext;
	} while (p != NULL);

	fprintf(fw,"\n");
}
///////////////////////////////////////////////////////////
//function:  打印hash链表
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
void writ_mghash(hashtable hashmg, const char * pnduppath)
{
	FILE * fw;
	long i;
	if(pnduppath == NULL || * pnduppath== 0)
		return ;
    fw = fopen(pnduppath, "wb");
	if(fw == NULL)
		return ;

	if(hashmg == NULL)
		return ;

    for(i = 0; i<DCHASH_SIZE; i++)
	{
		if(	(hashmg+i)->bExists != 0)
		{
			if ((hashmg + i)->pentityname != NULL)
			{
				fprintf(fw, "%s: ", (hashmg + i)->pentityname);
				print(&(hashmg + i)->slist, fw);
			}
				
		}
	}
	fclose(fw);
}

///////////////////////////////////////////////////////////
//function: 查找元素在hash表中
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
long GetHashmg(hashtable hash_mg, const char *pmkey)
{
	const unsigned long HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
	unsigned long nHash = HashString(pmkey, HASH_OFFSET);
	unsigned long nHashA = HashString(pmkey, HASH_A);
	unsigned long nHashB = HashString(pmkey, HASH_B);
	unsigned long nHashStart = nHash % DCHASH_SIZE, nHashPos = nHashStart;
	
	while((hash_mg +nHashPos)->bExists)
	{
		if((hash_mg +nHashPos)->nHashA == (long) nHashA && (hash_mg +nHashPos)->nHashB == (long) nHashB)
			return nHashPos;
		else
			nHashPos = (nHashPos + 1)%DCHASH_SIZE;
		if(nHashPos == nHashStart)
			break;
	}
	return -1; //没有找到
}


///////////////////////////////////////////////////////////
//function: 尾插法-插入场景、意图、描述程度到hash位置所在的list
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
int Insertsiattlist(siattlist * SL, char * scene, int intid, double score)
{
	siattlist pn, pk, pr;
	int ln;

	if (scene == NULL || *scene == 0 || intid ==0)
		return 0;
	pr = NULL;
	pk = *SL;
	while (pk != NULL)
	{
		///list查重
		pr = pk;
		if (strcmp(scene, pr->scene) == 0 && intid == pr->intent)
		{
			printf("不是有效的");
			return 0;
		}
		else
		   pk = pk->pnext;
	}
	pn = (SIATTBNODE *)malloc(sizeof(SIATTBNODE));
	if (pn != NULL)
	{
		ln = 0;
		ln = strlen(scene);
		pn->scene = (char *)malloc(ln + 1);
		strncpy(pn->scene, scene, ln);
		*(pn->scene + ln) = 0;
		
		pn->intent = intid;
		pn->score = score;
		pn->pnext = NULL;
		printf("scene = %s,intent = %d,score = %f\n",pn->scene,pn->intent,pn->score);
		if (pr == NULL)
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

///////////////////////////////////////////////////////////
//function: 插入新的元素进入hash表：word：list
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
int Ins4Hashmg(hashtable * HashMg, const char *pmkey, char * scene, int intid, double score)
{
	int ln,ires;
	
	const unsigned long HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
	unsigned long nHash = HashString(pmkey, HASH_OFFSET);
	unsigned long nHashA = HashString(pmkey, HASH_A);
	unsigned long nHashB = HashString(pmkey, HASH_B);
	unsigned long nHashStart = nHash%DCHASH_SIZE, nHashPos = nHashStart;
	while(((*HashMg)+nHashPos)->bExists)
	{
		nHashPos = (nHashPos + 1)%DCHASH_SIZE;
		if (nHashPos == nHashStart)
			return 0;
	}

	((*HashMg)+nHashPos)->bExists = 1;
	((*HashMg)+nHashPos)->nHashA = nHashA;
	((*HashMg)+nHashPos)->nHashB = nHashB;

    if(pmkey != NULL && *pmkey != 0)
	{
		ln = strlen(pmkey);
		((*HashMg)+nHashPos)->pentityname = (char *)malloc(ln+1);
		strncpy(((*HashMg)+nHashPos)->pentityname, pmkey, ln);
		*(((*HashMg)+nHashPos)->pentityname + ln) = 0;	 
	}
	ires = 0;
	ires = Insertsiattlist(&((*HashMg) + nHashPos)->slist, scene, intid, score);
	

	return ires;
}
