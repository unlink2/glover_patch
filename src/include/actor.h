#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "typedefs.h"
#include "memory.h"
#include "matrix.h"

/*
 * This file contains an incomplete actor struct
 * with lots of unknown data points
 */


// struct from colwav.py
// TODO verify. This has not been verified to be correct yet
// 78 bytes
typedef struct collision_polygon {
    u8 padding[16];
    vector3 pos1; // tris of polygon
    vector3 pos2;
    vector3 pos3;
    u8 unknown_1[68];
} collision_polygon;

// 60 bytes
typedef struct actor_properties {
    u8 padding[0x60];
} actor_properties;

// 7C bytes
typedef struct actor_collision {
    collision_polygon *pcollision_floor; // TODO make struct
    collision_polygon *pcollision_wall;
    u32 unknown1;
    u32 unknown2;
    u32 floor_collision_x;
    u32 floor_collision_y;
    u32 floor_collision_z;
    u32 wall_collision_x;
    u32 wall_collision_y;
    u32 wall_collision_z;
    u8 padding[0x48];
    float hitbox_real; // hitbox real approaches hitbox_size slowly
    float hitbox_size;
    u32 unknown_1;
} actor_collision;

// 50 bytes
typedef struct model_properties {
    u8 padding[0x50];
} model_properties;

// TODO needs better name
// changes how animations play
// 50 bytes 
typedef struct model_other {
    u8 padding[0x50];
} model_other;

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
    model_other *pother;
    u32 *unknown_ptr_2;
    actor_collision *pcollision;
    model_properties *pmodelproperties;

    u32 unknown_7[2];
    u32 *unknown_ptr_3; // 40 bytes size?
    u32 *unknown_ptr_4;
} glover_actor;


#endif
