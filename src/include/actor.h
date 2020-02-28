#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "typedefs.h"
#include "memory.h"

/*
 * This file contains an incomplete actor struct
 * with lots of unknown data points
 */


typedef struct actor_properties {
    u8 padding[0x60];
} actor_properties;

typedef struct actor_collision {
    u8 padding[0x74];
    float hitbox_size;
    u32 unknown_1;
} actor_collision;

typedef struct model_properties {
    u8 padding[0x50];
} model_properties;

// size == F0
typedef struct glover_actor {
    struct glover_actor *pnext;
    struct glover_actor *pprev;
    u32 unknown_1[7]; // padding
    u32 visible_flag; // actor will render if nonzero. usually set to 64 or FF
    u32 unknown_2[3];
    float xpos;
    float ypos;
    float zpos;
    float prev_xpos;
    float prev_ypos;
    float prev_zpos;

    u32 unknown_3[3];
    float velx;
    float gravity;
    float vely;
    float velz;
    u32 unknown_4[5];
    float angle_velocity;
    u32 unknown_5[2];
    float angle;
    u32 unkown_6[12];
    u32 scalex;
    u32 scaley;
    u32 scalez;

    u32 *pmodel_data; // TODO create struct for this
    actor_properties *pproperties; // TODO find better name
    u32 *unknown_ptr_1;
    u32 *unknown_ptr_2;
    actor_collision *pcollision;
    model_properties *pmodelproperties;

    u32 unknown_7[2];
    u32 *unknown_ptr_3;
    u32 *unknown_ptr_4;
} glover_actor;


#endif
