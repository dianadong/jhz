#ifndef _INTENTION_H_
#define _INTENTION_H_


typedef struct IN_node
{
	char * intention;       //��ͼ ID
	double score;           //�����̶�
	struct IN_node * pnext;
}INNODE, * intentionList;

int ins_intList(intentionList *IL, const char * intention, double score);


#endif