#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <memory.h>
#include <math.h>
#include "segment.h"
//#include "stdbool.h"

int	SingleFreq[65536];
uchar WordLenMap[65536][MAX_WORD_LEN];
struct word {char name[256]; int weight;};
struct word * wordlist;

////////////////////////////////////////////////////////
///function:
//parameter: 
//author: wuxiaoqi
//time: 2013-4-8
////////////////////////////////////////////////////////
unsigned int dhash(char *key, register int len, int i)
{
	register unsigned int n = 0;
	register int ii;
	register char *pkey = (char *)key;
	for (ii=0; ii<len; ii++)
		n = (n<<5) + n + *pkey++;
	return ((n & (HASH_SIZE - 1)) + i * (n | 0xffffffff)) & (HASH_SIZE - 1);
}
////////////////////////////////////////////////////////
///function:
//parameter: 
//author: wuxiaoqi
//time: 2013-4-8
////////////////////////////////////////////////////////
int getfq(uchar *p, int ilen)
{
	if (ilen < 2)
		return 0;
	else if (ilen == 2)
		return SingleFreq[*(ushort *)p];
	else
	{
		char buffer[MAX_WORD_LEN + 1] = "";
		int value = 0;

		memcpy(buffer, p, ilen);
		
		int k = 0;
		struct word *tmp;
		do
		{
			tmp = &wordlist[dhash(buffer, ilen, k)];
			k++;
		}
		while(tmp->name[0] && (*tmp->name != *buffer || strcmp(tmp->name + 1, buffer + 1)));

		if (tmp->name[0])
			value = tmp->weight;
		return value;
	}
}

