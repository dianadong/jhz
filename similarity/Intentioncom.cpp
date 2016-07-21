#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"Intentioncom.h"
////////////////////////////////////////////////////////////////
///function:  意图ID及对应描述程度放入链表IL
//parameter:
//author: 
//time: 2016-6-6
//////////////////////////////////////////////////////

int ins_intList(intentionList *IL, const char * intention, double score)
{
	intentionList pn, pk, pr;
	int kbit, ln;

	if(intention == NULL ||*intention == 0)
		return 0;

	pr = NULL;
	pk = *IL;
	while(pk != NULL)
	{
		kbit = -1;
		kbit = strcmp(intention, pk->intention);
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
		pn = (INNODE*)malloc(sizeof(INNODE));
		if(pn != NULL)
        {
			ln = 0;
			ln = strlen(intention);

			pn->intention = (char *)malloc(ln+1);
			strncpy(pn->intention, intention, ln);
			*(pn->intention +ln) = 0;

			pn->score = score;
			pn->pnext = NULL;

			if(pr == NULL)
			{
				pn->pnext = *IL;
				*IL = pn;
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
