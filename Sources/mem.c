#include "mem.h"
#include <stdlib.h>

t_vector            *vector_create()
{
    return vector_create_sized(10);
}

t_vector            *vector_create_sized(int size)
{
    if (size <= 0)
        size = 1;

    t_vector *result = malloc(sizeof(t_vector));
    result->data = malloc(sizeof(void *) * size);
    result->current = 0;
    result->limit = size;

    return result;
}

t_vector            *vector_init(t_vector *result, int size)
{
    if (size <= 0)
        size = 1;

    result->data = malloc(sizeof(void *) * size);
    result->current = 0;
    result->limit = size;

    return result;
}

void                vector_append(t_vector *target, void *toadd)
{
    if (target->current + 1 >= target->limit)
    {
        target->limit *= 2;
		target->data = realloc(target->data, sizeof(void *) * target->limit);
    }
    target->data[target->current++] = toadd;
}

bool                vector_contains(t_vector *target, void *contains)
{
    int i;

    for (i = 0; i < target->current; i++)
    {
        if (target->data[i] == contains)
            return true;
    }
    return false;
}

bool                vector_remove(t_vector *target, void *toremove)
{
    int i;

    for (i = 0; i < target->current; i++)
    {
        if (target->data[i] == toremove)
        {
            target->current--;
            for (; i < target->current; i++)
            {
                target->data[i] = target->data[i + 1];
            }
            return true;
        }
    }
    return false;
}

bool                vector_remove_index(t_vector *target, int toremove)
{
    if (toremove < target->current)
    {
        target->current--;
        for (; toremove < target->current; toremove++)
        {
            target->data[toremove] = target->data[toremove + 1];
        }
        return true;
    }
    return false;
}

void                vector_clear(t_vector *target)
{
    target->current = 0;
}


void                vector_delete(t_vector **target)
{
    free((*target)->data);
    free(*target);
    *target = 0;
}
