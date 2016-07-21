
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"sharfuct.h"

unsigned long cryptTable[0x500];
/////////////////////////////////////
//function:去除空白字符
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////
int GetRealString(char *pbuf)
{
		int lLen = strlen(pbuf)-1;
		while(lLen>0 && (pbuf[lLen] == (char)0x0d || 
			pbuf[lLen] == (char)0x0a || pbuf[lLen] == ' '
			|| pbuf[lLen] == '\t' )) lLen--;
		if(lLen<0) 
		{
			*pbuf = 0;
			return lLen;
		}
		pbuf[lLen + 1] = 0;
		return lLen+1;
}

/////////////////////////////////////
//function:去除特殊字符串
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////
int removalspecstr(const char *psource, char * ptarget)
{
	int kb = 0, ln;
    char * p, *q, *s, *k;
	

	if(psource == NULL || *psource == 0)
		return kb;
	k = ptarget;
	p = (char *) psource;
	while(* p != 0)
	{
		q = NULL;
		q = strchr(p, '&');
		if(q != NULL)
		{
			s = NULL;
			s = strchr(p, ';');
			if(s!= NULL)
			{
				ln = 0;
			    ln = q - p;
                strncpy(k, p, ln);
				*(k+ln) = 0;
				kb += ln;
				k = k + ln;
				p = s + 1;
			}
			else
			{
				ln = strlen(p);
				strncpy(k, p, ln);
				*(k+ln) = 0;
				kb += ln;
				break;
			}
		}
		else
		{
		    ln = strlen(p);
			strncpy(k, p, ln);
			*(k+ln) = 0;
			break;
		}
	}
	return kb;
}

/////////////////////////////////////
//function:去除()里字符串
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////
int removbrackstr(const char *psource, char * ptarget)
{
	int kb = 0, ln;
    char * p, *q, *s, *k;
	
	if(psource == NULL || *psource == 0)
		return kb;
	k = ptarget;
	p = (char *) psource;
	while(* p != 0)
	{
		q = NULL;
		q = strchr(p, '(');
		if(q != NULL)
		{
			s = NULL;
			s = strchr(p, ')');
			if(s!= NULL)
			{
				ln = 0;
			    ln = q - p;
                strncpy(k, p, ln);
				*(k+ln) = 0;
				kb += ln;
				k = k + ln;
				p = s + 1;
			}
			else
			{
				ln = strlen(p);
				strncpy(k, p, ln);
				*(k+ln) = 0;
				kb += ln;
				break;
			}
		}
		else
		{
		    ln = strlen(p);
			strncpy(k, p, ln);
			*(k+ln) = 0;
			break;
		}
	}
	return kb;
}
//////////////////////////////////////////////////////////////////////
///function:置换位置解决冲突
//parameter: 
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////////////////
void prepareCryptTable()
{
	unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;
	for( index1 = 0; index1 < 0x100; index1++ )
	{
		for( index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
		{
			unsigned long temp1, temp2;
			seed = (seed * 125 + 3) % (0x2AAAAB);
			temp1 = (seed & 0xFFFF)<<(0x10);
			seed = (seed * 125 + 3) %(0x2AAAAB);
			temp2 = (seed & 0xFFFF);
			cryptTable[index2] = ( temp1 | temp2 );
		}	
	}
}

////////////////////////////////////////////////////////////////
///function:哈希函数
//parameter: 
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////////
unsigned long HashString(const char *lptermName, unsigned long dwHashType)
{
	unsigned char *key  = (unsigned char *)lptermName;
	unsigned long seed1 = 0x7FED7FED;
	unsigned long seed2 = 0xEEEEEEEE;
	int ch;

	while( *key != 0 )
	{
		ch = *key++;
		seed1 = cryptTable[(dwHashType<<8) + ch] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2<<5) + 3;
	}
	return seed1;
}

#define __malloc(x) malloc(x)
#define __free(x) free(x)
#define __assert(x)

