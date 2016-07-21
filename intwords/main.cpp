#undef UNICODE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> 

#include "sharfuct.h"
#include "segment.h"
#include "EntityRecog.h"
#include "WordSvector.h"

char **stop;           //停用词表的指针
#define MAXLINES 1250      //停用表大小

int main()
{
	const char * singlepath = "H:\\GitDemo\\data_file\\singlegbk.txt";    //字词典加载：词组/词频
	const char * wordpath1 = "H:\\GitDemo\\data_file\\wordgbknew.txt";
	const char * wordpath2 = "H:\\GitDemo\\data_file\\newadd.txt";

	const char * senctpath = "H:\\GitDemo\\data_file\\CSentence.txt";      //音乐场景数据库
	const char * songpath = "H:\\GitDemo\\data_file\\CSnamcode.txt";
	const char * namepath = "H:\\GitDemo\\data_file\\CStancode.txt";

	const char * wparpath = "H:\\GitDemo\\data_file\\wordpart.txt";      //词性标注集合

	char intentpath[MAX_PATH] = "H:\\GitDemo\\data_file\\IS";           //导入各场景语料
	char *STOP = "H:\\GitDemo\\data_file\\newstopword.txt";               //停用词表

	const char *reskpath = "H:\\GitDemo\\intwords\\intentset.txt";    //写入意图集合，格式：SC01####1	音乐,n,11，10.0

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

	ires = Initlex3maj((char *)singlepath, (char *)wordpath1, (char *)wordpath2);//加载字典，权重归一化
	if (ires < 0)
	{
		printf("ERROR:ci dian jiazai shibai!!!\n");
		return -1;
	}
	ires = wordpart2Table(&ewhash, wparpath);              //词、词性入hash
	if (ires < 0)
	{
		printf("ERROR:ci xing jiazai shibai!!!\n");
		return -1;
	}
	ires = Sceneinfo2hash(&ewhash, namepath, songpath, senctpath);      //歌手名、歌曲名、意图语句进hash	
	if (ires < 0)
	{
		printf("ERROR:singer、song、intentionset jiazai shibai!!!\n");
		return -1;
	}
	
	////////////////////////////////////////
	//生成意图集合
	WIN32_FIND_DATA FindFileData;   //循环读取文件夹下文件
	strcpy(szFind, intentpath);
	strcat(szFind, "\\*.*");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return -1;

	mallocLines = MAXLINES;          //建立停用词表，返回停用词个数
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
			ires = intent_aggVectorW(ewhash, sijhpath, stop, nStop, fw);       //各意图语句切分后，连带词性、词频统计放入hash中,并将hash转换成list
		}
		if (!FindNextFile(hFind, &FindFileData))
			break;
	}
	fclose(fw);

	for (i = 0; i < nStop; i++)     //释放停用词表
		free(stop[i]);
	free(stop);

	system("pause");
	return 0;
}
