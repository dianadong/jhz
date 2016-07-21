#undef UNICODE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> 

#include "sharfuct.h"
#include "segment.h"
#include "EntityRecog.h"
#include "WordSvector.h"

char **stop;           //ͣ�ôʱ��ָ��
#define MAXLINES 1250      //ͣ�ñ��С

int main()
{
	const char * singlepath = "H:\\GitDemo\\data_file\\singlegbk.txt";    //�ִʵ���أ�����/��Ƶ
	const char * wordpath1 = "H:\\GitDemo\\data_file\\wordgbknew.txt";
	const char * wordpath2 = "H:\\GitDemo\\data_file\\newadd.txt";

	const char * senctpath = "H:\\GitDemo\\data_file\\CSentence.txt";      //���ֳ������ݿ�
	const char * songpath = "H:\\GitDemo\\data_file\\CSnamcode.txt";
	const char * namepath = "H:\\GitDemo\\data_file\\CStancode.txt";

	const char * wparpath = "H:\\GitDemo\\data_file\\wordpart.txt";      //���Ա�ע����

	char intentpath[MAX_PATH] = "H:\\GitDemo\\data_file\\IS";           //�������������
	char *STOP = "H:\\GitDemo\\data_file\\newstopword.txt";               //ͣ�ôʱ�

	const char *reskpath = "H:\\GitDemo\\intwords\\intentset.txt";    //д����ͼ���ϣ���ʽ��SC01####1	����,n,11��10.0

	FILE *fw, *fr;

	if (reskpath == NULL || *reskpath == 0 )
	{
		printf("ERROR:Can not open the filepath!\n");
		return -1;
	}

	fw = fopen(reskpath, "wb");
	
	if (fw == NULL)
	{
		printf("ERROR:Can not open the file!\n");
		return -1;
	}

	char *sijhpath;
	char szFind[MAX_PATH], szFile[MAX_PATH];
	int ires = 0, nStop = 0, i, mallocLines;
	
	siattlist intentL = NULL;

	hashtable ewhash = NULL, wsvhash = NULL;
	Init_mghash(&ewhash);
	Init_mghash(&wsvhash);

	ires = Initlex3maj((char *)singlepath, (char *)wordpath1, (char *)wordpath2);//�����ֵ䣬Ȩ�ع�һ��
	if (ires < 0)
	{
		printf("ERROR:ci dian jiazai shibai!!!\n");
		return -1;
	}
	ires = wordpart2Table(&ewhash, wparpath);              //�ʡ�������hash
	if (ires < 0)
	{
		printf("ERROR:ci xing jiazai shibai!!!\n");
		return -1;
	}
	ires = Sceneinfo2hash(&ewhash, namepath, songpath, senctpath);      //������������������ͼ����hash	
	if (ires < 0)
	{
		printf("ERROR:singer��song��intentionset jiazai shibai!!!\n");
		return -1;
	}
	
	////////////////////////////////////////
	//������ͼ����
	WIN32_FIND_DATA FindFileData;   //ѭ����ȡ�ļ������ļ�
	strcpy(szFind, intentpath);
	strcat(szFind, "\\*.*");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return -1;

	mallocLines = MAXLINES;          //����ͣ�ôʱ�����ͣ�ôʸ���
	stop = (char**)malloc(sizeof(char*)*mallocLines);
	nStop = readStopLib(STOP, stop);

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

			ires = 0;
			ires = intent_aggVectorW(ewhash, sijhpath, stop, nStop, fw);       //����ͼ����зֺ��������ԡ���Ƶͳ�Ʒ���hash��,����hashת����list
		}
		if (!FindNextFile(hFind, &FindFileData))
			break;
	}
	fclose(fw);

	for (i = 0; i < nStop; i++)     //�ͷ�ͣ�ôʱ�
		free(stop[i]);
	free(stop);

	system("pause");
	return 0;
}