#ifdef DEBUG
static void __kmp_test(unsigned char *W, unsigned int wlen, unsigned int *T)
{
	unsigned int i=0;
	printf("i:\tW[i]\tT[i]\n");
	while(i<wlen)
	{
		printf("%d:\t%c\t%d\n", i, W[i], T[i]);
		i++;
	}
}
#endif
static void __kmp_table(unsigned char *W, unsigned int wlen, unsigned int *T)
{
	unsigned int pos=2, cnd=0;
	T[0]=-1;
	T[1]=0;
	while(pos < wlen)
	{
		if (W[pos-1] == W[cnd])
		{
			cnd = cnd+1;
			T[pos] = cnd;
			pos = pos+1;
		}
		else if (cnd > 0)
		{
			cnd = T[cnd];
		}
		else
		{
			T[pos]=0;
			pos=pos+1;
		}

	}
}
unsigned int kmp_search(unsigned char *S, unsigned int slen, unsigned char *W, unsigned int wlen)
{
	unsigned int m=0, i=0;
	unsigned int *T;

	__assert(S && W);
	T = (unsigned int*)__malloc(wlen * sizeof(unsigned int));
	__assert(T);
	__kmp_table(W, wlen, T);

	#ifdef DEBUG
	__kmp_test(W, wlen, T);
	#endif

	while (m+i < slen)
	{
		if (W[i] == S[m+i])
		{
			if (i == wlen-1) 
			{
				__free(T);
				return m;
			}
			i = i+1;
		} 
		else
		{
			m = m+i-T[i];
			if (T[i] > -1)
				i = T[i];
			else
				i = 0;
		}
	}
	__free(T);
	return slen;
}

///////////////////////////////////////////////////////////
//function: 实体插入结果链表
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
int ins_entList(entlist *EL, const char * pentity, const char * pidcode, int startsit)
{
	entlist pn, pk, pr;
	int kbit, ln;

	if(pentity == NULL ||*pentity == 0)
		return 0;
	pr = NULL;
	pk = *EL;
	while(pk != NULL)
	{
		kbit = -1;
		kbit = strcmp(pentity, pk->pentityk);
		if(kbit <= 0)
			break;

		pr = pk;
		pk = pk->pnext;
	}
	if(pk != NULL && kbit == 0 )
	{
		//pk->csnum += 1;
		return 0;
	}
	else
	{
		pn = (ENTNODE *)malloc(sizeof(ENTNODE));
		if(pn != NULL)
        {
			ln = 0;
			ln = strlen(pentity);

			pn->pentityk = (char *)malloc(ln+1);
			strncpy(pn->pentityk, pentity, ln);
			*(pn->pentityk +ln) = 0;

			ln = 0;
			ln = strlen(pidcode);
			if(ln >= 0)
			{
				pn->pidcode = (char *)malloc(ln+1);
				strncpy(pn->pidcode, pidcode, ln);
				*(pn->pidcode +ln) = 0;
            }
			pn->startsit = startsit;
			pn->pnext = NULL;

			if(pr == NULL)
			{
				pn->pnext = *EL;
				*EL = pn;
                return 1;
			}
			else
			{
				pn->pnext = pk;
				pr->pnext = pn;
				return 1;
			}
		}
		else
			return 0;
	}
	return 0;
}

