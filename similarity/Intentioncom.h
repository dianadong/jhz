#ifndef _INTENTION_H_
#define _INTENTION_H_


typedef struct IN_node
{
	char * intention;       //“‚Õº ID
	double score;           //√Ë ˆ≥Ã∂»
	struct IN_node * pnext;
}INNODE, * intentionList;

int ins_intList(intentionList *IL, const char * intention, double score);


#endif