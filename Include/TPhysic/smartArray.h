#ifndef __SMARTARRAY_H__
#define __SMARTARRAY_H__

typedef struct	    s_smart_array
{
	int				size;
	int				first_free_slot;
	int				last_free_slot;
	int				object_qty;
	void			**object_array;

}			    	t_smart_array;


t_smart_array		*newSmartArray(int size);
void				putInSmartArray(t_smart_array *smartArray, void *data);
void				removeFromSmartArray(t_smart_array *smartArray, int objectId);
void				removeValueFromSmartArray(t_smart_array *smartArray, void *data);
void				deleteSmartArray(t_smart_array *smartArray);
#endif