#include "APS_Temperature.h"
#include "APSList.h"
#include <stdio.h>
#include <stdlib.h>

void Destroy_Temp(Temp* temp) {
	if ((temp->predBGs).IOB != NULL)
	{
		for (unsigned int i = 0; i < temp->predBGs.IOB->length; i++)
			if(temp->predBGs.IOB->elements && temp->predBGs.IOB->elements[i])
				free(temp->predBGs.IOB->elements[i]);
		List_Destroy(temp->predBGs.IOB);
		temp->predBGs.IOB = NULL;
	}
	if ((temp->predBGs).COB != NULL)
	{
		for (unsigned int i = 0; i < temp->predBGs.COB->length; i++)
			if (temp->predBGs.COB->elements && temp->predBGs.COB->elements[i])
				free(temp->predBGs.COB->elements[i]);
		List_Destroy(temp->predBGs.COB);
		temp->predBGs.COB = NULL;
	}
}
