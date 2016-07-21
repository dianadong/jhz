////////////////////////////////////////////
//循环对文件夹下每个文本文件进行切词
///////////////////////////////////////////
#undef UNICODE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <windows.h> 

#include"segment.h"
#include"sharfuct.h"
#include"EntityRecog.h"
#include"WordSvector.h"

int main()
{
	const char * singlepath = "H:\\data_file\\singlegbk.txt";//字典：字/字频
	const char * wordpath1 = "H:\\data_file\\wordgbknew.txt";//词典1：词组/词频
	const char * wordpath2 = "H:\\data_file\\newadd.txt";//词典2：词组/词频
	
	//const char * sijhpath = "H:\\intent_und\\qiefen\\data_file\\dySentence13.txt";//切分对象：意图语句（纯句子）
	char intentpath[MAX_PATH] = "H:\\data_file\\US";
	const char * qiefenpath = "H:\\qiefen\\qiefenres.txt";//切分结果

	FILE *fp,*fw;
	char * pbuf, *pseg, *q, *p, *sijhpath;
	int len, ren,ires = -1;
	char szFind[MAX_PATH], szFile[MAX_PATH];

	WIN32_FIND_DATA FindFileData;
	

	ires= Initlex3maj((char *)singlepath, (char *)wordpath1, (char *) wordpath2);//初始化，字典和词典的权重归一化
    if(ires < 0)
	{
		printf("ci dian jiazai shibai!!!\n");
		return -1;
	}
	
    ires = 0;
    
	strcpy(szFind, intentpath);
	strcat(szFind, "\\*.*");

	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);

	if (INVALID_HANDLE_VALUE == hFind)
		return -1;

	if (qiefenpath == NULL || *qiefenpath == 0)
		return -1;

	fw = NULL;
	fw = fopen(qiefenpath, "w");
	if (fw == NULL)
	{
		printf("ERROR:Can not open qiefenpath!");
		fclose(fw);
		return -1;
	}

	pbuf = (char *)malloc(1 << 10);
	pseg = (char *)malloc(1 << 12);
	*pbuf = 0;
	*pseg = 0;

	while (TRUE)
	{

		if (FindFileData.cFileName[0] != '.')
		{
			strcpy(szFile, intentpath);
			strcat(szFile, "\\");
			strcat(szFile, FindFileData.cFileName);
			//printf("%s\n", szFile);

			sijhpath = szFile;
			if (sijhpath == NULL || *sijhpath == 0)
				return -1; 

			fp = NULL;
			fp = fopen(sijhpath, "r");
			if (fp == NULL)
				return -1;
			
			while (fgets(pbuf, 1 << 10, fp) && fw != NULL)
			{
				len = 0;
				len = GetRealString(pbuf);

				if (len <= 0)
					continue;
				p = NULL;
				p = strstr(pbuf, "####");
				if (p != NULL)
				{
					fprintf(fw, "%s\n", pbuf);
					continue;
				}
				ren = 1024;
				*pseg = 0;
				ires = 0;
				//int repqiefen(char * srcbuf, int lensrc, char * resbuf, int lenres)
				ires = repqiefen(pbuf, len, pseg, ren);//切词
				q = pseg;//切词结果

				while (*q != 0)
				{
					fprintf(fw, "%s\n", q);
					*q = NULL;
				}


			}
			fclose(fp);
		}

		if (!FindNextFile(hFind, &FindFileData))
			break;
	}
 	
	fclose(fw);
	free(pseg);
	pseg = NULL;
    free(pbuf);
	pbuf = NULL;

	system("pause");
	return 0;
}
