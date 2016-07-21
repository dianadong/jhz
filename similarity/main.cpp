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

	char *STOP = "H:\\GitDemo\\data_file\\newstopword.txt";                      //停用词表
	char *INTENTSET = "H:\\GitDemo\\similarity\\intentset(1,50,1000).txt";         //意图词集合

	char *resultpath = "H:\\GitDemo\\similarity\\result.txt";     //存储相似度计算结果
	const char *reskpath = "H:\\GitDemo\\similarity\\intentset.txt";    //写入意图集合，格式：SC01####1	音乐,n,11，10.0
    
	FILE *FI,*fw, *fr;    
	 
	if (INTENTSET == NULL || *INTENTSET == 0 || reskpath == NULL || *reskpath == 0 || resultpath == NULL || *resultpath == 0)
	{
		printf("ERROR:Can not open the filepath!\n");
		return -1;
	}

	FI = fopen(INTENTSET, "r");    
	//FU = fopen(USERSENTENCE, "r");
	fw = fopen(reskpath, "wb");
	fr = fopen(resultpath, "wb");
	if (FI == NULL || fw == NULL || fr == NULL)
	{
		printf("ERROR:Can not open the file!\n");
		return -1;
	}
        
	char *pbuf, *p, *q, *temp, *s, *qint, *sijhpath, *pykskey, *pfxresk;
	char intid[20], szFind[MAX_PATH], szFile[MAX_PATH];
	int intenval = 1, len, ires = 0, count = 0, nStop = 0, mallocLines, i;
	double similarity = 0.0000001;
	long lkbit;
		
	entlist userL = NULL;
	siattlist intentL = NULL;

	hashtable ewhash = NULL, wsvhash = NULL;
	Init_mghash(&ewhash);
	Init_mghash(&wsvhash);

	//ires = Initlex((char *)singlepath, (char *)wordpath1);
	ires= Initlex3maj((char *)singlepath, (char *)wordpath1, (char *) wordpath2);//加载字典，权重归一化
    if(ires < 0)
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
		printf("ERROR:singer\song\intentionset jiazai shibai!!!\n");
		return -1;
	}
	/*
    lkbit = -1;
	lkbit = GetHashmg(ewhash, "求索");//返回hash表中位置
	if(lkbit >= 0)
	{
		printf(" part === %s \n", (ewhash+lkbit)->pidcode);
	}
	*/
	/////////////////////////////////////////////
	//用户语句处理
	pykskey = "来个电影看看";
	pfxresk = (char *)malloc(1024);
	*pfxresk = 0;
	ires = 0;
	ires = uqueryVecfeat(ewhash, pykskey, pfxresk);     //用户语句进行特征表示
	if (ires < 0)
	{
		printf("ERROR:usersentence qiefen shibai!!!\n");
		return -1;
	}

	createuserlist(pfxresk, &userL);                     //用户输入pfxresk转成list

	pbuf = (char *)malloc(1 << 14);
	*pbuf = 0;

	fprintf(fr, "user sentence：%s\n", pykskey);
	fprintf(fr, "Vector represention: %s\n", pfxresk);
	fprintf(fr, "####################################\n");

	
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

	while(TRUE)
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
			ires = intent_aggVectorW(ewhash,sijhpath,stop,nStop,fw);       //各意图语句切分后，连带词性、词频统计放入hash中,并将hash转换成list
		}
		if (!FindNextFile(hFind, &FindFileData))
			break;
	}
	fclose(fw);
	
	for (i = 0; i < nStop; i++)     //释放停用词表
		free(stop[i]);	
	free(stop);

	/////////////////////////////////////////////
	//计算每个意图和用户输入语句的相似度
	while(fgets(pbuf, 1<<14, FI) &&strlen(pbuf)>1)   //过滤掉各种换行符等其他符号，避免因此使得pbuf不为空而进入循环出错
	{

		//printf("%s\n",pbuf);                         //整个意图下的词实体及其属性
		q = pbuf;
		p = NULL;
	    p = strchr(pbuf, 0x09);                    //查找首次出现\t的位置，找出意图ID
		len = 0;
		len = p - q;
		intid[0] = 0;
		strncpy(intid, q, len);
		intid[len] = 0;                            //意图ID
		qint = intid;
		//printf("####################\n");
		fprintf(fr,"Intention ID : %s,",intid);

		p++;                                       //忽略‘/t’

   		createlist(p,&intentL);
		
		//print(&intentL);
		//printlist(userL);
	
		similarity = VectorSimil(userL,intentL);        //意图描述程度
		fprintf(fr,"The degree of description is %f\n", similarity);
		//ins_intList(&IL, qint, similarity);
		freeAtrList(&intentL);
	}
	fclose(fr);
	freeEntlist(&userL);

	system("pause");
    return 0;
}
