---
title: Glover Actor
layout: default
---

# Glover Actor

The Glover Actor is located at memory address `0x802902D8` on US NTSC.

Generally the struct looks like this:

<span style="color:red">Note the actor definition is incomplete. Help by expanding it!</span>

```c
    typedef struct glover_actor {
        struct glover_actor *pnext;
        struct glover_actor *pprev;
        u32 unknown_1[7]; // padding
        u32 visible_flag; // actor will render if nonzero. usually set to 64 or FF
        u32 actor_flags; // some flags for actors, e.g. for glover the 0th byte == 05 is on ball
        union {
            struct glover_actor *ball_attached; // for ball
            u32 unused;
        };
        u32 unknown_2[1];
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
```