////////////////////////////////////////////////////////
///function://·Ö¸î
//parameter: 
//author: wuxiaoqi
//time: 2013-4-8
////////////////////////////////////////////////////////
void getsegmenter(char * const src, int *len, int ilen)
{
	char currentchar;
	int i;
	//uchar map[ilen + 1];
	int * tcount = (int *)malloc((ilen + 1)*sizeof(int));
	int * power = (int *)malloc((ilen + 1)*sizeof(int));
	int * scount = (int *)malloc((ilen + 1)*sizeof(int));
	uchar * map = (uchar *)malloc((ilen + 1)*sizeof(uchar));

	map[ilen] = 1;
	for (i =0; i<ilen; i++)
	{
		if (src[i] >> 7)
		{
			map[i] = 1;
			i++;
			map[i] = 0;
		}
		else
			map[i] = 1;
	}

	if (!map[ilen])
	{
		ilen--;
		len[ilen] = 1;
	}

	int ii = 0;
	int pos = ilen - 1;
	if (!map[pos])
		pos --;

	tcount[ilen] = 0;
	power[ilen] = 0;
	scount[ilen] = 0;
	while(pos >= 0)
	{
		int kk;

		if (map[pos + 1])
			kk = 1;
		else
			kk = 2;

		power[pos] = power[pos + kk];
		scount[pos] = scount[pos + kk];
		tcount[pos] = tcount[pos + kk] + 1;
		len[pos] = kk;
		currentchar = src[pos];
		   
		{
			power[pos] = power[pos + kk];
			scount[pos] = scount[pos + kk] + (!map[pos + 1] ? SingleFreq[*(ushort *)(src + pos)] : 0);

			int m = 0;
			int n = 0;
			while (n = WordLenMap[*(ushort *)(src + pos)][m])
			{
				if (pos + n <= ilen)
				{
					int k = 0;
					struct word *tmp;
					char *newword = (char *)(src + pos);
					do
					{
						tmp = &wordlist[dhash(newword, n, k)];
						k++;
					}
					while(tmp->name[0] && (*tmp->name != *newword || 
						strncmp(tmp->name + 1, newword + 1, n - 1) || tmp->name[n] != 0));
	
					if (tmp->name[0])
					{
						int pw = power[pos+n]+ tmp->weight;
						if(tcount[pos+n]+1< tcount[pos]||(tcount[pos+n]+1 == 
							tcount[pos] && (scount[pos]+ power[pos]<scount[pos+n]+pw)))
						{
							tcount[pos]= tcount[pos+n]+1;
							len[pos] = n;
							power[pos]= pw;
							scount[pos] = scount[pos+n];
						}		   
					}	
				}

				m++;
			}
		}		   

		if (map[pos - 1])
			pos--;
		else
			pos -= 2;
	}  

        for (ii=0; ii<ilen; ii += len[ii])
        {
                int kk = ii;

		if (len[kk] == 1 && (src[kk] == '%' || isdigit(src[kk])))
			while (len[ii] < MAX_SEG_LEN && kk+1 < ilen && len[kk + 1] == 1 && (src[kk + 1] == '%' || isdigit(src[kk + 1])))
				kk++,len[ii]++;
		else if (len[kk] == 1 && isalpha(src[kk]))
			while (len[ii] < MAX_SEG_LEN && kk+1 < ilen && len[kk + 1] == 1 && isalpha(src[kk + 1]))
				kk++,len[ii]++;
        }
	if(tcount != NULL)
	{
		free(tcount);
		tcount= NULL;
	}
	if(power != NULL)
	{
		free(power);
		power= NULL;
	}
	if(scount != NULL)
	{
		free(scount);
		scount= NULL;
	}
	if(map != NULL)
	{
		free(map);
		map= NULL;
	}
}
////////////////////////////////////////////////////////
///function:ÇÐ·Ö
//parameter: 
//author: wuxiaoqi
//time: 2013-4-8
////////////////////////////////////////////////////////
int repqiefen(char * srcbuf, int lensrc, char * resbuf, int lenres)
{
	 int ii; 
	 int size = 0;	 
	 if(srcbuf == NULL )
	 {
          printf("srcbuf == NUL\n");
	 	  return -1;
	 }
	 if(resbuf == NULL )
	 {
		printf("resbuf == NUL\n");
		return -1;
	 }
	if( *srcbuf == 0)	
		return  0;
 	  
	 int *len = (int *)malloc((lensrc + 1) * sizeof(int));
	 memset(len, 0, (lensrc + 1) * sizeof(int));
	 getsegmenter(srcbuf, len, lensrc);
	 for(ii=0; ii<lensrc; ii += len[ii])
	 { 
		  if(size + len[ii] + 1 < lenres)
				size += sprintf(resbuf + size, "%.*s ", len[ii] , srcbuf + ii);
		  else
		  {
            	//printf("size = %d;lensrc = %d; len = %d; lenres = %d\n", size, lensrc, len[ii], lenres);
				free(len);
				return -1;	
		  }
	 }
	 free(len);
	 return size;
}
////////////////////////////////////////////////////////
///function:
//parameter: 
//author: wuxiaoqi
//time: 2013-4-8
////////////////////////////////////////////////////////
int Initlex(char * singpath, char * wordpath)
{
	int res = -1;
	wordlist = (struct word *)malloc(HASH_SIZE * sizeof(struct word));
	if(wordlist == NULL)
	{
  		 printf("ERROR mem in 550!!!\n");
			 return res;
	}
	int i;
	for(i=0; i< HASH_SIZE; i++)
	{
		wordlist[i].name[0] = 0;
		wordlist[i].weight = 0;
	}

	memset(SingleFreq, 0, sizeof(SingleFreq));
	memset(WordLenMap, 0, sizeof(WordLenMap));
	//printf("11111====== \n");
	res = loaddic(singpath, wordpath);
	return res;
}
////////////////////////////////////////////////////////
///function:
//parameter: 
//author: wuxiaoqi
//time: 2013-4-8
////////////////////////////////////////////////////////
void freewdlist()
{
	free(wordlist);
	wordlist = NULL;
}
////////////////////////////////////////////////////////
///function:
//parameter: 
//author: wuxiaoqi
//time: 2013-4-8
////////////////////////////////////////////////////////
int loaddic(char * singpath, char *  wordpath)
{
        FILE *fp = NULL;
        char buffer[64] = "";
        uint value = 0;
        char *pvalue = NULL;
        int weight = 0,len = 0;
        int ikeylen = 0;
        if(singpath ==NULL ||*singpath == 0)
                return -1;
        if(wordpath ==NULL ||*wordpath == 0)
                return -1;
        
        fp = fopen(singpath, "rb");
        if(fp != NULL)
        {
        	
                while (fgets(buffer, sizeof(buffer) - 1, fp) > 0)
                {
                	
                        len = 0;
                        len = strlen(buffer);
                        if(len <= 0)
                                break;
                        value = atoi(buffer + 3);// Convert string to int
        
                        if(value > 1500)
                                value = (int)sqrt((double)(0.5 * (value - 1500) + 1500 + 150));
                        else
                                value = (int)sqrt((double)(value + 150));
        
                        SingleFreq[*(ushort *)(buffer)] = value;
                        
                }
                fclose(fp);
        }
        else
        {
                 printf("add lexcon sing word lex!!\n");
                 return -1;
        }
        fp = NULL;
        fp = fopen(wordpath, "rb");
        if(fp != NULL)
        {
        	//printf("1111   ======   1111\n");
                size_t inlen = 0;
                size_t outlen = 0;
                while (fgets(buffer, sizeof(buffer) - 1, fp) > 0)
                {
                	    //printf("entity buffer  ======  %s\n", buffer);
                        len = 0;
                        len = strlen(buffer);
                        if(len <= 0)
                                break;
                        
                        value = atoi(buffer + 3);
                        pvalue = (char *)memchr(buffer, '\t', sizeof(buffer));
                        ikeylen = (pvalue - buffer) > MAX_WORD_LEN ? MAX_WORD_LEN : (pvalue - buffer);
                        *pvalue = 0;

                        weight = atoi(pvalue + 1);

                        if (weight > 500) 
                                value = (int)(8 * log((double)((weight - 500) * 0.4 + 500 + 100)));
                        else if (weight < 2) 
                                value = (int)(log((double)(weight + 10)));
                        else
                                value = (int)(8 * log((double)(weight + 100)));

                        bool allowed = true;
                        if ((ikeylen == 8) || (ikeylen == 6 && weight <= 16))
                        {
                                char *pkey = buffer;

                                while (*pkey)
                                {
                                        pkey++;

                                        int frontfq = getfq((uchar *)(buffer), pkey - buffer);
                                        if (!frontfq)   continue;

                                        int backfq = getfq((uchar *)pkey, ikeylen - (pkey - buffer));
                                        if (!backfq)    continue;

                                        if(((int)value < frontfq) && ((int)value < backfq))
                                        {
                                                allowed = false;
                                                        break;
                                        }       
                                }       
                        }

                        if (!allowed)
                        {
                                continue;
                        }
                        memset(pvalue, 0, MAX_WORD_LEN - ikeylen);
                        {
                                int k = 0;
                                struct word *tmp;
                                do
                                {
                                        tmp = &wordlist[dhash(buffer, ikeylen, k)];
                                        k++;
                                }
                                while(tmp->name[0] && strcmp(tmp->name, buffer));
                                strcpy(tmp->name, buffer);
                                tmp->weight = value;
                        }       
                        //max word len
                        if (ikeylen > 2)
                        {
                        	 int i;
                                for (i=0; i<MAX_WORD_LEN; i++)
                                {
                                        if (WordLenMap[*(ushort *)buffer][i] == ikeylen)
                                                break;
                                        else if (!WordLenMap[*(ushort *)buffer][i])
                                        {
                                                WordLenMap[*(ushort *)buffer][i] = ikeylen;
                                                        break;
                                        }
                                }       
                        }
                }
                fclose(fp);
        }
        else
        {
                printf("add lexcon mudiu word lex!!\n");
                return -1;
        }
  return 0;
}

