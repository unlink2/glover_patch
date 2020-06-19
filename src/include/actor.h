#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "typedefs.h"
#include "memory.h"
#include "matrix.h"

/*
 * This file contains an incomplete actor struct
 * with lots of unknown data points
 */


typedef struct obj_bank_t {
    char name[12]; // name of object in bank, if first byte is 0 end of bank
    BYTE_T *pdata; // ptr to data of object
    u32 size; // size of data
} obj_bank_t;

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
// PAct_Anim.t
typedef struct actor_properties {
    u8 padding[0x60];
} actor_properties;

// 190 bytes
// PActBeh18.p
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
    u8 unknown_2[0x114];
} actor_collision;

// 50 bytes (Verified in Object Bank)
// ACTOR_COL
typedef struct model_properties {
    u8 padding[0x50];
} model_properties;

// TODO needs better name
// changes how animations play
// 50 bytes
// ObjBank.mp
typedef struct model_other {
    u8 padding[0x50];
} model_other;

// size == F0
// LAND_ACTOR
typedef struct glover_actor {
    struct glover_actor *pnext;
    struct glover_actor *pprev;
    u32 unknown_1[7]; // padding
    u32 visible_flag; // actor will render if nonzero. usually set to 64 or FF
    u32 actor_flags; // some flags for actors, e.g. for glover the 0th byte == 05 is on ball
    u32 unknown_2[2];
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
    u32 unkown_6[5];
    float model_rotatex;
    float model_rotatey;
    float model_rotatez;
    u32 unknown_8[4];
    u32 scalex;
    u32 scaley;
    u32 scalez;

    u32 *pmodel_data; // TODO create struct for this
    actor_properties *pproperties; // PAct_Anim.t
    model_other *pother; // ObjBank.mp
    u32 *unknown_ptr_2; // Act_Shad.t (40 bytes)
    actor_collision *pcollision; // PActBeh18.p
    model_properties *pmodelproperties; // ACTOR_COL

    u32 unknown_7[2];
    u32 *unknown_ptr_3; // 40 bytes size?
    u32 *unknown_ptr_4;
} glover_actor;

typedef struct camera_t {
    BYTE_T buffer1[52];
    float xpos;
    float ypos;
    float zpos;
    BYTE_T buffer2[416];
} camera_t;


/**
 * Conditions? part of switches
 * 0x10 bytes long
 * puzzle.nes
 */
typedef struct puzzle_t {
    void *porigin1; // they just point back to original address of ptr?
    void *porigin2;
    WORD_T flags; // ?
    WORD_T unknown;
} puzzle_t;

/**
 * Action object (physical switch?)
 * 0x30 bytes
 */
typedef struct condition_t {
    BYTE_T padding[0x30]; // TODO what is in this struct?
} condition_t;

/**
 * Any push button like event
 * 0x50 bytes long
 */
typedef struct switch_t {
    struct switch_t *pnext;
    struct switch_t *pprev;
    puzzle_t *ppuzzle1; // 2 puzzles?
    puzzle_t *ppuzzle2;
    WORD_T unknown_1[4]; // some flags?
    condition_t *pcond1; // 2 conditions?
    condition_t *pcond2;
    BYTE_T padding[0x50-sizeof(WORD_T*)*10];
} switch_t;

/**
 * Garib
 * 0xB0 bytes
 * TODO
 */
typedef struct garib_t {
    struct garib_t *pnext;
    struct garib_t *pprev;
    BYTE_T padding[0xB0-sizeof(WORD_T*)*2];
} garib_t;

#endif