///////////////////////////////////////////////////////////
//function: 实体链表写入文件
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
void writentlist(entlist elist, FILE * fp)
{
	entlist pk;

	if(elist == NULL)
		return ;
	pk = elist;
	while(pk != NULL)
	{
		fprintf(fp, "%s\t%s\t%d\n", pk->pidcode, pk->pentityk, pk->startsit);
		pk = pk->pnext;
	}
}
///////////////////////////////////////////////////////////
//function: 实体链表写入文件
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
void printlist(entlist elist)
{
	entlist pk;

	if(elist == NULL)
		return ;
	pk = elist;
	while(pk != NULL)
	{
		printf("%s\t%s\t%d\n", pk->pidcode, pk->pentityk, pk->startsit);
		pk = pk->pnext;
	}
}
///////////////////////////////////////////////////////////
//function: 实体链表释放
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
void freeEntlist(entlist *EL)
{
	entlist pk, pa;
	if(*EL == NULL)
		return ;
    pa = NULL;
	pk = *EL;
	while(pk != NULL)
	{
		pa = pk;
		pk = pk->pnext;

		if(pa->pentityk != NULL)
		{
			free(pa->pentityk);
			pa->pentityk = NULL;
		}
		if(pa->pidcode != NULL)
		{
			free(pa->pidcode);
			pa->pidcode = NULL;
		}
		pa->startsit = 0;

		free(pa);
		pa = NULL;
	}
	*EL =NULL;
}
///////////////////////////////////////////////////////////
//function: 实体链表拷贝
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
void CopyElist(entlist *BL, entlist xlist)
{
	entlist pl;

	if(xlist == NULL)
		return ;

	pl = xlist;
	while(pl != NULL)
	{
		ins_entList(BL, pl->pentityk, pl->pidcode, pl->startsit);
		pl = pl->pnext;
	}
}
///////////////////////////////////////////////////////////
//function: 
//parameter:
//author: wuxiaoqi
//time: 2016-3-23
///////////////////////////////////////////////////////////
int __simimin(int a, int b, int c)
{
	if(a < b)
	{  
		if(a < c)  
			return a;
		else   
			return c;
	}
	else
	{
		if(b < c)  
			return b;
		else   
			return c;
	}
}
int __simicmp_distancA(char *strA, int pABegin, int pAEnd, char *strB, int pBBegin, int pBEnd, int **temp)
{  
	int a, b, c;
    if(pABegin > pAEnd)
	{
		if(pBBegin > pBEnd)
		{
			return 0;
		}
		else
		{
            return pBEnd - pBBegin + 1;
        }  
    }  
  
    if(pBBegin > pBEnd)
	{
        if(pABegin > pAEnd)
		{
            return 0;
        }
		else
		{
            return pAEnd - pABegin + 1;
        }
    }

    if(strA[pABegin] == strB[pBBegin])
	{
        if(temp[pABegin + 1][pBBegin + 1] != 0)
		{
			a = temp[pABegin + 1][pBBegin + 1];
        }
		else
		{
            a = __simicmp_distancA(strA, pABegin + 1, pAEnd, strB, pBBegin + 1, pBEnd, temp);
        }
        return a;  
    }
	else
	{
        if(temp[pABegin + 1][pBBegin + 1] != 0)
		{
            a = temp[pABegin + 1][pBBegin + 1];
        }
		else
		{
            a = __simicmp_distancA(strA, pABegin + 1, pAEnd, strB, pBBegin + 1, pBEnd, temp);
            temp[pABegin + 1][pBBegin + 1] = a;
        }
  
        if(temp[pABegin + 1][pBBegin] != 0)
		{
            b = temp[pABegin + 1][pBBegin];
        }
		else
		{
            b = __simicmp_distancA(strA, pABegin + 1, pAEnd, strB, pBBegin, pBEnd, temp);
            temp[pABegin + 1][pBBegin] = b;
        }
  
        if(temp[pABegin][pBBegin + 1] != 0)
		{
            c = temp[pABegin][pBBegin + 1];
        }
		else
		{
            c = __simicmp_distancA(strA, pABegin, pAEnd, strB, pBBegin + 1, pBEnd, temp);
            temp[pABegin][pBBegin + 1] = c;
        }
  
        return __simimin(a, b, c) + 1;
    }
}

int str2similar(const char * pAcomkey, const char *pBcomkey)
{
	int len_a, len_b;
	int **temp;
	int distance = 0, i;
	
	if(pAcomkey == NULL || *pAcomkey == 0)
		return distance;
	if(pBcomkey == NULL || *pBcomkey == 0)
		return distance;
    len_a = strlen(pAcomkey);
    len_b = strlen(pBcomkey);
  
	temp = (int**)malloc(sizeof(int*) * (len_a + 1));
    for(i = 0; i < len_a + 1; i++)
	{
        temp[i] = (int*)malloc(sizeof(int) * (len_b + 1));
        memset(temp[i], 0, sizeof(int) * (len_b + 1));
    }
    memset(temp, 0, sizeof(temp));
    distance = __simicmp_distancA((char *)pAcomkey, 0, len_a - 1, (char *)pBcomkey, 0, len_b - 1, temp);
    //printf("%d\n", distance);
	return distance;
}
