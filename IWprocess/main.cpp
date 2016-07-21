////////////////////////////
//意图词整理  形式：词：<场景 意图ID 权重> <场景 意图ID 权重> ......
///////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> 
#include "sharfuct.h"
#include "EntityRecog.h"

//#define INTENTSET "intentset.txt"
//#define INTENTSET "1111.txt"
int main()
{
	char *INTENTSET = "H:\\intwords\\intentset.txt";
	char *filepath = "H:\\IWprocess\\wordset.txt";  

	//加权之后的意图集合
	FILE *FI = fopen(INTENTSET, "r");
	if (FI == NULL)
	{
		printf("ERROR:Can not open the intentionset file!\n");
		return 0;
	}


	char * pbuf, *p, *s, *q, *qscene,*m;
	char *psenckey, *partk, *number1, *number2, *scene, *word, *intent;
	int len, ires = 0,location = 0, score, fval, intid;
	char temp[256];
	long lkbit;

	pbuf = (char *)malloc(1 << 14);
	*pbuf = 0;
	scene = (char*)malloc(1 << 14);//场景
	*scene = NULL;
	intent = (char *)malloc(1 << 14);//意图
	*intent = NULL;
	psenckey = (char *)malloc(1 << 14);
	*psenckey = NULL;
	m = (char *)malloc(1 << 14);
	*m = NULL;


	qscene = NULL;
	word = NULL;
	partk = NULL;
	number1 = NULL;
	number2 = NULL;

	siattlist intentL = NULL;
	hashtable ewhash = NULL, wsvhash = NULL;
	
	Init_mghash(&ewhash);
	Init_mghash(&wsvhash);
	
	while (fgets(pbuf, 1 << 14, FI) && strlen(pbuf) > 1)   //过滤掉各种换行符等其他符号，避免因此使得pbuf不为空而进入循环出错
	{
		printf("###################\n");
		printf("%s\n",pbuf);                         //整个意图下的词实体及其属性
		q = pbuf;
		p = NULL;
		p = strchr(pbuf, 0x09);                    //查找首次出现\t的位置，找出意图ID

		len = p - q;

		temp[0] = 0;
		strncpy(temp, q, len);
		temp[len] = 0;
		qscene = temp;//

		sourceqiefen(&qscene, '#', scene);//','
		qscene = qscene + 3;
		sourceqiefen(&qscene, 0x09, intent);
		intid = atoi(intent);
		//printf("scene = %s,intent = %d\n",scene,intid);
		p++;
		while (*p != 0 && strlen(p) > 1)
		{
			ires = -1;
			s = NULL;
			s = strchr(p, 0x09);

			len = 0;
			len = s - p;

			temp[0] = 0;
			strncpy(temp, p, len);
			temp[len] = 0;
			word = temp;


			if (word != NULL & strlen(word)>0)
			{
				sourceqiefen(&word, 0x2C, psenckey);//','
				sourceqiefen(&word, 0x2C, m);
				partk = m;
				
				sourceqiefen(&word, 0x2C, m);
				number1 = m;
				fval = atoi(number1);
				sourceqiefen(&word, 0x09, m);
				number2 = m;
				score = atof(number2);
				printf("word=%s ", psenckey);
			}
			
			ires = GetHashmg(wsvhash, psenckey);//从大表中查找

			if (ires >= 0)
			{

				location = GetHashmg(wsvhash, psenckey);//从大表中查找;
				ires = Insertsiattlist(&(wsvhash + location)->slist, scene, intid, score);
				     
			}
			else
			{
				ires = Ins4Hashmg(&wsvhash, psenckey, scene, intid, score);
				
			}
			p = s + 1;
			

		}
	}
	
	

	writ_mghash(wsvhash, filepath);
	free_mghash(&wsvhash);
	
	free(pbuf);
	pbuf = NULL;

	free(m);
	m = NULL;

	free(intent);
	intent = NULL;
	free(scene);
	scene = NULL;
	
	free(psenckey);
	psenckey = NULL;

	system("pause");
    return 0;
}
