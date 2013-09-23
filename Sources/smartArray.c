#include <stdlib.h>
#include "smartArray.h"

t_smart_array		*newSmartArray(int size)
{
	t_smart_array	*smartArray;

	smartArray = (t_smart_array *)malloc(sizeof(t_smart_array));
	smartArray->object_array = (void **)malloc(size * sizeof(void*));
	smartArray->size = size;
	smartArray->first_free_slot = 0;
	smartArray->last_free_slot = 0;
	smartArray->object_qty = 0;
	smartArray->object_array[0] = 0;

	return smartArray;
}

void				putInSmartArray(t_smart_array *smartArray, void *data)
{
	if (data == 0)
		return;
	smartArray->object_array[smartArray->first_free_slot] = data;
	smartArray->object_qty++;
	if (smartArray->last_free_slot < smartArray->object_qty)
	{
		smartArray->last_free_slot++;
		smartArray->object_array[smartArray->last_free_slot] = 0;
		smartArray->first_free_slot = smartArray->last_free_slot;
	}
	else
	{
		if (smartArray->object_qty <= smartArray->last_free_slot)
		{
			while (1){
				smartArray->first_free_slot++;
				if (smartArray->object_array[smartArray->first_free_slot] == NULL)
					break;
			}
		}
	}
}

void				removeFromSmartArray(t_smart_array *smartArray, int objectId)
{
	if (objectId >= smartArray->last_free_slot || smartArray->object_array[objectId] == 0)
		return;
	smartArray->object_array[objectId] = 0;
	if (smartArray->first_free_slot > objectId)
		smartArray->first_free_slot = objectId;
	if (smartArray->last_free_slot - 1 == objectId)
		smartArray->last_free_slot--;
	smartArray->object_qty--;
}

void				removeValueFromSmartArray(t_smart_array *smartArray, void *data)
{
	int i;

	for (i = 0; i < smartArray->last_free_slot; i++)
	{
		if (smartArray->object_array[i] == data)
			removeFromSmartArray(smartArray, i);
	}
}

void				deleteSmartArray(t_smart_array *smartArray)
{
	free(smartArray->object_array);
	free(smartArray);
}