////////////////////////////////////////////////////////
///function:
//parameter: 
//author: wuxiaoqi
//time: 2013-4-8
////////////////////////////////////////////////////////
int loadmajordic(char * singpath, char *  wordpath, char * majopath)
{
	FILE *fp = NULL;
	char buffer[64] = "";
	uint value = 0;
	char *pvalue = NULL;
	int weight = 0, len = 0;
	int ikeylen = 0;
	if(singpath ==NULL ||*singpath == 0)
		return -1;
	if(wordpath ==NULL ||*wordpath == 0)
		return -1;
        
	fp = fopen(singpath, "rb");
	if(fp != NULL)
	{
        	
		while (fgets(buffer, sizeof(buffer) - 1, fp) > 0)
		{
			len = 0;
			len = strlen(buffer);
			if(len <= 0)
				break;
			value = atoi(buffer + 3);
			if(value > 1500)
				value = (int)sqrt((double)(0.5 * (value - 1500) + 1500 + 150));
			else
				value = (int)sqrt((double)(value + 150));

        
			SingleFreq[*(ushort *)(buffer)] = value;
                        
		}
        fclose(fp);
	}
	else
	{
		printf("add lexcon sing word lex!!\n");
		return -1;
	}
	fp = NULL;
	fp = fopen(wordpath, "rb");
	if(fp != NULL)
	{
		size_t inlen = 0;
		size_t outlen = 0;
		while (fgets(buffer, sizeof(buffer) - 1, fp) > 0)
		{
                	    //printf("entity buffer  ======  %s\n", buffer);
			len = 0;
			len = strlen(buffer);
			if(len <= 0)
				break;
                        
			//value = atoi(buffer + 3);
			pvalue = (char *)memchr(buffer, '\t', sizeof(buffer));
			ikeylen = (pvalue - buffer) > MAX_WORD_LEN ? MAX_WORD_LEN : (pvalue - buffer);
			*pvalue = 0;

			weight = atoi(pvalue + 1);

			if (weight > 500) 
				value = (int)(8 * log((double)((weight - 500) * 0.4 + 500 + 100)));
			else if (weight < 2) 
				value = (int)(log((double)(weight + 10)));
			else
				value = (int)(8 * log((double)(weight + 100)));

			bool allowed = true;
			if ((ikeylen == 8) || (ikeylen == 6 && weight <= 16))
			{
				char *pkey = buffer;

				while (*pkey)
				{
					pkey++;

					int frontfq = getfq((uchar *)(buffer), pkey - buffer);
					if (!frontfq)
						continue;

					int backfq = getfq((uchar *)pkey, ikeylen - (pkey - buffer));
					if (!backfq)
						continue;
					if(((int)value < frontfq) && ((int)value < backfq))
					{
						allowed = false;
						break;
					}       
				}       
			}

			if (!allowed)
			{
				continue;
			}
			memset(pvalue, 0, MAX_WORD_LEN - ikeylen);
			{
				int k = 0;
				struct word *tmp;
				do
				{
					tmp = &wordlist[dhash(buffer, ikeylen, k)];
					k++;
				}
				while(tmp->name[0] && strcmp(tmp->name, buffer));
				strcpy(tmp->name, buffer);
				tmp->weight = value;
			}       
			//max word len
            if (ikeylen > 2)
            {
                 int i;
                    for (i=0; i<MAX_WORD_LEN; i++)
                    {
                            if (WordLenMap[*(ushort *)buffer][i] == ikeylen)
                                    break;
                            else if (!WordLenMap[*(ushort *)buffer][i])
                            {
                                    WordLenMap[*(ushort *)buffer][i] = ikeylen;
                                            break;
                            }
                    }       
            }
		}
		fclose(fp);
	}
	else
	{
		printf("add lexcon mudiu word lex!!\n");
		return -1;
	}

	if(majopath == NULL ||*majopath == 0)
		return 0;

    fp = NULL;
	fp = fopen(majopath, "rb");
	if(fp != NULL)
	{
		size_t inlen = 0;
		size_t outlen = 0;
		while (fgets(buffer, sizeof(buffer) - 1, fp) > 0)
		{
                	    //printf("entity buffer  ======  %s\n", buffer);
			len = 0;
			len = strlen(buffer);
			if(len <= 0)
				break;
                        
			//value = atoi(buffer + 3);
			pvalue = (char *)memchr(buffer, '\t', sizeof(buffer));
			ikeylen = (pvalue - buffer) > MAX_WORD_LEN ? MAX_WORD_LEN : (pvalue - buffer);
			*pvalue = 0;

			weight = atoi(pvalue + 1);

			if (weight > 500) 
				value = (int)(8 * log((double)((weight - 500) * 0.4 + 500 + 100)));
			else if (weight < 2) 
				value = (int)(log((double)(weight + 10)));
			else
				value = (int)(8 * log((double)(weight + 100)));

			bool allowed = true;
			if ((ikeylen == 8) || (ikeylen == 6 && weight <= 16))
			{
				char *pkey = buffer;

				while (*pkey)
				{
					pkey++;

					int frontfq = getfq((uchar *)(buffer), pkey - buffer);
					if (!frontfq)
						continue;

					int backfq = getfq((uchar *)pkey, ikeylen - (pkey - buffer));
					if (!backfq)
						continue;
					if(((int)value < frontfq) && ((int)value < backfq))
					{
						allowed = false;
						break;
					}       
				}       
			}

			if (!allowed)
			{
				continue;
			}
			memset(pvalue, 0, MAX_WORD_LEN - ikeylen);
			{
				int k = 0;
				struct word *tmp;
				do
				{
					tmp = &wordlist[dhash(buffer, ikeylen, k)];
					k++;
				}
				while(tmp->name[0] && strcmp(tmp->name, buffer));
				strcpy(tmp->name, buffer);
				tmp->weight = value;
			}       
			//max word len
            if (ikeylen > 2)
            {
                 int i;
                    for (i=0; i<MAX_WORD_LEN; i++)
                    {
                            if (WordLenMap[*(ushort *)buffer][i] == ikeylen)
                                    break;
                            else if (!WordLenMap[*(ushort *)buffer][i])
                            {
								WordLenMap[*(ushort *)buffer][i] = ikeylen;
								break;
                            }
                    }       
            }
		}
		fclose(fp);
	}
	else
	{
		printf("add lexcon mudiu word lex!!\n");
		return 0;
	}
	return 0;
}
////////////////////////////////////////////////////////
///function:
//parameter: 
//author: wuxiaoqi
//time: 2013-4-8
////////////////////////////////////////////////////////
int Initlex3maj(char * singpath, char * wordpath, char * majopath)
{
	int res = -1;
	wordlist = (struct word *)malloc(HASH_SIZE * sizeof(struct word));
	if(wordlist == NULL)
	{
  		 printf("ERROR mem in 550!!!\n");
			 return res;
	}
	int i;
	for(i=0; i< HASH_SIZE; i++)
	{
		wordlist[i].name[0] = 0;
		wordlist[i].weight = 0;
	}

	memset(SingleFreq, 0, sizeof(SingleFreq));
	memset(WordLenMap, 0, sizeof(WordLenMap));
	//printf("11111====== \n");
	res = loadmajordic(singpath, wordpath, majopath);
	return res;
}
