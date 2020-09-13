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

// polygon type
// each byte is a normalized direction vector from the origin
// TODO verify that they are just a v3 of s32
typedef struct model_polygon_t {
    s16 v1;
} model_polygon_t;

/**
 * Polygon struct for model
 * TODO incomplete size
 * This struct makes no sense.
 * Whoever is reading this please be warned. This way lies madness.
 * One day we will render triangles with this, but what will I have to give in return?
 */
typedef struct model_props_t {
    char padding[34];
    HWORD_T *pcolor; // pointer to tint color. just one hword
    char padding2[0x9C-34-sizeof(HWORD_T*)]; // lots of proprty values here

    // seems like data between this struct and polygon ptr is related to model as well.
    // dynamic array kinda of deal? where is the damn size?
    // Rectang has it at offset 0x8C. still no size??
    model_polygon_t *polygons; // pointer to polygon array offset 0x9C (or 0x8C?) (what is the size though?)
} model_props_t;

/**
 * Model entries 0x40 bytes
 * TODO incomplete
 */
typedef struct model_entry_t {
    u32 unknown0;
    char name[0xC]; // GBOdy = glover body object. hubs also have a 'Rectang' object loaded. good for research
    u32 offset; // offset from center or previous?
    void *unknown_ptr1;
    model_props_t *pmodel_props; // color properties. amount of polygons stored here too?
    WORD_T *pmodel_transforms; // position and transform.
    collision_polygon *ppositions; // relative position of polygons?
    WORD_T *ppolygons; // not polygons at all. see model_props_t
    u32 padding[3]; // not sure yet
    // 2 pointers to next object difference seems to be origin of where they attach
    struct model_entry_t *pnext1; // next object
    struct model_entry_t *pnext2; // another pnext? TODO what's the difference?
    u32 padding2[2]; // unsure
} model_entry_t;

typedef struct obj_bank_t {
    char name[12]; // name of object in bank, if first byte is 0 end of bank
    BYTE_T *pdata; // ptr to data of object
    u32 size; // size of data
} obj_bank_t;


// 60 bytes
// PAct_Anim.t
typedef struct actor_properties {
    u8 padding[0x60];
} actor_properties;

/**
 * Platform properties object
 * seems to be holding switch states
 * 0x60 bytes
 */
typedef struct platform_t {
    BYTE_T padding[0x60];
} platform_t;

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
    u8 padding[0x4C];
    u32 animation_state; // animation state (offset?)
} model_properties;

// TODO needs better name
// changes how animations play
// 50 bytes
// ObjBank.mp
typedef struct model_other {
    u8 padding[0x50];
} model_other;

// model data of actor
// TODO size is likely wrong!
typedef struct model_data_t {
    char padding[0xC]; // unknown data
    model_entry_t *pmodel_start; // ptr to start of model tree
} model_data_t;

// model act_shad_t
// 0x40 size
typedef struct act_shad {
    char padding[0x40];
} act_shad_t;

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
    float velz;
    u32 unknown_4[6];
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

    model_data_t *pmodel_data; // TODO create struct for this
    actor_properties *pproperties; // PAct_Anim.t
    model_other *pother; // ObjBank.mp
    act_shad_t *act_shad; // Act_Shad.t (40 bytes)
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

//
typedef struct model_data_list_t {
   WORD_T flags; // not sure
   model_data_t *pmodel_data; // can be swapped with any model
} model_data_list_t;

/**
 * Object Sprite 
 * 0x40 bytes
 */
typedef struct obj_sprite_t {
   struct obj_sprite_t *pnext;
   struct obj_sprite_t *pprev;
   void *unknown1;
   float xpos;
   float ypos;
   float zpos;
   short xscale;
   short yscale;
   u32 color;
   u32 offset;
   BYTE_T padding[0x1C];
} obj_sprite_t;

#endif
