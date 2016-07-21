
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
		((*HashMg)+i)->fqval = 0;
        ((*HashMg)+i)->score = 0.0001;
		((*HashMg)+i)->pidcode = NULL;
	}
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
		    ((*HashMg)+i)->fqval = 0;
			((*HashMg)+i)->score = 0.0001;
			if(((*HashMg)+i)->pidcode != NULL)
			{
				free(((*HashMg)+i)->pidcode);
				((*HashMg)+i)->pidcode = NULL;
			}
		}
	}
	free(*HashMg);
	*HashMg = NULL;
}

///////////////////////////////////////////////////////////
//function:
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
			if((hashmg+i)->pentityname != NULL)
				fprintf(fw, "%s === %s\n", (hashmg+i)->pentityname, (hashmg+i)->pidcode);
		}
	}
	fclose(fw);
}

///////////////////////////////////////////////////////////
//function:
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
void Accsce_hash(hashtable hashmg, FILE * fw)
{
	long i;
	if(hashmg == NULL)
		return ;

    for(i = 0; i<DCHASH_SIZE; i++)
	{
		if(	(hashmg+i)->bExists != 0)
		{
			fprintf(fw, "%s\t%s\t%d\n", (hashmg+i)->pentityname, (hashmg+i)->pidcode, (hashmg+i)->fqval);
		}
	}
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
//function: 插入新的元素进入hash表
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
int SetHashmg(hashtable * HashMg, const char *pmkey, const char * pidkey)
{
	int ln;
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
	((*HashMg)+nHashPos)->fqval = 0;
    ((*HashMg)+nHashPos)->score = 0.0001;
	if(pidkey != NULL && *pidkey != 0)
	{
		ln = strlen(pidkey);
		((*HashMg)+nHashPos)->pidcode = (char *)malloc(ln+1);
		strncpy(((*HashMg)+nHashPos)->pidcode, pidkey, ln);
		*(((*HashMg)+nHashPos)->pidcode + ln) = 0;	 
	}
	return 1;
}
///////////////////////////////////////////////////////////
//function: 插入新的元素进入hash表
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
int Ins3Hashmg(hashtable * HashMg, const char *pmkey, const char * pidkey, int fqv)
{
	int ln;
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
	((*HashMg)+nHashPos)->fqval = fqv;
    ((*HashMg)+nHashPos)->score = 0.0001;
	if(pidkey != NULL && *pidkey != 0)
	{
		ln = strlen(pidkey);
		((*HashMg)+nHashPos)->pidcode = (char *)malloc(ln+1);
		strncpy(((*HashMg)+nHashPos)->pidcode, pidkey, ln);
		*(((*HashMg)+nHashPos)->pidcode + ln) = 0;	 
	}
	return 1;
}
////////////////////////////////////////////////////////////////
///function: 实体名 ：编码   入 Hash
//parameter: 
//author: wuxiaoqi
//time: 2016-3-23
//////////////////////////////////////////////////////////////
int EntityCode2Table(hashtable * HashMg, const char * sourcepath)
{
	FILE * fp;
	char * pbuf, *p, *q, *pcode, *pkey;
	int ln, rbit, znum = 0;
	long lkbit;

	
	if(sourcepath == NULL || *sourcepath == 0)
		return znum;
    fp = NULL;
	fp = fopen(sourcepath, "rb");
	if(fp == NULL)
		return  znum;

	pbuf =(char *)malloc(1<<20);
	*pbuf = 0;
	pkey = (char *)malloc(1024);
	pcode = (char *)malloc(1024);
	//*pcode = 0;
	//*pkey = 0;
	while(fgets(pbuf, 1<<20, fp))
	{
		ln = 0;
		ln = GetRealString(pbuf);

		if(ln <= 0)
			continue;

		p = NULL;
		p = strchr(pbuf, 0x09);
		if(p == NULL)
			continue;

		ln = p - pbuf;
		*pkey = 0;
		strncpy(pkey, pbuf, ln);
		*(pkey+ln) = 0;
		++p;

		q = NULL;
		q = strchr(p, 0x09);
		if(q == NULL)
			continue;

		ln = 0;
		ln = q - p;
		*pcode = 0;
        strncpy(pcode, p, ln);
		*(pcode + ln) = 0;
			
        ++q;

		lkbit = -1;
		lkbit = GetHashmg(* HashMg, pkey);
		if(lkbit < 0)
		{
			rbit = 0;
			rbit = SetHashmg(HashMg, pkey, pcode);
			++znum;
		}
        *pbuf = 0;
	}
	free(pcode);
	pcode = NULL;
	free(pkey);
	pkey = NULL;
    free(pbuf);
	pbuf = NULL;
    fclose(fp);
	return znum;
}
////////////////////////////////////////////////////////////////
///function:     歌星：编码    装入Hash
///              歌名：编码    装入Hash
///              场景句子：编码    装入Hash
//parameter: 
//author: wuxiaoqi
//time: 2016-3-23
//////////////////////////////////////////////////////////////
int Sceneinfo2hash(hashtable * HashMg, const char * Snamepath, const char * Sstarpath, const char * Ssentpath)
{
	int gnum = 0;

    if(Snamepath != NULL && *Snamepath != 0)
	    gnum += EntityCode2Table(HashMg, Snamepath);

	if(Sstarpath != NULL && *Sstarpath != 0)
	    gnum += EntityCode2Table(HashMg, Sstarpath);
    
	if(Ssentpath != NULL && *Ssentpath != 0)
	    gnum += EntityCode2Table(HashMg, Ssentpath);
    
	return gnum;
}
////////////////////////////////////////////////////////////
///function:     词：词性    装入Hash
//parameter: 
//author: wuxiaoqi
//time: 2016-3-23
/////////////////////////////////////////////////////////////
int wordpart2Table(hashtable * HashMg, const char * sourcepath)
{
	FILE * fp;
	char * pbuf, *p, *q, *pcode, *pkey;
	int ln, rbit, znum = 0;
	long lkbit;

	
	if(sourcepath == NULL || *sourcepath == 0)
		return znum;
    fp = NULL;
	fp = fopen(sourcepath, "rb");
	if(fp == NULL)
		return  znum;

	pbuf =(char *)malloc(1<<20);
	*pbuf = 0;
	pkey = (char *)malloc(1024);
	pcode = (char *)malloc(1024);

	while(fgets(pbuf, 1<<20, fp))
	{
		ln = 0;
		ln = GetRealString(pbuf);
		if(ln <= 0)
			continue;
		p = NULL;
		p = strchr(pbuf, 0x09);
		if(p == NULL)
			continue;
		ln = 0;
		ln = p - pbuf;
		*pkey = 0;
		strncpy(pkey, pbuf, ln);
		*(pkey+ln) = 0;
		++p;
		////////////////////////////
        q = NULL;
		q = strchr(p, 0x2F);
		if(q == NULL)
			continue;
        ln = 0;
		ln = q - p;
		*pcode = 0;
		strncpy(pcode, p, ln);
		*(pcode+ln) = 0;
        ////////////////////////////////////
		lkbit = -1;
		lkbit = GetHashmg(* HashMg, pkey);
		if(lkbit >= 0)
			;
		else
		{
			rbit = 0;
			rbit = SetHashmg(HashMg, pkey, pcode);
			++znum;
		}
        *pbuf = 0;
	}
	free(pcode);
	pcode = NULL;
	free(pkey);
	pkey = NULL;
    free(pbuf);
	pbuf = NULL;
    fclose(fp);
	return znum;
}

////////////////////////////////////////////////////////////////
///function: <歌星####歌曲名> ：编码   入  2维Hash
//parameter: 
//author: wuxiaoqi
//time: 2016-3-23
//////////////////////////////////////////////////////////////
int SC_Ey2_Epail2hash(hashtable * HashMg, const char * Snamepath, const char * Sstarpath, const char * Ssentpath, const char * Epairpath)
{
	int gnum = 0;

    if(Snamepath != NULL && *Snamepath != 0)
	    gnum += EntityCode2Table(HashMg, Snamepath);

	if(Sstarpath != NULL && *Sstarpath != 0)
	    gnum += EntityCode2Table(HashMg, Sstarpath);

	if(Ssentpath != NULL && *Ssentpath != 0)
	    gnum += EntityCode2Table(HashMg, Ssentpath);

    if(Epairpath != NULL && *Epairpath != 0)
	    gnum += EntityCode2Table(HashMg, Epairpath);
	return gnum;
}
