#ifndef MEM_H
# define MEM_H

#include <stdlib.h>
#include <string.h>
#include "Util.h"

typedef struct s_vector
{
    void        **data;
    size_t      limit;
    size_t      current;
}               t_vector;

t_vector            *vector_create();
t_vector            *vector_create_sized(int size);
t_vector            *vector_init(t_vector *result, int size);

void                vector_append(t_vector *target, void *toadd);
bool                vector_contains(t_vector *target, void *contains);
bool                vector_remove(t_vector *target, void *toremove);
bool                vector_remove_index(t_vector *target, int toremove);
void                vector_delete(t_vector **target);
void                vector_clear(t_vector *target);

#endif
