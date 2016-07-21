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

	char *STOP = "H:\\GitDemo\\data_file\\newstopword.txt";                      //ͣ�ôʱ�
	char *INTENTSET = "H:\\GitDemo\\similarity\\intentset(1,50,1000).txt";         //��ͼ�ʼ���

	char *resultpath = "H:\\GitDemo\\similarity\\result.txt";     //�洢���ƶȼ�����
	const char *reskpath = "H:\\GitDemo\\similarity\\intentset.txt";    //д����ͼ���ϣ���ʽ��SC01####1	����,n,11��10.0
    
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
	ires= Initlex3maj((char *)singlepath, (char *)wordpath1, (char *) wordpath2);//�����ֵ䣬Ȩ�ع�һ��
    if(ires < 0)
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
		printf("ERROR:singer\song\intentionset jiazai shibai!!!\n");
		return -1;
	}
	/*
    lkbit = -1;
	lkbit = GetHashmg(ewhash, "����");//����hash����λ��
	if(lkbit >= 0)
	{
		printf(" part === %s \n", (ewhash+lkbit)->pidcode);
	}
	*/
	/////////////////////////////////////////////
	//�û���䴦��
	pykskey = "������Ӱ����";
	pfxresk = (char *)malloc(1024);
	*pfxresk = 0;
	ires = 0;
	ires = uqueryVecfeat(ewhash, pykskey, pfxresk);     //�û�������������ʾ
	if (ires < 0)
	{
		printf("ERROR:usersentence qiefen shibai!!!\n");
		return -1;
	}

	createuserlist(pfxresk, &userL);                     //�û�����pfxreskת��list

	pbuf = (char *)malloc(1 << 14);
	*pbuf = 0;

	fprintf(fr, "user sentence��%s\n", pykskey);
	fprintf(fr, "Vector represention: %s\n", pfxresk);
	fprintf(fr, "####################################\n");

	
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
			ires = intent_aggVectorW(ewhash,sijhpath,stop,nStop,fw);       //����ͼ����зֺ��������ԡ���Ƶͳ�Ʒ���hash��,����hashת����list
		}
		if (!FindNextFile(hFind, &FindFileData))
			break;
	}
	fclose(fw);
	
	for (i = 0; i < nStop; i++)     //�ͷ�ͣ�ôʱ�
		free(stop[i]);	
	free(stop);

	/////////////////////////////////////////////
	//����ÿ����ͼ���û������������ƶ�
	while(fgets(pbuf, 1<<14, FI) &&strlen(pbuf)>1)   //���˵����ֻ��з����������ţ��������ʹ��pbuf��Ϊ�ն�����ѭ������
	{

		//printf("%s\n",pbuf);                         //������ͼ�µĴ�ʵ�弰������
		q = pbuf;
		p = NULL;
	    p = strchr(pbuf, 0x09);                    //�����״γ���\t��λ�ã��ҳ���ͼID
		len = 0;
		len = p - q;
		intid[0] = 0;
		strncpy(intid, q, len);
		intid[len] = 0;                            //��ͼID
		qint = intid;
		//printf("####################\n");
		fprintf(fr,"Intention ID : %s,",intid);

		p++;                                       //���ԡ�/t��

   		createlist(p,&intentL);
		
		//print(&intentL);
		//printlist(userL);
	
		similarity = VectorSimil(userL,intentL);        //��ͼ�����̶�
		fprintf(fr,"The degree of description is %f\n", similarity);
		//ins_intList(&IL, qint, similarity);
		freeAtrList(&intentL);
	}
	fclose(fr);
	freeEntlist(&userL);

	system("pause");
    return 0;
}
