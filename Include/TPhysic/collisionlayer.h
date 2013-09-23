#ifndef COLLISIONLAYER_H
# define COLLISIONLAYER_H

# define COLI (1)
# define TRIG (1 << 1)
# define BLOK (1 << 2)

static char layers[5][5] =
{           //STATIC      TRIGGER    DYNAMIC    PROJECT  SUBWORLD
/*STATIC */   {0,           0,         BLOK,      COLI,     BLOK},
/*TRIGGER*/   {0,           0,         TRIG,         0,        0},
/*DYNAMIC*/   {BLOK,        TRIG,      BLOK,      COLI,     TRIG},
/*PROJECT*/   {COLI,        0,         COLI,      COLI,     TRIG},
/*SUBWORL*/   {BLOK,        0,         TRIG,      TRIG,     BLOK}
};

typedef enum {
    STATIC,
    TRIGGER,
    DYNAMIC,
    PROJECTILE,
    SUBWORLD
} collision_layers;

#endif
