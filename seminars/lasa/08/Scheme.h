#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
	Integer,
	String,
	List,
	Nil
} nodeType;


/**
recursively concats all string nodeTypes
*/
char *ConcatAll(nodeType *list);
int SumAll(nodeType *list);